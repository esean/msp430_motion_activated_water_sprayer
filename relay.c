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

typedef enum {
  NORMAL,
  WAIT_FOR_PULSE_OFF,
} RELAY_STATE;

static RELAY_STATE myState = NORMAL;

// boolean: true if we are actively in a 'water switch on' cycle (which may contain multiple on/off events)
static unsigned int isRelayPwmActive = false;
// boolean: >0 means pulse the output, ==0 means constant on
static unsigned int doPulsedRelaysPulse = 0;

// time at which relay switch will turn off
static unsigned long relay_off_time = 0L;
static unsigned long relay_on_time = 0L;
// stores last time relay was turned off
static unsigned long last_relay_turn_off_time = 0L;  

// forward
static void relay_turn_relay_off();


//-------------------------------------
// PUBLIC
//-------------------------------------

int relay_init()
{
  // make sure pwm to relay is off
  relay_pulse_relay_pwm(0,0);
  
  return 0;
}

// set relay PWM to percent for on_time_ticks
void relay_pulse_relay_pwm(unsigned int percent, unsigned int on_time_ticks)
{
  // don't allow any change if already spraying water
  if (!hw_is_water_off())
    return;

  // don't allow any change if we are in a cycle already
  if (isRelayPwmActive)
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
    relay_on_time = get_time();
    relay_off_time = relay_on_time + (unsigned long)on_time_ticks;
  }
  else
    relay_turn_relay_off(); 
}

// called every 250ms
void relay_callback()
{
  switch (myState)
  {
    case NORMAL:
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
#if 0
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
#else
          // pulse on & off, wait for tube to fall and turn on again
          if ((get_time() - relay_on_time) % doPulsedRelaysPulse) 
          {
            // invert state 
            ;
          }
#endif
        }
      }
      break;
    case WAIT_FOR_PULSE_OFF:
      break;
  }
}

void relay_hw_water_pressure_changed()
{
  // TODO: we need to act on this change *NOW*
    // if it just went on, record how long it took to go on
    // if it just went off, record how long from when relay was turned off
  //is_water_switch_off = hw_is_water_off();
  ;
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

