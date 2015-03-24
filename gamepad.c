#include "defines.h"
#include <avr/io.h>
#include <util/delay.h>
#include "gamepad.h"

#ifdef NES_ENABLED

void init_nes_gamepad()
{
	NES_PORT_DDR |= 1<<NES_LATCH_PIN; // Latch, output
	NES_PORT_DDR |= 1<<NES_CLOCK_PIN; // Clock, output
	NES_PORT_DDR &= ~(1<<NES_DATA_PIN); // Data, input
	NES_PORT_PORT |= 1<<NES_DATA_PIN; // Data, pull-up
}

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
#endif

#ifdef SNES_ENABLED
void init_snes_gamepad()
{
	SNES_PORT_DDR |= 1<<SNES_LATCH_PIN; // Latch, output
	SNES_PORT_DDR |= 1<<SNES_CLOCK_PIN; // Clock, output
	SNES_PORT_DDR &= ~(1<<SNES_DATA_PIN); // Data, input
	SNES_PORT_PORT |= 1<<SNES_DATA_PIN; // Data, pull-up
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
#endif

#ifdef N64_ENABLED
void init_n64_gamepad()
{
	TCCR0 |= _BV(CS00); // Timer 
	N64_PORT_DDR &= ~(1<<N64_DATA_PIN); // Input
	N64_PORT_PORT &= ~(1<<N64_DATA_PIN); // No pull-up (using external resistor)
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
#endif

#ifdef SMD_ENABLED
void init_smd_gamepad()
{
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
#endif

#ifdef DUALSHOCK_ENABLED
void init_dualshock_gamepad()
{
	DUALSHOCK_PORT_DDR |= (1<<DUALSHOCK_COMMAND_PIN); // Command pin - output
	DUALSHOCK_PORT_PORT |= (1<<DUALSHOCK_COMMAND_PIN); // Command pin - login high
	DUALSHOCK_PORT_DDR &= ~(1<<DUALSHOCK_DATA_PIN);   // Data pin - input
	DUALSHOCK_PORT_PORT |= (1<<DUALSHOCK_DATA_PIN);   // Data pin - pull-up
	DUALSHOCK_PORT_DDR |= (1<<DUALSHOCK_ATTENTION_PIN); // Attention - output
	DUALSHOCK_PORT_PORT |= (1<<DUALSHOCK_ATTENTION_PIN); // Attention - logic high	
	DUALSHOCK_PORT_DDR |= (1<<DUALSHOCK_CLOCK_PIN);   // Clock - output
	DUALSHOCK_PORT_PORT |= (1<<DUALSHOCK_CLOCK_PIN);  // Clock - logic high	
	/*
	DUALSHOCK_PORT_DDR &= ~(1<<DUALSHOCK_ACK_PIN);   // Ack pin - input
	DUALSHOCK_PORT_PORT |= (1<<DUALSHOCK_ACK_PIN);   // Ack pin - pull-up
	*/
}

int dualshock_command(uint8_t* command, uint8_t* data, int length)
{
	DUALSHOCK_PORT_PORT &= ~(1<<DUALSHOCK_ATTENTION_PIN); // Attention!
	_delay_us(20);
	int b, bit;
	for (b = 0; b < length; b++) // Each byte...
	{
		data[b] = 0;
		for (bit = 0; bit < 8; bit++)
		{
			if ((command[b] >> bit) & 1) // 1?
				DUALSHOCK_PORT_PORT |= (1<<DUALSHOCK_COMMAND_PIN); // 1!
			else DUALSHOCK_PORT_PORT &= ~(1<<DUALSHOCK_COMMAND_PIN); // 0!			
			DUALSHOCK_PORT_PORT &= ~(1<<DUALSHOCK_CLOCK_PIN);  // Clock - logic low
			_delay_us(20);
			if ((DUALSHOCK_PORT_PIN >> DUALSHOCK_DATA_PIN) & 1) // Reading data... 1?
				data[b] |= (1<<bit); // 1!
			DUALSHOCK_PORT_PORT |= (1<<DUALSHOCK_CLOCK_PIN);  // Clock - logic high			
			_delay_us(20);
		}
		if (b == 1 && data[1] == 0xFF) // Alternative device detection
		{
			DUALSHOCK_PORT_PORT |= (1<<DUALSHOCK_ATTENTION_PIN);  // No attention...
			return 0;
		}
		/*
		if (b<length-1) // Waiting for ACK
		{
			int t;
			for (t = 0; t < 50; t++)
			{			
				if (!((DUALSHOCK_PORT_PIN >> DUALSHOCK_ACK_PIN)&1)) // ACK reveived
				{
					ok = 1;
					break;
				}
				_delay_us(1);
			}
			if ((b < 2) && !ok) return 0; // No ACK in first two bytes? Aboooort! Saving time
		}
		*/
	}
	DUALSHOCK_PORT_PORT |= (1<<DUALSHOCK_ATTENTION_PIN);  // No attention...
	_delay_us(20);
	return 1;
}

int get_dualshock_gamepad(uint8_t* data, int size, uint8_t motor_small, uint8_t motor_large) // pointer to uint8_t[21], number of bytes to request, vibration...
{
	static char dualshock_configered = 0;

	uint8_t command_query[21] = {0x01, 0x42, 0, motor_small, motor_large, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	if (!dualshock_command(command_query, data, size))
	{
		dualshock_configered = 0;
		return 0;
	}
	if (!dualshock_configered) // Need to reconfigure dualshock
	{
		uint8_t command_config_mode[5] = {0x01, 0x43, 0x00, 0x01, 0x00};
		if (!dualshock_command(command_config_mode, data, sizeof(command_config_mode))) return 0;
		uint8_t command_analog_mode[9] = {0x01, 0x44, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00};
		if (!dualshock_command(command_analog_mode, data, sizeof(command_analog_mode))) return 0;
		uint8_t command_config_motors[9] = {0x01, 0x4D, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF};
		if (!dualshock_command(command_config_motors, data, sizeof(command_config_motors))) return 0;
		uint8_t command_config_pressure[9] = {0x01, 0x4F, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00};
		if (!dualshock_command(command_config_pressure, data, sizeof(command_config_pressure))) return 0;
		uint8_t command_config_mode_exit[8] = {0x01, 0x43, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
		if (!dualshock_command(command_config_mode_exit, data, sizeof(command_config_mode_exit))) return 0;
		dualshock_configered = 1;
		if (!dualshock_command(command_query, data, size)) return 0;
	}
	return 1;
}
#endif
