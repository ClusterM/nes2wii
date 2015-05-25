#include "nes2wii.h"
#include "defines.h"
#include <util/delay.h>
#include <avr/eeprom.h>
#include <inttypes.h>
#include "wiimote.h"
#include "gamepad.h"
#include <avr/wdt.h>

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

void wiimote_query()
{
	wdt_reset();
	RED_ON;
}

int main()
{
	RED_LED_PORT_DDR |= (1<<RED_LED_PIN); // Red led, output
	GREEN_LED_PORT_DDR |= (1<<GREEN_LED_PIN); // Red led, output
#ifdef DETECT_PORT_DDR
	WII_DISCONNECT;
	DETECT_PORT_DDR |= (1<<DETECT_PIN);
#endif	
#ifdef N64_ENABLED
	init_n64_gamepad();
#endif
#ifdef SNES_ENABLED
	init_snes_gamepad();
#endif
#ifdef NES_ENABLED
	init_nes_gamepad();
#endif
#ifdef SMD_ENABLED
	init_smd_gamepad();
#endif
#ifdef DUALSHOCK_ENABLED
	init_dualshock_gamepad();
#endif

	unsigned char but_dat[6]; // struct containing button data

	uint8_t mode = eeprom_read_byte((void*)0); // current mode
	if (mode > 2) mode = 0;
	int mode_change_timer = 0; // buttons combination hold time

	uint8_t connected = 0;

	while(1)
	{
		but_dat[0] = 0b01011111; // RX<4:3>	LX<5:0>
		but_dat[1] = 0b11011111; // RX<2:1>	LY<5:0>
		but_dat[2] = 0b10001111; // RX<0>	LT<4:3>	RY<4:0>
		but_dat[3] = 0b00000000; // LT<2:0>	RT<4:0>
		but_dat[4] = 0b11111111; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
		but_dat[5] = 0b11111111; // BZL	BB	BY	BA	BX	BZR	BDL	BDU

		if (!connected)
		{
			_delay_ms(1000);
			connected = 1;
			WII_CONNECT;
			wm_init((void*)classic_controller_id, but_dat, (void*)cal_data, wiimote_query);
			wdt_enable(WDTO_1S);
		}
		
		int x = 0;
		int y = 0;
		int b, c;
		int mode_change_counter = 0; // how many spetial buttons pressed
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
#ifdef DUALSHOCK_ENABLED
		uint8_t dualshock_data[9];
		if (get_dualshock_gamepad(dualshock_data, 9, 0, 0))
		{
			for (b = 0; b < 8; b++)
			{
				if (!((dualshock_data[3]>>b)&1))
				{
					GREEN_ON;
					switch (b)
					{
						case 0:
							PRESS_SELECT;
							break;
						case 1: // L3
							PRESS_HOME;
							PRESS_SELECT;
							break;
						case 2: // R3
							PRESS_HOME;
							PRESS_START;
							break;
						case 3:
							PRESS_START;
							break;
						case 4:
							PRESS_UP;
							break;
						case 5:
							PRESS_RIGHT;
							break;
						case 6:
							PRESS_DOWN;
							break;
						case 7:
							PRESS_LEFT;
							break;
					}
				}
			}
			for (b = 0; b < 8; b++)
			{
				if (!((dualshock_data[4]>>b)&1))
				{
					GREEN_ON;
					switch (b)
					{
						case 0: // L2
							PRESS_ZL;
							break;
						case 1: // R2
							PRESS_ZR
							break;
						case 2: // L1
							PRESS_L;
							break;
						case 3: // R1
							PRESS_R;
							break;
						case 4: // Triangle
							PRESS_X;
							break;
						case 5: // O
							PRESS_A;
							break;
						case 6: // X
							PRESS_B;
							break;
						case 7: // Square
							PRESS_Y;
							break;
					}
				}
			}
			if (dualshock_data[1]>>4 == 0x07) // Analog mode
			{
				x = (dualshock_data[7]-128)*30/128;
				y = -(dualshock_data[8]-128)*30/128;
				if (x < 7 && x > -7) x = 0; // Dead zone X
				if (y < 7 && y > -7) y = 0; // Dead zone Y
				uint8_t x2 = dualshock_data[5]>>3;
				uint8_t y2 = (0xFF-dualshock_data[6])>>3;
				x2 &= ~1; // Dead zone X
				y2 &= ~1; // Dead zone y
				
				but_dat[0] &= ~0b11000000; // RX<4:3>	LX<5:0>
				but_dat[1] &= ~0b11000000; // RX<2:1>	LY<5:0>		
				but_dat[2] &= ~0b10011111; // RX<0>	LT<4:3>	RY<4:0>				
				but_dat[0] |= ((x2>>3)&0b11)<<6;
				but_dat[1] |= ((x2>>1)&0b11)<<6;
				but_dat[2] |= (x2&1)<<7;
				but_dat[2] |= y2;
			}
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
						mode_change_counter++;
						break;
					case 1: // Y
						PRESS_Y;
						break;
					case 2: // Select
						PRESS_SELECT;
						break;
					case 3: // Start
						PRESS_START;
						mode_change_counter++;
						break;
					case 4: // Up
						if ((mode == 0) || (mode == 1)) y = 30;
						if ((mode == 0) || (mode == 2)) PRESS_UP;
						break;
					case 5: // Down
						if ((mode == 0) || (mode == 1)) y = -30;
						if ((mode == 0) || (mode == 2)) PRESS_DOWN;
						break;
					case 6: // Left
						if ((mode == 0) || (mode == 1)) x = -30;
						if ((mode == 0) || (mode == 2)) PRESS_LEFT;
						break;
					case 7: // Right
						if ((mode == 0) || (mode == 1)) x = 30;
						if ((mode == 0) || (mode == 2)) PRESS_RIGHT;
						break;
					case 8: // A
						PRESS_A;
						mode_change_counter++;
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
						mode_change_counter++;
						break;
					case 1: // B
						PRESS_B;
						mode_change_counter++;
						break;
					case 2: // Select
						PRESS_SELECT;
						break;
					case 3: // Start
						PRESS_START;
						mode_change_counter++;
						break;
					case 4: // Up
						if ((mode == 0) || (mode == 1)) y = 30;
						if ((mode == 0) || (mode == 2)) PRESS_UP;
						break;
					case 5: // Down
						if ((mode == 0) || (mode == 1)) y = -30;
						if ((mode == 0) || (mode == 2)) PRESS_DOWN;
						break;
					case 6: // Left
						if ((mode == 0) || (mode == 1)) x = -30;
						if ((mode == 0) || (mode == 2)) PRESS_LEFT;
						break;
					case 7: // Right
						if ((mode == 0) || (mode == 1)) x = 30;
						if ((mode == 0) || (mode == 2)) PRESS_RIGHT;
						break;
				}
			}
		}
