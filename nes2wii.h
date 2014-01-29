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

#define PRESS_A			but_dat[5] &= 0b11101111; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
#define PRESS_B			but_dat[5] &= 0b10111111; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
#define PRESS_X			but_dat[5] &= 0b11110111; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
#define PRESS_Y			but_dat[5] &= 0b11011111; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
#define PRESS_L			but_dat[4] &= 0b11011111; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
#define PRESS_R			but_dat[4] &= 0b11111101; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
#define PRESS_ZL		but_dat[5] &= 0b01111111; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
#define PRESS_ZR		but_dat[5] &= 0b11111011; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
#define PRESS_SELECT 	but_dat[4] &= 0b11101111; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
#define PRESS_START 	but_dat[4] &= 0b11111011; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
#define PRESS_UP		but_dat[5] &= 0b11111110; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
#define PRESS_DOWN		but_dat[4] &= 0b10111111; // BDR	BDD	BLT	B-	BH	B+	BRT	 1
#define PRESS_LEFT		but_dat[5] &= 0b11111101; // BZL	BB	BY	BA	BX	BZR	BDL	BDU
#define PRESS_RIGHT		but_dat[4] &= 0b01111111; // BDR	BDD	BLT	B-	BH	B+	BRT	 1

#define RED_LED_PORT_PORT PORT(RED_LED_PORT)
#define RED_LED_PORT_DDR DDR(RED_LED_PORT)
#define GREEN_LED_PORT_PORT PORT(GREEN_LED_PORT)
#define GREEN_LED_PORT_DDR DDR(GREEN_LED_PORT)

#define RED_ON RED_LED_PORT_PORT |= (1<<RED_LED_PIN);
#define RED_OFF RED_LED_PORT_PORT &= ~(1<<RED_LED_PIN);
#define GREEN_ON GREEN_LED_PORT_PORT |= (1<<GREEN_LED_PIN);
#define GREEN_OFF GREEN_LED_PORT_PORT &= ~(1<<GREEN_LED_PIN);

#endif