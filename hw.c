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
  bGotWdtInt = false;
  while (bGotWdtInt == false)
    ;
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
    P1OUT &= ~RELAY;
  else 
    P1OUT |= RELAY;
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
  return P1IN & MOTION_INPUT;
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
}

static void my_hw_init(void)
{
  P1DIR |= LED0 | LED1 | RELAY;     // output and
  P1OUT &= ~(LED0 | LED1 | RELAY);  // turn off P1.0 and P1.6  
#if USE_MSP_PWM == 1
  P1SEL |= RELAY;
#endif
  // motion sensor is connected here as I/O
  P1DIR &= ~MOTION_INPUT;    // input
  set_intlvl_debounce();
  P1IE |= MOTION_INPUT;
  P1IFG &= ~MOTION_INPUT;
}

#if USE_MSP_PWM == 1
static void ConfigureTimerPwm(void)
{  
  TACCR0 = TIMER_PWM_PERIOD;                              //   
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
  P1IFG &= ~MOTION_INPUT;                           // IFG cleared
  avg_hw_saw_motion();
}

#pragma vector=WDT_VECTOR
__interrupt void WDT_ISR(void)
{
  IFG1 &= ~WDTIFG;                 /* clear interrupt flag */
  bGotWdtInt = true;
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
