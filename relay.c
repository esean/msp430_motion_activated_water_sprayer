//
// use motion sensor to switch on water sprayer
//
// (c) esean_us@yahoo.com 6/14/12
//

#include "defs.h"
#include "relay.h"
#include "hw.h"
#include "filter.h"
#include "timer.h"

// If relay is left on more than this, it needs more time to turn off
#define ON_TIME_REQUIRES_MORE_OFF     2
#define ON_TIME_FULLY_OFF             6
#define ON_TIME_DEF_INTERSPACE_TICKS  (WDT_CYCLES_PER_SEC/2)  // 1/2 second

// boolean: true if we are actively driving a pwm to relay
static unsigned int isRelayPwmActive = false;
// boolean: >0 means pulse the output, ==0 means constant on
static unsigned int doPulsedRelaysPulse = 0;

// time at which relay switch will turn off
static unsigned long relay_off_time = 0L;
// stores time at which next relay PWM pulse may begin
static unsigned long timer_next_relay_pulse = 0L;
// stores last time relay was turned off
static unsigned long last_relay_turn_off_time = 0L;  

// forward
static void relay_set_relay_percent(unsigned int percent);
static void relay_turn_relay_off();


//-------------------------------------
// PUBLIC
//-------------------------------------

int relay_init()
{
  // make sure pwm to relay is off
  relay_set_relay_percent(0);
  
  return 0;
}

// set relay PWM to percent for on_time_ticks
void relay_pulse_relay_pwm(unsigned int percent, unsigned int on_time_ticks)
{
  // return if the inter-space pulse time has not been met
  // NOTE: compare as uint to avoid long compare
  if (timer_next_relay_pulse > get_time())
    return;

  if (isRelayPwmActive)
    return;

  relay_off_time = get_time() + (unsigned long)on_time_ticks;

  relay_set_relay_percent(percent);

  if (percent > 0)
  {
    timer_next_relay_pulse = get_time() + (unsigned long)on_time_ticks + \
      ((on_time_ticks > SEC_2_TICKS(ON_TIME_REQUIRES_MORE_OFF)) ? \
        (unsigned long)SEC_2_TICKS(ON_TIME_FULLY_OFF) : \
        (unsigned long)ON_TIME_DEF_INTERSPACE_TICKS);
  }
}

// called every 250ms
void relay_callback()
{
  // if currently doing a PWM ramp-down, slowly drive back off relay
  if (isRelayPwmActive)
  {
    // NOTE: compare as uint to avoid long compare
    if (relay_off_time <= get_time())
    {
      relay_turn_relay_off();
    }
    else if (doPulsedRelaysPulse > 0)
    {
      // Pulse water relay
      if (((unsigned int)get_time() % (WDT_CYCLES_PER_SEC * 2)) == 0)
      {
        P1OUT &= ~LED0;
        hw_set_relay_pwm_percent(0);
      }
      else if (((unsigned int)get_time() % (WDT_CYCLES_PER_SEC+1)) == 0)
      {
        P1OUT |= LED0;
#if USE_MSP_PWM == 1
        #error "TODO"
#else
        hw_set_relay_pwm_percent(100);
#endif
      }
    }
  }
}

unsigned long relay_last_turn_off_time()
{
  return last_relay_turn_off_time;
}

unsigned int relay_is_relay_on_now()
{
  return isRelayPwmActive;
}

void relay_set_pulsed_out_to(unsigned int new)
{
  if ( ! isRelayPwmActive)
  {
    doPulsedRelaysPulse = new;
  }
}

//-------------------------------------
// PRIVATE
//-------------------------------------

static void relay_set_relay_percent(unsigned int percent)
{
  // return if the inter-space pulse time has not been met
  // NOTE: compare as uint to avoid long compare
  if (timer_next_relay_pulse > get_time())
    return;

  // Red LED indicates relay is activated to some extent
  if (percent > 0)
  {
    P1OUT |= LED0;
    isRelayPwmActive = true;
#if USE_MSP_PWM == 1
    hw_set_relay_pwm_percent((percent>100) ? 100 : percent);
#else
    hw_set_relay_pwm_percent(100);
#endif
  }
  else
    relay_turn_relay_off(); 
}

static void relay_turn_relay_off()
{
  if (isRelayPwmActive)
  {
    P1OUT &= ~LED0;
    hw_set_relay_pwm_percent(0);
    last_relay_turn_off_time = get_time();
  }
  isRelayPwmActive = false;
}

