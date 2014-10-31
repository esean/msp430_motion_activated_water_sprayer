//
// use motion sensor to switch on water sprayer
//
// (c) esean_us@yahoo.com 6/14/12
//

#include "defs.h"
#include "hw.h"
#include "watcher.h"
#include "sensor.h"
#include "relay.h"
#include "timer.h"
#include "sens_ir.h"
#include "mymath.h"

#define RELAY_NO_MOTION_TIME_INTERVAL_SEC     30  // after 15 seconds relay off, do big spray
#define RELAY_NO_MOTION_TIME_INTERVAL_TICKS   SEC_2_TICKS(RELAY_NO_MOTION_TIME_INTERVAL_SEC)
#define RELAY_NO_MOTION_DECAY_SEC             180 // big sprays every 15sec during this time, decreasing pulse widths, off after this time
#define RELAY_NO_MOTION_DECAY_DIV             (100 / (RELAY_NO_MOTION_DECAY_SEC/RELAY_NO_MOTION_TIME_INTERVAL_SEC))

#define RELAY_MAX_SPRAY_TIME_SEC              3
#define RELAY_MAX_SPRAY_TIME_DIV              (100 / RELAY_MAX_SPRAY_TIME_SEC)

typedef enum {
  NORMAL,
  NO_MOTION,
  NO_MOTION_WAIT_FOR_OUR_SPRAY,
} WATCHER_MOOD;

// values from sensor
static unsigned int big_sprays = 0;
// mood dictates how it will respond to movement
static WATCHER_MOOD myMood = NORMAL;

// forward


//-------------------------------------
// PUBLIC
//-------------------------------------

// called every 250ms
void watcher_callback()
{
  unsigned int sensor_val;
  unsigned int watcher_percent = 0;
  unsigned int final_percent = 0;

  // get what sensor saw
  sensor_val = sensor_get_final_percent();

  switch (myMood)
  {
    case NORMAL:
      if ( ( sensor_val == 0 ) && ( relay_is_relay_on_now() == 0) )
      {
        if ((unsigned int)(get_time() - relay_last_turn_off_time()) > RELAY_NO_MOTION_TIME_INTERVAL_TICKS)
        {
          myMood = NO_MOTION;
        }
      }
      else
      {
        // if there is motion, but longterm avg has dropped (no change), we should add some quick bursts
        // TODO: need derivative here to see direction of long/short-term avg
        if ((sens_ir_get_last_sum() > 50) && (sens_ir_get_last_longShortAvg() < 50))
        {
          watcher_percent += 50;
        }
      }
      break;

    case NO_MOTION:
      // if there hasn't been a water pulse in a while, shoot for a second
      if ( ( sensor_val == 0 ) && ( relay_is_relay_on_now() == 0) )
      {
        if ((unsigned int)(get_time() - relay_last_turn_off_time()) > RELAY_NO_MOTION_TIME_INTERVAL_TICKS)
        {
          // big spray but decrease their intensity over time if they continue
          big_sprays = MYMIN(big_sprays+1,RELAY_NO_MOTION_DECAY_DIV);
          watcher_percent = ((RELAY_NO_MOTION_DECAY_DIV * big_sprays) > 100) ? \
            0 : 100 - (RELAY_NO_MOTION_DECAY_DIV * big_sprays);
          sensor_val = watcher_percent;   // we lie so pulse is bigger, avg then equals what we want
          // spray has been started, wait for it to end and start another if still no motion
          myMood = NO_MOTION_WAIT_FOR_OUR_SPRAY;
          if (watcher_percent > 50)
            relay_set_pulsed_out_to(1);
          else
            // once PWM cycle gets too short, just make always on spray
            relay_set_pulsed_out_to(0);
        }
      }
      else
      {
        big_sprays = 0;
        myMood = NORMAL;
        relay_set_pulsed_out_to(0);
      }
      break;

    case NO_MOTION_WAIT_FOR_OUR_SPRAY:
      // once our spray is done, return to NO_MOTION
      if ( relay_is_relay_on_now() == 0 )
        myMood = NO_MOTION;
      break;
  }

  final_percent = (watcher_percent == 0) ? sensor_val : ((sensor_val + watcher_percent) / 2);

  // set relay PWM
  relay_pulse_relay_pwm(final_percent, SEC_2_TICKS(final_percent / RELAY_MAX_SPRAY_TIME_DIV));
}


//-------------------------------------
// PRIVATE
//-------------------------------------

