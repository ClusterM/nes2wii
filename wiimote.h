#ifndef wiimote_h

#include "defines.h"
#include <string.h>
#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>
#include <avr/interrupt.h>

// initialize wiimote interface with id, starting data, and calibration data
void wm_init(unsigned char *, unsigned char *, unsigned char *, void (*)(void));

// set button data
void wm_newaction(unsigned char *);

#define wiimote_h
#endif
