#ifndef _NES2WII_H_
#define _NES2WII_H_

#include "defines.h"

#define GLUE(a,b) a##b
#define DDR(p) GLUE(DDR,p)
#define PORT(p) GLUE(PORT,p)
#define PIN(p) GLUE(PIN,p)

#define twi_port PORT(TWI_PORT)
#define twi_ddr DDR(TWI_PORT)
#define twi_scl_pin TWI_SCL_PIN
#define twi_sda_pin TWI_SDA_PIN

#ifdef DETECT_PORT
#define dev_detect_port PORT(DETECT_PORT)
#define dev_detect_ddr DDR(DETECT_PORT)
#define dev_detect_pin DETECT_PIN
#endif

#define RED_LED_PORT_PORT PORT(RED_LED_PORT)
#define RED_LED_PORT_DDR DDR(RED_LED_PORT)
#define GREEN_LED_PORT_PORT PORT(GREEN_LED_PORT)
#define GREEN_LED_PORT_DDR DDR(GREEN_LED_PORT)

#define RED_ON RED_LED_PORT_PORT |= (1<<RED_LED_PIN)
#define RED_OFF RED_LED_PORT_PORT &= ~(1<<RED_LED_PIN)
#define GREEN_ON GREEN_LED_PORT_PORT |= (1<<GREEN_LED_PIN)
#define GREEN_OFF GREEN_LED_PORT_PORT &= ~(1<<GREEN_LED_PIN)

#define ABS(x) (x >= 0 ? x : -x)
#define ABSMAX(a,b) (ABS(a)>ABS(b)?a:b)

#define DPAD_MODE_BOTH 0
#define DPAD_MODE_STICK 1
#define DPAD_MODE_DPAD 2

#endif
