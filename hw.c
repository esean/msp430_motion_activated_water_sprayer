//
// use motion sensor to switch on water sprayer
//
// (c) esean_us@yahoo.com 6/14/12
//

#include "defs.h"
#include "hw.h"
#include "timer.h"
#include "sensor.h"
#include "avg.h"
#include "relay.h"
#if XCODE_BUILD == 1
#include <stdio.h>
#endif

#if XCODE_BUILD == 1
extern int XCODE_saw_motion;
extern int XCODE_saw_tube_down;
#endif

unsigned int bGotWdtInt = false;

// forward
static void hw_wait_for_motion_sensor_boot();
static void set_clk(void);
static void set_intlvl_debounce();
static void my_hw_init(void);
#if USE_MSP_PWM == 1
static void ConfigureTimerPwm(void);
#endif


//------------------------------------
// PUBLIC
//------------------------------------

void hw_go_to_sleep()
{
  P1OUT &= ~LED1;
  //unsigned long tnow = get_time();
  // can't go to LPM3 or PWM stops
  //_BIS_SR(LPM3_bits + GIE);                 // Enter LPM3 w/interrupt
  _BIS_SR(GIE);
  // wait for WDT to fire
#if XCODE_BUILD != 1
  bGotWdtInt = false;
  while (bGotWdtInt == false)
    ;
#endif
  P1OUT |= LED1;
}

// percent = on time 100% = on all the time
void hw_set_relay_pwm_percent(unsigned int percent)
{
#if USE_MSP_PWM == 1
  percent = (percent > 100) ? 100 : percent;
  unsigned long sum = TIMER_PWM_PERIOD - (((unsigned long)percent * (unsigned long)TIMER_PWM_PERIOD)/100L);
  TACCR1 = ( (sum) < (TIMER_PWM_PERIOD-1) ? (sum) : (TIMER_PWM_PERIOD-1) );
  TACCTL0 |= CCIE;
  TACCTL1 |= CCIE; 
#else
  if (percent == 0)
#if XCODE_BUILD == 1
	printf("OFF\n");
#else
    P1OUT &= ~RELAY;
#endif
  else 
#if XCODE_BUILD == 1
	  printf("ON\n");
#else
	P1OUT |= RELAY;
#endif
#endif
}

int hw_init( void )
{
  my_hw_init();
  set_clk();
#if USE_MSP_PWM == 1
  ConfigureTimerPwm();
#endif
  
  // WDT ints used as basic system timer
  WDTCTL = WDT_ADLY_250;    // first we set to 250ms here (read below about boot and IR sensor)
  IE1 |= WDTIE;                             // Enable WDT interrupt
  
  // when board is first powered, motion sensor output is high
  // for many seconds to calibrate, just wait for it to go low.
  //
  // IR sensor takes a while on power up before
  // signal goes low initially, wait for it
  // first, go to sleep for 250ms
  _BIS_SR(LPM3_bits + GIE);
  
  // This is to handle the case where board comes up but sensor
  // is delayed in raising signal for calibration and we miss it
  // here, we will catch in main.
  hw_wait_for_motion_sensor_boot();
  
  // now set WDT to correct timing
  WDTCTL = WDT_PARAM;

  return 0;
}

int hw_see_motion_now()
{
#if XCODE_BUILD == 1
  return XCODE_saw_motion;
#else
  return P1IN & MOTION_INPUT;
#endif
}

int hw_is_water_off()
{
#if XCODE_BUILD == 1
  return (XCODE_saw_tube_down);
#else
  // when touching, the voltage read will be pulled down to 0v
  return ( ! ( P1IN & OFF_SENSOR ) );
#endif
}


//------------------------------------
// PRIVATE
//------------------------------------

static void hw_wait_for_motion_sensor_boot()
{
  while (hw_see_motion_now())
    ;
}

static void set_clk(void)
{
  if (CALBC1_1MHZ ==0xff || CALDCO_1MHZ == 0xff)                                     
  {  
    while(1);                               // if calibration constants erased
    // do not load, trap cpu!!
  } 
  //1Mhz
  BCSCTL1 = CALBC1_1MHZ;                    // Set range
  DCOCTL = CALDCO_1MHZ;                     // Set DCO step + modulation */
  /* //8Mhz
  BCSCTL1 = CALBC1_8MHZ;                    // Set range
  DCOCTL = CALDCO_8MHZ;                     // Set DCO step + modulation */
  /* //12Mhz
  BCSCTL1 = CALBC1_12MHZ;                   // Set range
  DCOCTL = CALDCO_12MHZ;                    // Set DCO step + modulation*/
  /* //16Mhz
  BCSCTL1 = CALBC1_16MHZ;                   // Set range
  DCOCTL = CALDCO_16MHZ;                    // Set DCO step + modulation*/
}

static void set_intlvl_debounce()
{
  if (hw_see_motion_now())  // 0x20 is motion sensor input
    P1IES |= MOTION_INPUT;  // 0=low-2-high 1=high-2-low
  else
    P1IES &= ~MOTION_INPUT;
  if (P1IN & OFF_SENSOR)
    P1IES |= OFF_SENSOR;
  else
    P1IES &= ~OFF_SENSOR;
}

static void my_hw_init(void)
{
  P1SEL &= ~(LED0 | LED1 | RELAY | OFF_SENSOR);

  // OUTPUTS
  P1DIR |= LED0 | LED1 | RELAY;     // output and
  P1OUT &= ~(LED0 | LED1 | RELAY);  // turn off P1.0 and P1.6  
#if USE_MSP_PWM == 1
  P1SEL |= RELAY;
#endif

  // INPUTS
  // motion sensor and off sensor connected as input
  P1DIR &= ~(MOTION_INPUT | OFF_SENSOR);    // input
  // off-sensor pull-up
  P1REN |= OFF_SENSOR;
  P1OUT |= OFF_SENSOR;
  set_intlvl_debounce();
  P1IFG &= ~(MOTION_INPUT | OFF_SENSOR);
  P1IE |= MOTION_INPUT | OFF_SENSOR;
}

#if USE_MSP_PWM == 1
static void ConfigureTimerPwm(void)
{  
  TACCR0 = TIMER_PWM_PERIOD; 
  TACTL = TASSEL_2 | MC_1;                  // TACLK = SMCLK, Up mode.
  TACCTL0 = CCIE;
  TACCTL1 = CCIE + OUTMOD_3;                // TACCTL1 Capture Compare
  // start with pwm "off"
  TACCR1 = TIMER_PWM_PERIOD;
}
#endif

// motion sensor edge triggers port1
// Port 1 interrupt service routine
#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{
  set_intlvl_debounce();
    // TODO: want to see both sides of transition on off_sensor, so can tell when goes on *AND* off
  if (P1IFG & OFF_SENSOR)
  {
    relay_hw_water_pressure_changed();
    P1IFG &= ~OFF_SENSOR;
  }
  if (P1IFG & MOTION_INPUT)
  {
    avg_hw_saw_motion();
    P1IFG &= ~MOTION_INPUT;
  }
    // TODO: add edge detect for parrot "repeat"
}

#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void)
{
  bGotWdtInt = true;
  IFG1 &= ~WDTIFG;                 /* clear interrupt flag */
  __low_power_mode_off_on_exit();           // exit low-power mode
}

// Timer A1 interrupt service routine
#pragma vector=TIMER0_A1_VECTOR
__interrupt void ta1_isr(void)
{
  TACCTL1 &= ~CCIFG;
}

// Timer A0 interrupt service routine
#pragma vector=TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
  TACCTL0 &= ~CCIFG;              
}
