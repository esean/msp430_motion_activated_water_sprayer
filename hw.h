//
// use motion sensor to switch on water sprayer
//
// (c) esean_us@yahoo.com 6/14/12
//

#include  "msp430g2352.h"
//#include  "msp430g2231.h"

#define     LED0                  BIT0  // P1.0 - led1 on board, red
#define     RELAY                 BIT4  // P1.4
#define     MOTION_INPUT          BIT5  // P1.5
#define     LED1                  BIT6  // P1.6 - led2 on board, green
#define     OFF_SENSOR            BIT7  // P1.7 - mesh netting around tube for off detection, pulled low on contact

#define     TIMER_PWM_PERIOD      16384 // 0.5sec @ 32khz

#if XCODE_BUILD == 1
#define WDT_PARAM             WDT_ADLY_16
#define WDT_CYCLES_PER_SEC    10
#else
#if 0
#define WDT_PARAM             WDT_ADLY_16
#define WDT_CYCLES_PER_SEC    63
#else
#define WDT_PARAM             WDT_ADLY_1_9
#define WDT_CYCLES_PER_SEC    526
#endif
#endif	// XCODE_BUILD==1

#define SEC_2_TICKS(a)        ((a) * WDT_CYCLES_PER_SEC)

// is IR signal input high ?
int hw_see_motion_now();

// is the water turned off, is tube touching side metal?
int hw_is_water_off();

// called by sensor to set HW PWM
void hw_set_relay_pwm_percent(unsigned int percent);

void hw_go_to_sleep();

int hw_init( void );

