#include "wiimote.h"
#include "wm_crypto.h"

// pointer to user function
static void (*wm_sample_event)();

// crypto data
static volatile unsigned char wm_rand[10];
static volatile unsigned char wm_key[6];
static volatile unsigned char wm_ft[8];
static volatile unsigned char wm_sb[8];

// virtual register
static volatile unsigned char twi_reg[256];
static volatile unsigned int twi_reg_addr;

static volatile unsigned char twi_first_addr_flag; // set address flag
static volatile unsigned char twi_rw_len; // length of most recent operation

void twi_slave_init(unsigned char addr)
{
	// initialize stuff
	twi_reg_addr = 0;

	// set slave address
	TWAR = addr << 1;
	
	// enable twi module, acks, and twi interrupt
	TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWEA);

	// enable interrupts
	sei();
}

void twi_clear_int(unsigned char ack)
{
	// get ready by clearing interrupt, with or without ack
	if(ack != 0)
	{
		TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT) | _BV(TWEA);
	}
	else
	{
		TWCR = _BV(TWEN) | _BV(TWIE) | _BV(TWINT);
	}
}

/*

I'd like to thank Hector Martin for posting his encryption method!
His website is http://www.marcansoft.com/
Decryption method found at http://www.derkeiler.com/pdf/Newsgroups/sci.crypt/2008-11/msg00110.pdf

*/

unsigned char wm_ror8(unsigned char a, unsigned char b)
{
	// bit shift with roll-over
	return (a >> b) | ((a << (8 - b)) & 0xFF);
}

