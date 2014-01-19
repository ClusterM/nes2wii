#define F_CPU 20000000L

#define twi_port PORTC
#define twi_ddr DDRC
#define twi_scl_pin 0
#define twi_sda_pin 1

#define N64_ENABLED
#define N64_PORT PORTD
#define N64_PORT_DDR DDRD
#define N64_PORT_PIN PIND
#define N64_DATA_PIN 3

#define NES_ENABLED
#define NES_PORT PORTA
#define NES_PORT_DDR DDRA
#define NES_PORT_PIN PINA
#define NES_LATCH_PIN 0
#define NES_CLOCK_PIN 1
#define NES_DATA_PIN 2

#define SNES_ENABLED
#define SNES_PORT PORTA
#define SNES_PORT_DDR DDRA
#define SNES_PORT_PIN PINA
#define SNES_LATCH_PIN 4
#define SNES_CLOCK_PIN 5
#define SNES_DATA_PIN 6

#define SMD_ENABLED
#define SMD_SELECT_PORT PORTA
#define SMD_SELECT_PORT_DDR DDRA
#define SMD_SELECT_PIN 7
#define SMD_DATA_PORT PORTC
#define SMD_DATA_PORT_DDR DDRC
#define SMD_DATA_PORT_PIN PINC
#define SMD_DATA0_PIN 2
#define SMD_DATA1_PIN 3
#define SMD_DATA2_PIN 4
#define SMD_DATA3_PIN 5
#define SMD_DATA4_PIN 6
#define SMD_DATA5_PIN 7
