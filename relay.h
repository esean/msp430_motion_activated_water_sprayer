//
// use motion sensor to switch on water sprayer
//
// (c) esean_us@yahoo.com 6/14/12
//


#include "hw.h"

int relay_init();

// sets relay PWM to 'percent' and for on_time_ticks in WDT ticks (not seconds)
void relay_pulse_relay_pwm(unsigned int percent, unsigned int on_time_ticks);

// returns the last timer tick count when timer was turned off
unsigned long relay_last_turn_off_time();

// returns 1 if relay is currently on, 0 otherwise
unsigned int relay_is_relay_on_now();

// set relay pulse mode: 0=constant on, >0 means pulse more quickly
void relay_set_pulsed_out_to(unsigned int new);

// this fcn is called when the water tube detects contact or not
void relay_hw_water_pressure_changed();

// CB
void relay_callback();