void wm_gentabs()
{
	unsigned char idx;

	// check all idx
	for(idx = 0; idx < 7; idx++)
	{
		// generate test key
		unsigned char ans[6];
		unsigned char tkey[6];
		unsigned char t0[10];
		
		unsigned char i;
		for(i = 0; i < 6; i++)
		{
			ans[i] = pgm_read_byte(&(ans_tbl[idx][i]));
		}	
		for(i = 0; i < 10; i++)
		{
			t0[i] = pgm_read_byte(&(sboxes[0][wm_rand[i]]));
		}
	
		tkey[0] = ((wm_ror8((ans[0] ^ t0[5]), (t0[2] % 8)) - t0[9]) ^ t0[4]);
		tkey[1] = ((wm_ror8((ans[1] ^ t0[1]), (t0[0] % 8)) - t0[5]) ^ t0[7]);
		tkey[2] = ((wm_ror8((ans[2] ^ t0[6]), (t0[8] % 8)) - t0[2]) ^ t0[0]);
		tkey[3] = ((wm_ror8((ans[3] ^ t0[4]), (t0[7] % 8)) - t0[3]) ^ t0[2]);
		tkey[4] = ((wm_ror8((ans[4] ^ t0[1]), (t0[6] % 8)) - t0[3]) ^ t0[4]);
		tkey[5] = ((wm_ror8((ans[5] ^ t0[7]), (t0[8] % 8)) - t0[5]) ^ t0[9]);

		// compare with actual key
		if(memcmp(tkey, wm_key, 6) == 0) break; // if match, then use this idx
	}

	// generate encryption from idx key and rand
	wm_ft[0] = pgm_read_byte(&(sboxes[idx + 1][wm_key[4]])) ^ pgm_read_byte(&(sboxes[idx + 2][wm_rand[3]]));
	wm_ft[1] = pgm_read_byte(&(sboxes[idx + 1][wm_key[2]])) ^ pgm_read_byte(&(sboxes[idx + 2][wm_rand[5]]));
	wm_ft[2] = pgm_read_byte(&(sboxes[idx + 1][wm_key[5]])) ^ pgm_read_byte(&(sboxes[idx + 2][wm_rand[7]]));
	wm_ft[3] = pgm_read_byte(&(sboxes[idx + 1][wm_key[0]])) ^ pgm_read_byte(&(sboxes[idx + 2][wm_rand[2]]));
	wm_ft[4] = pgm_read_byte(&(sboxes[idx + 1][wm_key[1]])) ^ pgm_read_byte(&(sboxes[idx + 2][wm_rand[4]]));
	wm_ft[5] = pgm_read_byte(&(sboxes[idx + 1][wm_key[3]])) ^ pgm_read_byte(&(sboxes[idx + 2][wm_rand[9]]));
	wm_ft[6] = pgm_read_byte(&(sboxes[idx + 1][wm_rand[0]])) ^ pgm_read_byte(&(sboxes[idx + 2][wm_rand[6]]));
	wm_ft[7] = pgm_read_byte(&(sboxes[idx + 1][wm_rand[1]])) ^ pgm_read_byte(&(sboxes[idx + 2][wm_rand[8]]));
	
	wm_sb[0] = pgm_read_byte(&(sboxes[idx + 1][wm_key[0]])) ^ pgm_read_byte(&(sboxes[idx + 2][wm_rand[1]]));
	wm_sb[1] = pgm_read_byte(&(sboxes[idx + 1][wm_key[5]])) ^ pgm_read_byte(&(sboxes[idx + 2][wm_rand[4]]));
	wm_sb[2] = pgm_read_byte(&(sboxes[idx + 1][wm_key[3]])) ^ pgm_read_byte(&(sboxes[idx + 2][wm_rand[0]]));
	wm_sb[3] = pgm_read_byte(&(sboxes[idx + 1][wm_key[2]])) ^ pgm_read_byte(&(sboxes[idx + 2][wm_rand[9]]));
	wm_sb[4] = pgm_read_byte(&(sboxes[idx + 1][wm_key[4]])) ^ pgm_read_byte(&(sboxes[idx + 2][wm_rand[7]]));
	wm_sb[5] = pgm_read_byte(&(sboxes[idx + 1][wm_key[1]])) ^ pgm_read_byte(&(sboxes[idx + 2][wm_rand[8]]));
	wm_sb[6] = pgm_read_byte(&(sboxes[idx + 1][wm_rand[3]])) ^ pgm_read_byte(&(sboxes[idx + 2][wm_rand[5]]));
	wm_sb[7] = pgm_read_byte(&(sboxes[idx + 1][wm_rand[2]])) ^ pgm_read_byte(&(sboxes[idx + 2][wm_rand[6]]));
}

void wm_slaveTxStart(unsigned char addr)
{
	if(addr >= 0x00 && addr < 0x06)
	{
		// call user event
		wm_sample_event();
	}
}

void wm_slaveRx(unsigned char addr, unsigned char l)
{
	// if encryption data is sent, store them accordingly
	unsigned int i;
	if(addr >= 0x40 && addr < 0x46)
	{
		for(i = 0; i < 6; i++)
		{
			wm_rand[9 - i] = twi_reg[0x40 + i];
		}
	}
	else if(addr >= 0x46 && addr < 0x4C)
	{
		for(i = 6; i < 10; i++)
		{
			wm_rand[9 - i] = twi_reg[0x40 + i];
		}
		for(i = 0; i < 2; i++)
		{
			wm_key[5 - i] = twi_reg[0x40 + 10 + i];
		}
	}
	else if(addr >= 0x4C && addr < 0x50)
	{
		for(i = 2; i < 6; i++)
		{
			wm_key[5 - i] = twi_reg[0x40 + 10 + i];
		}
		if(addr + l == 0x50)
		{
			// generate decryption once all data is loaded
			wm_gentabs();
		}
	}
}

void wm_newaction(unsigned char * d)
{
	// load button data from user application
	memcpy(twi_reg, d, 6);
}

