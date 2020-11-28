#include "defines.h"
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>
#include "gamepads.h"
//#include "usart.h"

struct nes_state nes1;
struct nes_state nes2;
struct nes_state nes3;
struct nes_state nes4;
struct snes_state snes1;
struct snes_state snes2;
struct smd_state smd1;
struct smd_state smd2;
struct dualshock_state dualshock1;
struct dualshock_state dualshock2;
struct n64_state n64_1;
struct n64_state n64_2;

void gamepads_init()
{
#ifdef NES_SNES_PORT
	PORT(NES_SNES_PORT) |= (1 << NES_SNES_LATCH_PIN);
	DDR(NES_SNES_PORT) |= (1 << NES_SNES_LATCH_PIN);
	PORT(NES_SNES_PORT) |= (1 << NES_SNES_CLOCK_PIN);
	DDR(NES_SNES_PORT) |= 1 << NES_SNES_CLOCK_PIN;
#ifdef NES1_DATA_PIN
	DDR(NES_SNES_PORT) &= ~(1 << NES1_DATA_PIN);
	PORT(NES_SNES_PORT) |= (1 << NES1_DATA_PIN);
#endif
#ifdef NES2_DATA_PIN
	DDR(NES_SNES_PORT) &= ~(1 << NES2_DATA_PIN);
	PORT(NES_SNES_PORT) |= (1 << NES2_DATA_PIN);
#endif
#ifdef SNES1_DATA_PIN
	PORT(NES_SNES_PORT) |= (1 << SNES1_DATA_PIN);
	DDR(NES_SNES_PORT) &= ~(1 << SNES1_DATA_PIN);
#endif
#ifdef SNES2_DATA_PIN
	PORT(NES_SNES_PORT) |= (1 << SNES2_DATA_PIN);
	DDR(NES_SNES_PORT) &= ~(1 << SNES2_DATA_PIN);
#endif
#endif
#ifdef SMD_SELECT_PORT
	PORT(SMD_SELECT_PORT) |= (1 << SMD_SELECT_PIN);
	DDR(SMD_SELECT_PORT) |= (1 << SMD_SELECT_PIN);
#endif
#ifdef SMD1_DATA_PORT
	DDR(SMD1_DATA_PORT) &= ~(1 << SMD1_DATA_PIN0);
	DDR(SMD1_DATA_PORT) &= ~(1 << SMD1_DATA_PIN1);
	DDR(SMD1_DATA_PORT) &= ~(1 << SMD1_DATA_PIN2);
	DDR(SMD1_DATA_PORT) &= ~(1 << SMD1_DATA_PIN3);
	DDR(SMD1_DATA_PORT) &= ~(1 << SMD1_DATA_PIN4);
	DDR(SMD1_DATA_PORT) &= ~(1 << SMD1_DATA_PIN5);
	PORT(SMD1_DATA_PORT) |= (1 << SMD1_DATA_PIN0);
	PORT(SMD1_DATA_PORT) |= (1 << SMD1_DATA_PIN1);
	PORT(SMD1_DATA_PORT) |= (1 << SMD1_DATA_PIN2);
	PORT(SMD1_DATA_PORT) |= (1 << SMD1_DATA_PIN3);
	PORT(SMD1_DATA_PORT) |= (1 << SMD1_DATA_PIN4);
	PORT(SMD1_DATA_PORT) |= (1 << SMD1_DATA_PIN5);
#endif
#ifdef SMD2_DATA_PORT
	DDR(SMD2_DATA_PORT) &= ~(1 << SMD2_DATA_PIN0);
	DDR(SMD2_DATA_PORT) &= ~(1 << SMD2_DATA_PIN1);
	DDR(SMD2_DATA_PORT) &= ~(1 << SMD2_DATA_PIN2);
	DDR(SMD2_DATA_PORT) &= ~(1 << SMD2_DATA_PIN3);
	DDR(SMD2_DATA_PORT) &= ~(1 << SMD2_DATA_PIN4);
	DDR(SMD2_DATA_PORT) &= ~(1 << SMD2_DATA_PIN5);
	PORT(SMD2_DATA_PORT) |= (1 << SMD2_DATA_PIN0);
	PORT(SMD2_DATA_PORT) |= (1 << SMD2_DATA_PIN1);
	PORT(SMD2_DATA_PORT) |= (1 << SMD2_DATA_PIN2);
	PORT(SMD2_DATA_PORT) |= (1 << SMD2_DATA_PIN3);
	PORT(SMD2_DATA_PORT) |= (1 << SMD2_DATA_PIN4);
	PORT(SMD2_DATA_PORT) |= (1 << SMD2_DATA_PIN5);
#endif
#ifdef DUALSHOCK_PORT
	PORT(DUALSHOCK_PORT) |= (1 << DUALSHOCK_CMD_PIN); // Command pin - login high
	DDR(DUALSHOCK_PORT) |= (1 << DUALSHOCK_CMD_PIN); // Command pin - output
	DDR(DUALSHOCK_PORT) &= ~(1 << DUALSHOCK_DATA_PIN);   // Data pin - input
	PORT(DUALSHOCK_PORT) |= (1 << DUALSHOCK_DATA_PIN);   // Data pin - pull-up
	PORT(DUALSHOCK_PORT) |= (1 << DUALSHOCK_CLOCK_PIN);  // Clock - logic high	
	DDR(DUALSHOCK_PORT) |= (1 << DUALSHOCK_CLOCK_PIN);   // Clock - output
#ifdef DUALSHOCK1_ATT_PIN
	PORT(DUALSHOCK_PORT) |= (1 << DUALSHOCK1_ATT_PIN); // Attention - logic high	
	DDR(DUALSHOCK_PORT) |= (1 << DUALSHOCK1_ATT_PIN); // Attention - output
#endif
#ifdef DUALSHOCK2_ATT_PIN
	PORT(DUALSHOCK_PORT) |= (1 << DUALSHOCK2_ATT_PIN); // Attention - logic high	
	DDR(DUALSHOCK_PORT) |= (1 << DUALSHOCK2_ATT_PIN); // Attention - output
#endif
#endif
#ifdef N64_PORT
	TCCR0 |= _BV(CS00); // Timer 
#ifdef N64_1_DATA_PIN
	DDR(N64_PORT) &= ~(1 << N64_1_DATA_PIN); // Input
	PORT(N64_PORT) |= (1 << N64_1_DATA_PIN); // Pull-up
#endif
#ifdef N64_2_DATA_PIN
	DDR(N64_PORT) &= ~(1 << N64_2_DATA_PIN); // Input
	PORT(N64_PORT) |= (1 << N64_2_DATA_PIN); // Pull-up
#endif
#endif
	memset((void*)&nes1, 0, sizeof(nes1));
	memset((void*)&nes2, 0, sizeof(nes2));
	memset((void*)&nes3, 0, sizeof(nes3));
	memset((void*)&nes4, 0, sizeof(nes4));
	memset((void*)&snes1, 0, sizeof(snes1));
	memset((void*)&snes2, 0, sizeof(snes2));
	memset((void*)&smd1, 0, sizeof(smd1));
	memset((void*)&smd2, 0, sizeof(smd2));
	memset((void*)&dualshock1, 0, sizeof(dualshock1));
	memset((void*)&dualshock2, 0, sizeof(dualshock2));
	memset((void*)&n64_1, 0, sizeof(n64_1));
	memset((void*)&n64_2, 0, sizeof(n64_2));
}

