#include "nes2wii.h"
#include "defines.h"
#include <util/delay.h>
#include "wiimote.h"

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

volatile int red_led_timer = 0;

uint8_t get_nes_gamepad()
{
	uint8_t gamepad_data = 0;
	NES_PORT_PORT &= ~(1<<NES_LATCH_PIN); // Latch
	int b;
	for (b = 0; b < 8; b++)
	{
		NES_PORT_PORT &= ~(1<<NES_CLOCK_PIN); // Clock
		_delay_us(10);
		gamepad_data |= (((NES_PORT_PIN>>NES_DATA_PIN)&1)<<b);
		NES_PORT_PORT |= 1<<NES_CLOCK_PIN; // Clock
		_delay_us(10);
	}		
	NES_PORT_PORT |= 1<<NES_LATCH_PIN; // Latch
	return gamepad_data;
}

uint16_t get_snes_gamepad()
{
	uint16_t gamepad_data = 0;
	SNES_PORT_PORT &= ~(1<<SNES_LATCH_PIN); // Latch
	int b;
	for (b = 0; b < 16; b++)
	{
		SNES_PORT_PORT &= ~(1<<SNES_CLOCK_PIN); // Clock
		_delay_us(10);
		gamepad_data |= ((uint16_t)((SNES_PORT_PIN>>SNES_DATA_PIN)&1)<<b);
		SNES_PORT_PORT |= 1<<SNES_CLOCK_PIN; // Clock
		_delay_us(10);
	}		
	SNES_PORT_PORT |= 1<<SNES_LATCH_PIN; // Latch
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
			while (!N64SIGNAL) if (TCNT0 >= 0xF0) return 0;
			TCNT0 = 0;
			while(N64SIGNAL) if (TCNT0 >= 0xF0) return 0;
			data[b] = data[b]<<1;
			if (TCNT0 < 0x24 * F_CPU / 20000000UL) data[b] |= 1;
		}
	}
	return 1;
}

uint16_t get_smd_gamepad()
{
	uint8_t gamepad_data_low = 0;
	uint8_t gamepad_data_high = 0;
	SMD_SELECT_PORT_PORT &= ~(1<<SMD_SELECT_PIN); // Select - low
	_delay_us(50);
	gamepad_data_low = ((SMD_DATA_PORT_PIN>>SMD_DATA0_PIN)&1) 
		| (((SMD_DATA_PORT_PIN>>SMD_DATA1_PIN)&1)<<1) 
		| (((SMD_DATA_PORT_PIN>>SMD_DATA2_PIN)&1)<<2)
		| (((SMD_DATA_PORT_PIN>>SMD_DATA3_PIN)&1)<<3)
		| (((SMD_DATA_PORT_PIN>>SMD_DATA4_PIN)&1)<<4)
		| (((SMD_DATA_PORT_PIN>>SMD_DATA5_PIN)&1)<<5);
	SMD_SELECT_PORT_PORT |= 1<<SMD_SELECT_PIN; // Select - high
	_delay_us(50);
	gamepad_data_high = ((SMD_DATA_PORT_PIN>>SMD_DATA0_PIN)&1) 
		| (((SMD_DATA_PORT_PIN>>SMD_DATA1_PIN)&1)<<1) 
		| (((SMD_DATA_PORT_PIN>>SMD_DATA2_PIN)&1)<<2)
		| (((SMD_DATA_PORT_PIN>>SMD_DATA3_PIN)&1)<<3)
		| (((SMD_DATA_PORT_PIN>>SMD_DATA4_PIN)&1)<<4)
		| (((SMD_DATA_PORT_PIN>>SMD_DATA5_PIN)&1)<<5);
	return ((uint16_t)gamepad_data_high<<8) | gamepad_data_low;
}

void wiimote_query()
{
	RED_ON;
	red_led_timer = 0;
}