void wm_init(unsigned char * id, unsigned char * t, unsigned char * cal_data, void (*function)(void))
{
	// link user function
	wm_sample_event = function;

	// start state
	wm_newaction(t);
	twi_reg[0xF0] = 0; // disable encryption

	// set id
	unsigned int i, j;
	for(i = 0, j = 0xFA; i < 6; i++, j++)
	{
		twi_reg[j] = id[i];
	}

	// set calibration data
	for(i = 0, j = 0x20; i < 6; i++, j++)
	{
		twi_reg[j] = cal_data[i];
	}

/*
	// initialize device detect pin
	dev_detect_port &= 0xFF ^ _BV(dev_detect_pin);
	dev_detect_ddr |= _BV(dev_detect_pin);
	_delay_ms(500); // delay to simulate disconnect
*/
	// ready twi bus, no pull-ups
	twi_port &= 0xFF ^ _BV(twi_scl_pin);
	twi_port &= 0xFF ^ _BV(twi_sda_pin);

	// start twi slave, link events
	twi_slave_init(0x52);

	// make the wiimote think something is connected
//	dev_detect_port |= _BV(dev_detect_pin);
}

ISR(TWI_vect)
{
	switch(TW_STATUS)
	{
		// Slave Rx
		case TW_SR_SLA_ACK: // addressed, returned ack
		case TW_SR_GCALL_ACK: // addressed generally, returned ack
		case TW_SR_ARB_LOST_SLA_ACK: // lost arbitration, returned ack
		case TW_SR_ARB_LOST_GCALL_ACK: // lost arbitration generally, returned ack
			// get ready to receive pointer
			twi_first_addr_flag = 0;
			// ack
			twi_clear_int(1);
			break;
		case TW_SR_DATA_ACK: // data received, returned ack
		case TW_SR_GCALL_DATA_ACK: // data received generally, returned ack
		if(twi_first_addr_flag != 0)
		{
			// put byte in register
			unsigned char t = TWDR;
			if(twi_reg[0xF0] == 0xAA && twi_reg_addr != 0xF0) // if encryption is on
			{
				// decrypt
				twi_reg[twi_reg_addr] = (t ^ wm_sb[twi_reg_addr % 8]) + wm_ft[twi_reg_addr % 8];
			}
			else
			{
				twi_reg[twi_reg_addr] = t;
			}
			twi_reg_addr++;
			twi_rw_len++;
		}
		else
		{
			// set address
			twi_reg_addr = TWDR;
			twi_first_addr_flag = 1;
			twi_rw_len = 0;
		}
		twi_clear_int(1); // ack
			break;
		case TW_SR_STOP: // stop or repeated start condition received
			// run user defined function
			wm_slaveRx(twi_reg_addr - twi_rw_len, twi_rw_len);
			twi_clear_int(1); // ack future responses
			break;
		case TW_SR_DATA_NACK: // data received, returned nack
		case TW_SR_GCALL_DATA_NACK: // data received generally, returned nack
			twi_clear_int(0); // nack back at master
			break;
		
		// Slave Tx
		case TW_ST_SLA_ACK:	// addressed, returned ack
		case TW_ST_ARB_LOST_SLA_ACK: // arbitration lost, returned ack
			// run user defined function
			wm_slaveTxStart(twi_reg_addr);
			twi_rw_len = 0;
		case TW_ST_DATA_ACK: // byte sent, ack returned
			// ready output byte
			if(twi_reg[0xF0] == 0xAA) // encryption is on
			{
				// encrypt
				TWDR = (twi_reg[twi_reg_addr] - wm_ft[twi_reg_addr % 8]) ^ wm_sb[twi_reg_addr % 8];
			}
			else
			{
				TWDR = twi_reg[twi_reg_addr];
			}
			twi_reg_addr++;
			twi_rw_len++;
			twi_clear_int(1); // ack
			break;
		case TW_ST_DATA_NACK: // received nack, we are done 
		case TW_ST_LAST_DATA: // received ack, but we are done already!
			// ack future responses
			twi_clear_int(1);
			break;
		default:
			twi_clear_int(0);
			break;
	}
}