#ifdef DUALSHOCK_PORT
int8_t dualshock_command(uint8_t* command, uint8_t* data, int length, uint8_t controller_number)
{
	int8_t result = 1;
	if (!controller_number)
	{
#ifdef DUALSHOCK1_ATT_PIN
		PORT(DUALSHOCK_PORT) &= ~(1 << DUALSHOCK1_ATT_PIN); // Attention!
#endif
	} else {
#ifdef DUALSHOCK2_ATT_PIN
		PORT(DUALSHOCK_PORT) &= ~(1 << DUALSHOCK2_ATT_PIN); // Attention!
#endif
	}
	_delay_us(10);
	uint8_t b, bit;
	for (b = 0; b < length; b++) // Each byte...
	{
		uint8_t d = 0;
		for (bit = 0; bit < 8; bit++)
		{
			if ((command[b] >> bit) & 1) // 1?
				PORT(DUALSHOCK_PORT) |= (1 << DUALSHOCK_CMD_PIN); // Command pin - login high
			else
				PORT(DUALSHOCK_PORT) &= ~(1 << DUALSHOCK_CMD_PIN); // Command pin - login low
			PORT(DUALSHOCK_PORT) &= ~(1 << DUALSHOCK_CLOCK_PIN);  // Clock - logic low
			_delay_us(10);
			if ((PIN(DUALSHOCK_PORT) >> DUALSHOCK_DATA_PIN) & 1) // Reading data... 1?
				d |= (1<<bit); // 1!
			PORT(DUALSHOCK_PORT) |= (1 << DUALSHOCK_CLOCK_PIN);  // Clock - logic high
			_delay_us(10);
		}		
		if (b == 1 && d == 0xFF) // Alternative device detection
		{
			result = 0;
			break;
		}
		if (data)
		{
			*data = d;
			data++;
		}
	}
	if (!controller_number)
	{
#ifdef DUALSHOCK1_ATT_PIN
		PORT(DUALSHOCK_PORT) |= (1 << DUALSHOCK1_ATT_PIN); // No attention...
#endif
	} else {
#ifdef DUALSHOCK2_ATT_PIN
		PORT(DUALSHOCK_PORT) |= (1 << DUALSHOCK2_ATT_PIN); // No attention...
#endif
	}
	_delay_us(20);
	return result;
}