int main()
{
	RED_LED_PORT_DDR |= (1<<RED_LED_PIN); // Red led, output
	GREEN_LED_PORT_DDR |= (1<<GREEN_LED_PIN); // Red led, output
	RED_ON;
	TCCR0 |= _BV(CS00); // Timer 
#ifdef N64_ENABLED
	N64_PORT_DDR &= ~(1<<N64_DATA_PIN); // Input
	N64_PORT_PORT &= ~(1<<N64_DATA_PIN); // No pull-up (using external resistor)
#endif
#ifdef SNES_ENABLED
	SNES_PORT_DDR |= 1<<SNES_LATCH_PIN; // Latch, output
	SNES_PORT_DDR |= 1<<SNES_CLOCK_PIN; // Clock, output
	SNES_PORT_PORT |= 1<<SNES_DATA_PIN; // Data, pull-up
#endif
#ifdef NES_ENABLED
	NES_PORT_DDR |= 1<<NES_LATCH_PIN; // Latch, output
	NES_PORT_DDR |= 1<<NES_CLOCK_PIN; // Clock, output
	NES_PORT_PORT |= 1<<NES_DATA_PIN; // Data, pull-up
#endif
#ifdef SMD_ENABLED
	SMD_SELECT_PORT_DDR |= 1<<SMD_SELECT_PIN; // Select, output
	SMD_DATA_PORT_DDR &= ~(1<<SMD_DATA0_PIN); // Data 0, input
	SMD_DATA_PORT_DDR &= ~(1<<SMD_DATA1_PIN); // Data 1, input
	SMD_DATA_PORT_DDR &= ~(1<<SMD_DATA2_PIN); // Data 2, input
	SMD_DATA_PORT_DDR &= ~(1<<SMD_DATA3_PIN); // Data 3, input
	SMD_DATA_PORT_DDR &= ~(1<<SMD_DATA4_PIN); // Data 4, input
	SMD_DATA_PORT_DDR &= ~(1<<SMD_DATA5_PIN); // Data 5, input
	SMD_DATA_PORT_PORT |= 1<<SMD_DATA0_PIN; // Data 0, pull-up
	SMD_DATA_PORT_PORT |= 1<<SMD_DATA1_PIN; // Data 1, pull-up
	SMD_DATA_PORT_PORT |= 1<<SMD_DATA2_PIN; // Data 2, pull-up
	SMD_DATA_PORT_PORT |= 1<<SMD_DATA3_PIN; // Data 3, pull-up
	SMD_DATA_PORT_PORT |= 1<<SMD_DATA4_PIN; // Data 4, pull-up
	SMD_DATA_PORT_PORT |= 1<<SMD_DATA5_PIN; // Data 5, pull-up
#endif

	unsigned char but_dat[6]; // struct containing button data
	but_dat[0] = 0b01011111; // RX<4:3>	LX<5:0>
	but_dat[1] = 0b11011111; // RX<2:1>	LY<5:0>
	but_dat[2] = 0b10001111; // RX<0>	LT<4:3>	RY<4:0>
	but_dat[3] = 0b00000000; // LT<2:0>	RT<4:0>
	but_dat[4] = 0b11111111; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
	but_dat[5] = 0b11111111; // BZL	BB	BY	BA	BX	BZR	BDL	BDU

	wm_init((void*)classic_controller_id, but_dat, (void*)cal_data, wiimote_query);

	while(1)
	{
		but_dat[0] = 0b01011111; // RX<4:3>	LX<5:0>
		but_dat[1] = 0b11011111; // RX<2:1>	LY<5:0>
		but_dat[2] = 0b10001111; // RX<0>	LT<4:3>	RY<4:0>
		but_dat[3] = 0b00000000; // LT<2:0>	RT<4:0>
		but_dat[4] = 0b11111111; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
		but_dat[5] = 0b11111111; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
		int x = 0;
		int y = 0;
		int b, c;
		GREEN_OFF;
	
#ifdef N64_ENABLED	
		uint8_t n64_data[4];
		cli();
		int n64_ok = get_n64_gamepad(n64_data);
		sei();
		if (n64_ok)
		{
			int b;
			for (b = 0; b < 8; b++)
			{
				if (((n64_data[0]>>(7-b))&1))
				{
					GREEN_ON;
					switch (b)
					{
						case 0: // A
							PRESS_A;
							break;
						case 1: // B
							PRESS_B;
							break;
						case 2: // Z
							PRESS_ZL;
							break;
						case 3: // Start
							PRESS_START;
							break;
						case 4: // Up
							PRESS_UP;
							break;
						case 5: // Down
							PRESS_DOWN;
							break;
						case 6: // Left
							PRESS_LEFT;
							break;
						case 7: // Right
							PRESS_RIGHT;
							break;
					}
				}
			}
			for (b = 0; b < 8; b++)
			{
				if (((n64_data[1]>>(7-b))&1))
				{
					GREEN_ON;
					switch (b)
					{
						case 2: // L
							PRESS_L;
							break;
						case 3: // R
							PRESS_R;
							break;
						case 4: // C-Up
							but_dat[2] |= 0b00011111; // RX<0>	LT<4:3>	RY<4:0>
							break;
						case 5: // C-Down
							but_dat[2] &= ~0b00011111; // RX<0>	LT<4:3>	RY<4:0>
							break;
						case 6: // C-Left
							but_dat[0] &= ~0b11000000; // RX<4:3>	LX<5:0>
							but_dat[1] &= ~0b11000000; // RX<2:1>	LY<5:0>			
							break;
						case 7: // C-Right
							but_dat[0] |= 0b11000000; // RX<4:3>	LX<5:0>
							but_dat[1] |= 0b11000000; // RX<2:1>	LY<5:0>			
						break;
					}
				}
			}
		
			x = n64_data[2];
			if (x >= 0x80) x = -0x100+x;
			y = n64_data[3];
			if (y >= 0x80) y = -0x100+y;
			x = x * 30 / 80;
			y = y * 30 / 80;
		}
#endif
#ifdef SNES_ENABLED
		uint16_t snes_gamepad_data = get_snes_gamepad();
		for (b = 0; b < 16; b++)
		{
			if (!((snes_gamepad_data>>b)&1))
			{
				GREEN_ON;
				switch (b)
				{
					case 0: // B
						PRESS_B;
						break;
					case 1: // Y
						PRESS_Y;
						break;
					case 2: // Select
						PRESS_SELECT;
						break;
					case 3: // Start
						PRESS_START;
						break;
					case 4: // Up
						y = 30;
						break;
					case 5: // Down
						y = -30;
						break;
					case 6: // Left
						x = -30;
						break;
					case 7: // Right
						x = 30;
						break;
					case 8: // A
						PRESS_A;
						break;
					case 9: // X
						PRESS_X;
						break;
					case 10: // L
						PRESS_L;
						break;
					case 11: // R
						PRESS_R;
						break;
				}
			}
		}
#endif
#ifdef NES_ENABLED
		uint8_t nes_gamepad_data = get_nes_gamepad();
		for (b = 0; b < 8; b++)
		{
			if (!((nes_gamepad_data>>b)&1))
			{
				GREEN_ON;
				switch (b)
				{
					case 0: // A
						PRESS_A;
						break;
					case 1: // B
						PRESS_B;
						break;
					case 2: // Select
						PRESS_SELECT;
						break;
					case 3: // Start
						PRESS_START;
						break;
					case 4: // Up
						y = 30;
						break;
					case 5: // Down
						y = -30;
						break;
					case 6: // Left
						x = -30;
						break;
					case 7: // Right
						x = 30;
						break;
				}
			}
		}
#endif
#ifdef SMD_ENABLED		
		for (c = 0; c < 4; c++)
		{
			uint16_t smd_gamepad_data = get_smd_gamepad();
			if ((smd_gamepad_data & 0b00001111) || (c < 2)) // 3-button mode
			{
				for (b = 0; b <= 13; b++)
				{
					if (!((smd_gamepad_data>>b)&1))
					{
						switch (b)
						{
							case 0: // Up
								y = 30;
								GREEN_ON;
								break;
							case 1: // Down
								y = -30;
								GREEN_ON;
								break;
							case 4: // A(SMD)/Y(Classic)
								PRESS_Y;
								GREEN_ON;
								break;
							case 5: // Start
								PRESS_START;
								GREEN_ON;
								break;
							case 10: // Left
								x = -30;
								GREEN_ON;
								break;
							case 11: // Right
								x = 30;
								GREEN_ON;
								break;
							case 12: // B(SMD)/A(Classic)
								PRESS_A;
								GREEN_ON;
								break;
							case 13: // C(SMD)/B(Classic)
								PRESS_B;
								GREEN_ON;
								break;
						}
					}
				}
			} else { // 6-button mode
				for (b = 4; b <= 11; b++)
				{
					if (!((smd_gamepad_data>>b)&1))
					{
						switch (b)
						{
							case 4: // A(SMD)/Y(Classic)
								PRESS_Y;
								GREEN_ON;
								break;
							case 5: // Start
								PRESS_START;
								GREEN_ON;
								break;
							case 8: // Z(SMD)/R(Classic)
								PRESS_R;
								GREEN_ON;
								break;
							case 9: // Y(SMD)/X(Classic)
								PRESS_X;
								GREEN_ON;
								break;
							case 10: // X(SMD)/L(Classic)
								PRESS_L;
								GREEN_ON;
								break;
							case 11: // Mode(SMD)/Select(Classic)
								PRESS_SELECT;
								GREEN_ON;
							break;
						}
					}
				}
			}
		}
#endif
		but_dat[0] += x;
		but_dat[1] += y;
		wm_newaction(but_dat);
	
		_delay_us(10);
		if (++red_led_timer >= 10) RED_OFF;
	}
	return 0;
}

