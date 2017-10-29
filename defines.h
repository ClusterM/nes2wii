#ifndef _DEFINES_H_
#define _DEFINES_H_

#define F_CPU 16000000L
#define UART_BAUD 9600UL

#define TWI_PORT C
#define TWI_SCL_PIN 0
#define TWI_SDA_PIN 1

#define DETECT_PORT C
#define DETECT_PIN 2

#define NES_SNES_PORT C
#define NES_SNES_LATCH_PIN 3
#define NES_SNES_CLOCK_PIN 4
#define NES1_DATA_PIN 5
#define NES2_DATA_PIN 6
#define SNES1_DATA_PIN 7
//#define SNES2_DATA_PIN

#define SMD_SELECT_PORT A
#define SMD_SELECT_PIN 6
#define SMD1_DATA_PORT A
//#define SMD2_DATA_PORT A
#define SMD1_DATA_PIN0 0
#define SMD1_DATA_PIN1 1
#define SMD1_DATA_PIN2 2
#define SMD1_DATA_PIN3 3
#define SMD1_DATA_PIN4 4
#define SMD1_DATA_PIN5 5

#define SMD_USE_DENDY9_PIN

#define DUALSHOCK_PORT B
#define DUALSHOCK1_ATT_PIN 0
//#define DUALSHOSK2_ATT_PIN 0
#define DUALSHOCK_CLOCK_PIN 1
#define DUALSHOCK_CMD_PIN 2
#define DUALSHOCK_DATA_PIN 3

#define N64_PORT D
#define N64_1_DATA_PIN 2
//#define N64_2_DATA_PIN 2

#define RED_LED_PORT D
#define RED_LED_PIN 6
#define GREEN_LED_PORT D
#define GREEN_LED_PIN 7

#define DEAD_ZONE 32

#endif