uint8_t get_dualshock_gamepad(uint8_t* data, int size, uint8_t motor_small, uint8_t motor_large, uint8_t controller_number)
{
	static uint8_t dualshock_configered[2] = {0, 0};
	uint8_t command_config_mode[5] = {0x01, 0x43, 0x00, 0x01, 0x00};
	uint8_t command_analog_mode[9] = {0x01, 0x44, 0x00, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00};
	uint8_t command_config_motors[9] = {0x01, 0x4D, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t command_config_pressure[9] = {0x01, 0x4F, 0x00, 0xFF, 0xFF, 0x03, 0x00, 0x00, 0x00};
	uint8_t command_config_mode_exit[8] = {0x01, 0x43, 0x00, 0x00, 0xFF, 0xFF, 0xFF, 0xFF};
	uint8_t command_query[21] = {0x01, 0x42, 0, motor_small, motor_large, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	if (!dualshock_configered[controller_number]) // Need to reconfigure dualshock
	{
		if (!dualshock_command(command_config_mode, 0, sizeof(command_config_mode), controller_number)) return 0;
		if (!dualshock_command(command_analog_mode, 0, sizeof(command_analog_mode), controller_number)) return 0;
		if (!dualshock_command(command_config_motors, 0, sizeof(command_config_motors), controller_number)) return 0;
		if (!dualshock_command(command_config_mode_exit, 0, sizeof(command_config_mode_exit), controller_number)) return 0;
		dualshock_configered[controller_number] = 1;
	}

	if (!dualshock_command(command_query, data, size, controller_number))
	{
		dualshock_configered[controller_number] = 0;
		return 0;
	}

	if (data[1] == 0x73) // Analog but without pressure
	{
		if (!dualshock_command(command_config_mode, 0, sizeof(command_config_mode), controller_number)) return 0;
		if (!dualshock_command(command_config_pressure, 0, sizeof(command_config_pressure), controller_number)) return 0;
		if (!dualshock_command(command_config_mode_exit, 0, sizeof(command_config_mode_exit), controller_number)) return 0;
		if (!dualshock_command(command_query, data, size, controller_number))
		{
			dualshock_configered[controller_number] = 0;
			return 0;
		}
	}

	return 1;
}

void dualshock_decode(uint8_t controller_number, struct dualshock_state* dualshock)
{
	uint8_t dualshock_data[21];
	if (get_dualshock_gamepad(dualshock_data, sizeof(dualshock_data), 0, 0, controller_number))
	{
		dualshock->connected = 1;
		dualshock->analog = (dualshock_data[1] & 0xF0) == 0x70;
		dualshock->pressure = (dualshock_data[1] & 0xFF) == 0x79;
		if (!dualshock->analog)	memset((void*)&dualshock_data[5], 0, 4);
		if (!dualshock->pressure) memset((void*)&dualshock_data[9], 0, 12);
		dualshock_data[3] = ~dualshock_data[3];
		dualshock_data[4] = ~dualshock_data[4];
		dualshock_data[5] = (int8_t)(dualshock_data[5]-0x80);
		dualshock_data[6] = (int8_t)(dualshock_data[6]-0x80);
		dualshock_data[7] = (int8_t)(dualshock_data[7]-0x80);
		dualshock_data[8] = (int8_t)(dualshock_data[8]-0x80);
		memcpy((void*)dualshock, (void*)&dualshock_data[3], 18);
	} else {
		memset((void*)dualshock, 0, sizeof(struct dualshock_state));
	}
}
#endif

uint8_t get_nintendo64_gamepad(uint32_t* data, uint8_t controller_number)
{
	uint8_t b;
	uint8_t n64_times[33];
	uint32_t n64_data = 0;
	if (!controller_number)
	{
#ifdef N64_1_DATA_PIN
		N64SEND_0(1); N64SEND_0(1); N64SEND_0(1); N64SEND_0(1); N64SEND_0(1); N64SEND_0(1); N64SEND_0(1); N64SEND_1(1); N64SEND_STOP(1);
		for (b = 0; b < 33; b++)
		{
			TCNT0 = 0;
			while (!N64SIGNAL(1)) if (TCNT0 >= 0xF0) return 0;
			TCNT0 = 0;
			while(N64SIGNAL(1)) if (TCNT0 >= 0xF0) return 0;
			n64_times[b] = TCNT0;
		}
#endif
	} else {
#ifdef N64_2_DATA_PIN
		N64SEND_0(2); N64SEND_0(2); N64SEND_0(2); N64SEND_0(2); N64SEND_0(2); N64SEND_0(2); N64SEND_0(2); N64SEND_1(2); N64SEND_STOP(2);
		for (b = 0; b < 33; b++)
		{
			TCNT0 = 0;
			while (!N64SIGNAL(2)) if (TCNT0 >= 0xF0) return 0;
			TCNT0 = 0;
			while(N64SIGNAL(2)) if (TCNT0 >= 0xF0) return 0;
			n64_times[b] = TCNT0;
		}
#endif
	}

	for (b = 0; b < 32; b++)
	{
		n64_data <<= 1;
		n64_data |= ((n64_times[b] / (F_CPU / 1000000UL) < 2) ? 1 : 0);
	}

	((uint8_t*)&n64_data)[0] *= -1;
	*data = n64_data;
	return 1;
}

void gamepads_query()
{
	uint8_t b;
#ifdef NES_SNES_PORT
	uint16_t nes1_d = 0;
	uint16_t nes2_d = 0;
	uint16_t snes1_d = 0;
	uint16_t snes2_d = 0;
	PORT(NES_SNES_PORT) |= (1 << NES_SNES_LATCH_PIN); // Latch
	_delay_us(10);
	PORT(NES_SNES_PORT) &= ~(1 << NES_SNES_LATCH_PIN); // Latch
	_delay_us(10);
	for (b = 0; b < 12; b++)
	{
		PORT(NES_SNES_PORT) &= ~(1 << NES_SNES_CLOCK_PIN); // Clock
		_delay_us(10);
		if (b < 8)
		{
#ifdef NES1_DATA_PIN
			nes1_d <<= 1;
			nes1_d |= (((PIN(NES_SNES_PORT)>>NES1_DATA_PIN)&1)^1);
#endif
#ifdef NES2_DATA_PIN
			nes2_d <<= 1;
			nes2_d |= (((PIN(NES_SNES_PORT)>>NES2_DATA_PIN)&1)^1);
#endif
		}
#ifdef SNES1_DATA_PIN
		snes1_d <<= 1;
		snes1_d |= (((PIN(NES_SNES_PORT)>>SNES1_DATA_PIN)&1)^1);
#endif
#ifdef SNES2_DATA_PIN
		snes2_d <<= 1;
		snes2_d |= (((PIN(NES_SNES_PORT)>>SNES2_DATA_PIN)&1)^1);
#endif
		PORT(NES_SNES_PORT) |= (1 << NES_SNES_CLOCK_PIN); // Clock
		_delay_us(10);
	}
	memcpy((void*)&nes1, (void*)&nes1_d, sizeof(nes1));
	memcpy((void*)&nes2, (void*)&nes2_d, sizeof(nes2));
	memcpy((void*)&snes1, (void*)&snes1_d, sizeof(snes1));
	memcpy((void*)&snes2, (void*)&snes2_d, sizeof(snes2));
#endif

#ifdef SMD_SELECT_PORT
#ifdef SMD1_DATA_PORT
	uint8_t sega_d1[10] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
#endif
#ifdef SMD2_DATA_PORT
	uint8_t sega_d2[10] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
#endif

	PORT(SMD_SELECT_PORT) |= (1 << SMD_SELECT_PIN);
	for (b = 0; b < 10; b++)
	{
		_delay_us(10);
#ifdef SMD1_DATA_PORT
		sega_d1[b] = ((PIN(SMD1_DATA_PORT)>>SMD1_DATA_PIN0)&1) 
			| (((PIN(SMD1_DATA_PORT)>>SMD1_DATA_PIN1)&1) << 1)
			| (((PIN(SMD1_DATA_PORT)>>SMD1_DATA_PIN2)&1) << 2)
			| (((PIN(SMD1_DATA_PORT)>>SMD1_DATA_PIN3)&1) << 3)
			| (((PIN(SMD1_DATA_PORT)>>SMD1_DATA_PIN4)&1) << 4)
			| (((PIN(SMD1_DATA_PORT)>>SMD1_DATA_PIN5)&1) << 5)
			| (((PIN(SMD_SELECT_PORT)>>SMD_SELECT_PIN)&1) << 7);
#endif
#ifdef SMD2_DATA_PORT
		sega_d2[b] = ((PIN(SMD2_DATA_PORT)>>SMD2_DATA_PIN0)&1) 
			| (((PIN(SMD2_DATA_PORT)>>SMD2_DATA_PIN1)&1) << 1)
			| (((PIN(SMD2_DATA_PORT)>>SMD2_DATA_PIN2)&1) << 2)
			| (((PIN(SMD2_DATA_PORT)>>SMD2_DATA_PIN3)&1) << 3)
			| (((PIN(SMD2_DATA_PORT)>>SMD2_DATA_PIN4)&1) << 4)
			| (((PIN(SMD2_DATA_PORT)>>SMD2_DATA_PIN5)&1) << 5);
#endif
		PORT(SMD_SELECT_PORT) ^= (1 << SMD_SELECT_PIN);
	}
#ifdef SMD1_DATA_PORT
	smd1.connected = !(sega_d1[1] & 0b001100);
	if (smd1.connected)
	{
		smd1.six_buttons = !(sega_d1[5] & 0b001111) && !(~sega_d1[7] & 0b001111);
		smd1.a = !!(~sega_d1[1] & 0b010000);
		smd1.b = !!(~sega_d1[0] & 0b010000);
		smd1.c = !!(~sega_d1[0] & 0b100000);
		smd1.x = !!(smd1.six_buttons && (~sega_d1[6] & 0b000100));
		smd1.y = !!(smd1.six_buttons && (~sega_d1[6] & 0b000010));
		smd1.z = !!(smd1.six_buttons && (~sega_d1[6] & 0b000001));
		smd1.up = !!(~sega_d1[0] & 0b000001);
		smd1.down = !!(~sega_d1[0] & 0b000010);
		smd1.left = !!(~sega_d1[0] & 0b000100);
		smd1.right = !!(~sega_d1[0] & 0b001000);
		smd1.start = !!(~sega_d1[1] & 0b100000);
		smd1.mode = !!(smd1.six_buttons && (~sega_d1[6] & 0b001000));
	} else {
		smd1.six_buttons = 0;
		smd1.a = 0;
		smd1.b = 0;
		smd1.c = 0;
		smd1.x = 0;
		smd1.y = 0;
		smd1.z = 0;
		smd1.up = 0;
		smd1.down = 0;
		smd1.left = 0;
		smd1.right = 0;
		smd1.start = 0;
		smd1.mode = 0;
	}
#endif
#ifdef SMD2_DATA_PORT
	smd2.connected = !(sega_d2[1] & 0b001100);
	if (smd2.connected)
	{
		smd2.six_buttons = !(sega_d2[5] & 0b001111) && !(~sega_d2[7] & 0b001111);
		smd2.a = !!(~sega_d2[1] & 0b010000);
		smd2.b = !!(~sega_d2[0] & 0b010000);
		smd2.c = !!(~sega_d2[0] & 0b100000);
		smd2.x = !!(smd2.six_buttons && (~sega_d2[6] & 0b000100));
		smd2.y = !!(smd2.six_buttons && (~sega_d2[6] & 0b000010));
		smd2.z = !!(smd2.six_buttons && (~sega_d2[6] & 0b000001));
		smd2.up = !!(~sega_d2[0] & 0b000001);
		smd2.down = !!(~sega_d2[0] & 0b000010);
		smd2.left = !!(~sega_d2[0] & 0b000100);
		smd2.right = !!(~sega_d2[0] & 0b001000);
		smd2.start = !!(~sega_d2[1] & 0b100000);
		smd2.mode = !!(smd2.six_buttons && (~sega_d2[6] & 0b001000));
	} else {
		smd2.six_buttons = 0;
		smd2.a = 0;
		smd2.b = 0;
		smd2.c = 0;
		smd2.x = 0;
		smd2.y = 0;
		smd2.z = 0;
		smd2.up = 0;
		smd2.down = 0;
		smd2.left = 0;
		smd2.right = 0;
		smd2.start = 0;
		smd2.mode = 0;
	}
#endif
#endif

#ifdef SMD_USE_DENDY9_PIN
	uint16_t nes3_d = 0;
	uint16_t nes4_d = 0;
#ifdef SMD1_DATA_PORT
	if (!smd1.connected)
	{
		DDR(SMD1_DATA_PORT) |= (1 << SMD1_DATA_PIN4); // Data 4 aka VCC, output
		DDR(SMD1_DATA_PORT) |= (1 << SMD1_DATA_PIN2); // Data 2 aka latch, output
		DDR(SMD1_DATA_PORT) |= (1 << SMD1_DATA_PIN3); // Data 3 aka clock, output
		PORT(SMD1_DATA_PORT) |= (1 << SMD1_DATA_PIN2); // Latch
		_delay_us(10);
		PORT(SMD1_DATA_PORT) &= ~(1 << SMD1_DATA_PIN2); // Latch
		_delay_us(10);
		for (b = 0; b < 8; b++)
		{
			PORT(SMD1_DATA_PORT) &= ~(1 << SMD1_DATA_PIN3); // Clock
			_delay_us(10);
			nes3_d <<= 1;
			nes3_d |= (((PIN(SMD1_DATA_PORT)>>SMD1_DATA_PIN1)&1)^1);
			PORT(SMD1_DATA_PORT) |= (1 << SMD1_DATA_PIN3); // Clock
		}		
		PORT(SMD1_DATA_PORT) |= (1 << SMD1_DATA_PIN2); // Latch
		DDR(SMD1_DATA_PORT) &= ~(1 << SMD1_DATA_PIN2); // Input
		DDR(SMD1_DATA_PORT) &= ~(1 << SMD1_DATA_PIN3); // Input
		DDR(SMD1_DATA_PORT) &= ~(1 << SMD1_DATA_PIN4); // Input
	}
#endif
#ifdef SMD2_DATA_PORT
	if (!smd2.connected)
	{
		DDR(SMD2_DATA_PORT) |= (1 << SMD2_DATA_PIN4); // Data 4 aka VCC, output
		DDR(SMD2_DATA_PORT) |= (1 << SMD2_DATA_PIN2); // Data 2 aka latch, output
		DDR(SMD2_DATA_PORT) |= (1 << SMD2_DATA_PIN3); // Data 3 aka clock, output
		PORT(SMD2_DATA_PORT) |= (1 << SMD2_DATA_PIN2); // Latch
		_delay_us(10);
		PORT(SMD2_DATA_PORT) &= ~(1 << SMD2_DATA_PIN2); // Latch
		_delay_us(10);
		for (b = 0; b < 8; b++)
		{
			PORT(SMD2_DATA_PORT) &= ~(1 << SMD2_DATA_PIN3); // Clock
			_delay_us(10);
			nes4_d <<= 1;
			nes4_d |= (((PIN(SMD2_DATA_PORT)>>SMD2_DATA_PIN1)&1)^1);
			PORT(SMD2_DATA_PORT) |= (1 << SMD2_DATA_PIN3); // Clock
		}		
		PORT(SMD2_DATA_PORT) |= (1 << SMD2_DATA_PIN2); // Latch
		DDR(SMD2_DATA_PORT) &= ~(1 << SMD2_DATA_PIN2); // Input
		DDR(SMD2_DATA_PORT) &= ~(1 << SMD2_DATA_PIN3); // Input
		DDR(SMD2_DATA_PORT) &= ~(1 << SMD2_DATA_PIN4); // Input
	}
#endif
	memcpy((void*)&nes3, (void*)&nes3_d, sizeof(nes3));
	memcpy((void*)&nes4, (void*)&nes4_d, sizeof(nes4));
#endif

#ifdef DUALSHOCK_PORT
#ifdef DUALSHOCK1_ATT_PIN
	dualshock_decode(0, &dualshock1);
#endif
#ifdef DUALSHOCK2_ATT_PIN
	dualshock_decode(1, &dualshock2);
#endif
#endif
//	tx_d(dualshock1.connected); tx_byte(' '); tx_d(dualshock1.analog); tx_byte(' '); tx_d(dualshock1.pressure); tx_byte(' '); tx_hex(dualshock1.lx,1); tx_byte(' '); tx_hex(dualshock1.ly,1); tx_byte(' '); tx_hex(dualshock1.rx,1); tx_byte(' '); tx_hex(dualshock1.ry,1); tx_byte(' '); tx_byte(' '); 
//	tx_d(dualshock1.pressure_cross); tx_byte(' '); tx_d(dualshock1.pressure_square); tx_byte(' '); tx_d(dualshock1.pressure_triangle); tx_byte(' '); tx_d(dualshock1.pressure_circle); tx_byte(' '); tx_byte(' '); 
//	tx_d(dualshock1.cross); tx_byte(' '); tx_d(dualshock1.square); tx_byte(' '); tx_d(dualshock1.triangle); tx_byte(' '); tx_d(dualshock1.circle);
//	tx_str_C("\r\n");

#ifdef N64_PORT
	uint32_t n64_data;
#ifdef N64_1_DATA_PIN
	if (get_nintendo64_gamepad(&n64_data, 0))
		memcpy((void*)&n64_1, (void*)&n64_data, sizeof(n64_1));
#endif
#ifdef N64_2_DATA_PIN
	if (get_nintendo64_gamepad(&n64_data, 1))
		memcpy((void*)&n64_2, (void*)&n64_data, sizeof(n64_2));
#endif
#endif
//	tx_d(smd1.a); tx_byte(' '); tx_d(smd1.b); tx_byte(' '); tx_d(smd1.c); tx_str_C("\r\n");
//	tx_d(n64_1.a); tx_byte(' '); tx_d(n64_1.b); tx_byte(' '); tx_hex(n64_1.x,1); tx_byte(' '); tx_hex(n64_1.y,1); tx_str_C("\r\n");
//	tx_d(nes1.a); tx_byte(' '); tx_d(nes1.b); tx_byte(' '); tx_str_C("\r\n");
//	tx_d(n64_1.c_up); tx_byte(' '); tx_d(n64_1.c_down); tx_byte(' '); tx_d(n64_1.c_left); tx_byte(' '); tx_d(n64_1.c_right); tx_str_C("\r\n");
}
