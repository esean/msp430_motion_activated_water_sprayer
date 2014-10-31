//
// use motion sensor to switch on water sprayer
//
// (c) esean_us@yahoo.com 6/14/12
//

#include "defs.h"
#include "timer.h"

static unsigned long timer = 0L;

// forward


//------------------------------------
// PUBLIC
//------------------------------------

unsigned long get_time()
{
  // TODO: factor in timer_rollover
  return timer;
}

// called every 250ms
void timer_callback()
{
  // increment our unit of time
  timer++;
}


//------------------------------------
// PRIVATE
//------------------------------------

