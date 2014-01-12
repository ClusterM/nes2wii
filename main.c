#include "defines.h"
#include <util/delay.h>
#include "wiimote.h"

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

void wiimote_query()
{
}

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

int main()
{
	NES_PORT_DDR |= 1<<NES_LATCH_PIN; // Latch
	NES_PORT_DDR |= 1<<NES_CLOCK_PIN; // Clock
	NES_PORT |= 1<<NES_DATA_PIN; // Data, pull-up

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
		but_dat[0] = 0b01011111; // RX<4:3>	LX<5:0>
		but_dat[1] = 0b11011111; // RX<2:1>	LY<5:0>
		but_dat[2] = 0b10001111; // RX<0>	LT<4:3>	RY<4:0>
		but_dat[3] = 0b00000000; // LT<2:0>	RT<4:0>
		but_dat[4] = 0b11111111; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
		but_dat[5] = 0b11111111; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
		uint8_t gamepad_data = get_nes_gamepad();
		int b;
		for (b = 0; b < 8; b++)
		{
			if (!((gamepad_data>>b)&1))
			{
				switch (b)
				{
					case 0: // A
						but_dat[5] &= 0b11101111; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
						break;
					case 1: // B
						but_dat[5] &= 0b10111111; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
						break;
					case 2: // Select
						but_dat[4] &= 0b11101111; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
						break;
					case 3: // Start
						but_dat[4] &= 0b11111011; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
						break;
					case 4: // Up
						//but_dat[5] &= 0b11111110; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
						but_dat[1] += 30;
						break;
					case 5: // Down
						//but_dat[4] &= 0b10111111; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
						but_dat[1] -= 30;
						break;
					case 6: // Left
						//but_dat[5] &= 0b11111101; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
						but_dat[0] -= 30;
						break;
					case 7: // Right
						//but_dat[4] &= 0b01111111; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
						but_dat[0] += 30;
						break;
				}
			}
		}		
		wm_newaction(but_dat);
	}
	return 0;
}