#endif
#ifdef SMD_ENABLED		
		char smd_present = 0;
		for (c = 0; c < 4; c++)
		{
			uint16_t smd_gamepad_data = get_smd_gamepad();
			if ((smd_gamepad_data & 0b00001111) || (c < 2)) // 3-button mode
			{
				for (b = 0; b <= 13; b++)
				{
					if (!((smd_gamepad_data>>b)&1))
					{
						smd_present = 1; // Coontroller is connected
						switch (b)
						{
							case 0: // Up
								if ((mode == 0) || (mode == 1)) y = 30;
								if ((mode == 0) || (mode == 2)) PRESS_UP;
								GREEN_ON;
								break;
							case 1: // Down
								if ((mode == 0) || (mode == 1)) y = -30;
								if ((mode == 0) || (mode == 2)) PRESS_DOWN;
								GREEN_ON;
								break;
							case 4: // A(SMD)/Y(Classic)
								PRESS_Y;
								GREEN_ON;
								if (c == 0) mode_change_counter++;
								break;
							case 5: // Start
								PRESS_START;
								GREEN_ON;
								if (c == 0) mode_change_counter++;
								break;
							case 10: // Left
								if ((mode == 0) || (mode == 1)) x = -30;
								if ((mode == 0) || (mode == 2)) PRESS_LEFT;
								GREEN_ON;
								break;
							case 11: // Right
								if ((mode == 0) || (mode == 1)) x = 30;
								if ((mode == 0) || (mode == 2)) PRESS_RIGHT;
								GREEN_ON;
								break;
							case 12: // B(SMD)/B(Classic)
								PRESS_B;
								GREEN_ON;
								if (c == 0) mode_change_counter++;
								break;
							case 13: // C(SMD)/A(Classic)
								PRESS_A;
								GREEN_ON;
								break;
						}
					}
				}
				if (!smd_present) break; // Saving time
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
		if (mode_change_counter == 3) // A+B+Start pressed?
		{
			mode_change_timer++;
			if (mode_change_timer >= 2000) // For a long time?
			{
				mode_change_timer = 0;
				mode++; // Changing current mode...
				if (mode > 2) mode = 0;
				eeprom_write_byte((void*)0, mode);
				// Blinking green led indicating new mode...
				GREEN_OFF;
				_delay_ms(500);
				int m;
				for (m = 0; m <= mode; m++)
				{
					GREEN_ON;
					_delay_ms(200);
					GREEN_OFF;
					_delay_ms(200);
				}
				_delay_ms(1000);
			}
		} else mode_change_timer = 0;
	
		_delay_ms(1);
	}
	return 0;
}
