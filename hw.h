//
// use motion sensor to switch on water sprayer
//
// (c) esean_us@yahoo.com 6/14/12
//

#include  "msp430g2231.h"

#define     MOTION_INPUT          BIT5  // P1.5
#define     RELAY                 BIT2  // P1.2
#define     LED0                  BIT0  // P1.0 - led1 on board, red
#define     LED1                  BIT6  // P1.6 - led2 on board, green

#define     TIMER_PWM_PERIOD      16384 // 0.5sec @ 32khz

#if 0
#define WDT_PARAM             WDT_ADLY_16
#define WDT_CYCLES_PER_SEC    63
#else
#define WDT_PARAM             WDT_ADLY_1_9
#define WDT_CYCLES_PER_SEC    526
#endif
#define SEC_2_TICKS(a)        ((a) * WDT_CYCLES_PER_SEC)

// is IR signal input high ?
int hw_see_motion_now();

// called by sensor to set HW PWM
void hw_set_relay_pwm_percent(unsigned int percent);

void hw_go_to_sleep();

int hw_init( void );

