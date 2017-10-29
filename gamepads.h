#ifndef _GAMEPAD_H_
#define _GAMEPAD_H_

#include <inttypes.h>
#include "defines.h"

#define GLUE(a,b) a##b
#define DDR(p) GLUE(DDR,p)
#define PORT(p) GLUE(PORT,p)
#define PIN(p) GLUE(PIN,p)

#define WAIT(t) {TCNT0=0; while(TCNT0 < (F_CPU / 1000000UL) * t);}
#define N64SEND(n,t) {PORT(N64_PORT) &= ~(1 << N64_##n##_DATA_PIN); DDR(N64_PORT) |= (1 << N64_##n##_DATA_PIN); WAIT(t); DDR(N64_PORT) &= ~(1 << N64_##n##_DATA_PIN); PORT(N64_PORT) |= (1 << N64_##n##_DATA_PIN);}
#define N64SEND_1(n) {N64SEND(n,1); WAIT(3);}
#define N64SEND_0(n) {N64SEND(n,3); WAIT(1);}
#define N64SEND_STOP(n) {N64SEND(n,1); WAIT(2);}
#define N64SIGNAL(n) (!((PIN(N64_PORT) >> N64_##n##_DATA_PIN)&1))

void gamepads_init();
void gamepads_query();

struct nes_state {
	uint8_t right : 1, left : 1, down : 1, up : 1, start : 1, select : 1, b : 1, a : 1;
};

struct snes_state {
	uint16_t r : 1, l : 1, x : 1, a : 1, right : 1, left : 1, down : 1, up : 1, start : 1, select : 1, y : 1, b : 1;
};

struct smd_state {
	uint16_t connected : 1, six_buttons : 1, a : 1, b : 1, c : 1, x : 1, y : 1, z : 1, start : 1, mode :1, up : 1, down : 1, left : 1, right : 1;
};

struct dualshock_state {
	uint16_t select : 1, l3 : 1, r3 : 1, start : 1, up : 1, right : 1, down : 1, left : 1, l2 : 1, r2 : 1, l1 : 1, r1 : 1, triangle : 1, circle : 1, cross : 1, square : 1;
	int8_t rx, ry, lx, ly;
	uint8_t pressure_right, pressure_left, pressure_up, pressure_down, pressure_triangle, pressure_circle, pressure_cross, pressure_square, pressure_l1, pressure_r1, pressure_l2, pressure_r2;
	uint8_t connected : 1, analog : 1, pressure : 1;
};

struct n64_state {
	int8_t y;
	int8_t x;
	uint8_t c_right : 1, c_left : 1, c_down : 1, c_up : 1, r : 1, l : 1, : 2;
	uint8_t right : 1, left : 1, down : 1, up : 1, start : 1, z : 1, b : 1, a : 1;
};

extern struct nes_state nes1;
extern struct nes_state nes2;
extern struct nes_state nes3;
extern struct nes_state nes4;
extern struct snes_state snes1;
extern struct snes_state snes2;
extern struct smd_state smd1;
extern struct smd_state smd2;
extern struct dualshock_state dualshock1;
extern struct dualshock_state dualshock2;
extern struct n64_state n64_1;
extern struct n64_state n64_2;

#endif
