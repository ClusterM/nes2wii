#include "defines.h"
#include <util/delay.h>
#include "wiimote.h"

#define WAIT(t) {TCNT0=0; while(TCNT0 < 20 * t);}

#define N64SEND(t) {DDRD |= (1<<3); WAIT(t); DDRD &= ~(1<<3);}
#define N64SEND_1 {N64SEND(1); WAIT(3);}
#define N64SEND_0 {N64SEND(3); WAIT(1);}
#define N64SEND_STOP {N64SEND(1); WAIT(2);}
#define N64SEND_STOP {N64SEND(1); WAIT(2);}
#define N64SIGNAL (!((PIND>>3)&1))

// most of this data is found on
// http://wiibrew.org/wiki/Wiimote/Extension_Controllers

// classic controller id
const unsigned char classic_controller_id[6] = {0x00, 0x00, 0xA4, 0x20, 0x01, 0x01};

// calibration data
const unsigned char cal_data[32] = {
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00
};

uint8_t get_nes_gamepad()
{
	uint8_t gamepad_data = 0;
	NES_PORT &= ~(1<<NES_LATCH_PIN); // Latch
	int b;
	for (b = 0; b < 8; b++)
	{
		NES_PORT &= ~(1<<NES_CLOCK_PIN); // Clock
		_delay_us(10);
		gamepad_data |= (((NES_PORT_PIN>>NES_DATA_PIN)&1)<<b);
		NES_PORT |= 1<<NES_CLOCK_PIN; // Clock
		_delay_us(10);
	}		
	NES_PORT |= 1<<NES_LATCH_PIN; // Latch
	_delay_us(10);
	return gamepad_data;
}

int get_n64_gamepad(uint8_t* data)
{
	int b, bit;
	N64SEND_0; N64SEND_0; N64SEND_0; N64SEND_0; N64SEND_0; N64SEND_0; N64SEND_0; N64SEND_1; N64SEND_STOP;
	for (b = 0; b < 4; b++)
	{
		data[b] = 0;
		for (bit = 0; bit < 8; bit++)
		{		
			TCNT0 = 0;
			while (!N64SIGNAL); if (TCNT0 >= 0xF0) return 0;
			TCNT0 = 0;
			while(N64SIGNAL); if (TCNT0 >= 0xF0) return 0;
			data[b] = data[b]<<1;
			if (TCNT0 < 0x24) data[b] |= 1;
		}
	}
	return 1;
}

void wiimote_query()
{
	unsigned char but_dat[6]; // struct containing button data
	but_dat[0] = 0b01011111; // RX<4:3>	LX<5:0>
	but_dat[1] = 0b11011111; // RX<2:1>	LY<5:0>
	but_dat[2] = 0b10001111; // RX<0>	LT<4:3>	RY<4:0>
	but_dat[3] = 0b00000000; // LT<2:0>	RT<4:0>
	but_dat[4] = 0b11111111; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
	but_dat[5] = 0b11111111; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
	uint8_t data[4];
	if (get_n64_gamepad(data))
	{
		int b;
		for (b = 0; b < 8; b++)
		{
			if (((data[0]>>(7-b))&1))
			{
				switch (b)
				{
					case 0: // A
						but_dat[5] &= 0b11101111; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
						break;
					case 1: // B
						but_dat[5] &= 0b10111111; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
						break;
					case 2: // Z
						but_dat[5] &= 0b01111111; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
						break;
					case 3: // Start
						but_dat[4] &= 0b11111011; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
						break;
					case 4: // Up
						but_dat[5] &= 0b11111110; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
						break;
					case 5: // Down
						but_dat[4] &= 0b10111111; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
						break;
					case 6: // Left
						but_dat[5] &= 0b11111101; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
						break;
					case 7: // Right
						but_dat[4] &= 0b01111111; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
						break;
				}
			}
		}
		for (b = 0; b < 8; b++)
		{
		if (((data[1]>>(7-b))&1))
			{
				switch (b)
				{
					case 2: // L
						but_dat[5] &= 0b11011111; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
						break;
					case 3: // R
						but_dat[4] &= 0b11111101; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
						break;
					case 4: // Up
						but_dat[2] |= 0b00011111; // RX<0>	LT<4:3>	RY<4:0>
						break;
					case 5: // Down
						but_dat[2] &= ~0b00011111; // RX<0>	LT<4:3>	RY<4:0>
						break;
					case 6: // Left
						but_dat[0] &= ~0b11000000; // RX<4:3>	LX<5:0>
						but_dat[1] &= ~0b11000000; // RX<2:1>	LY<5:0>			
						break;
					case 7: // Right
						but_dat[0] |= 0b11000000; // RX<4:3>	LX<5:0>
						but_dat[1] |= 0b11000000; // RX<2:1>	LY<5:0>			
					break;
				}
			}
		}
		
		int x = data[2];
		if (x >= 0x80) x = -0x100+x;
		int y = data[3];
		if (y >= 0x80) y = -0x100+y;
		but_dat[0] += x * 30 / 80;
		but_dat[1] += y * 30 / 80;
		wm_newaction(but_dat);
	}	
}

int main()
{
	TCCR0 |= _BV(CS00); // Таймер 
	DDRD &= ~(1<<3); // На ввод
	PORTD &= ~(1<<3); // Без подтяжки

	unsigned char but_dat[6]; // struct containing button data
	but_dat[0] = 0b01011111; // RX<4:3>	LX<5:0>
	but_dat[1] = 0b11011111; // RX<2:1>	LY<5:0>
	but_dat[2] = 0b10001111; // RX<0>	LT<4:3>	RY<4:0>
	but_dat[3] = 0b00000000; // LT<2:0>	RT<4:0>
	but_dat[4] = 0b11111111; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
	but_dat[5] = 0b11111111; // BZL	BB	BY	BA	BX	BZR	BDL	BDU

	wm_init(classic_controller_id, but_dat, cal_data, wiimote_query);

	while(1)
	{
//		_delay_ms(1);
	}
	return 0;
}

