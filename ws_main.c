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
#include "filter.h"
#include "sens_ir.h"
#include "relay.h"
#include "watcher.h"
#include "mymath.h"

int boot()
{
  if (hw_init()) return 1;
  if (relay_init()) return 1;
  return 0;
}

#if XCODE_BUILD == 1
int init_main(void)
#else
int main( void )
#endif
{
  
  // bring up system
  if (boot())
  {
    // init fail, fatal
    P1OUT |= LED0;
    P1OUT |= LED1;
#if XCODE_BUILD == 1
	  return 1;
#else
    while (1)
      ;
#endif
  }
  
#if XCODE_BUILD == 1
	return 0;
}

void call_main(void)
{
#else
	while(1)
#endif
	
  
  {    
    // sleep 250ms, awoken by WDT int
    hw_go_to_sleep();
    
    // update system time
    // always goes first!
    timer_callback();

    // store samples
    avg_callback();

    // take measurements and take actions
    sensor_callback();
    
	// act on above actions
    watcher_callback();

    // handle relay PWM
    // always goes last!
    relay_callback();
  }
  
}
