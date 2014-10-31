
// use motion sensor to switch on water sprayer
//
// (c) esean_us@yahoo.com 6/14/12
//

#include "defs.h"
#include "hw.h"
#include "avg.h"
#include "filter.h"

#define LONG_TERM_COEF    910
#define SHORT_TERM_COEF   500

static unsigned int wdt_cnt = 0;
static unsigned int motNow = 0;

// IIR filters
static unsigned int avg_long_term = 0;
static unsigned int avg_short_term = 0;

// forward


//------------------------------------
// PUBLIC
//------------------------------------

void avg_hw_saw_motion()
{
  // factor in motion coming on
  //      or going off as one count
  //      but really we should just count coming on
  //      ie, the motion just started
  // These are weighted heavier, than just seeing motion
  //  each WDT interrupt
  if (hw_see_motion_now())
    motNow += 2;
}

// called every WDT timeout, 250ms
void avg_callback()
{
  // if motion still exists, increment
  if (hw_see_motion_now())
    motNow += 1;
  
  // short/long terms updated every tick

  // array is stuffed here - this is eye to world, ie, amount of motion-seen quantity
  //
  // store a new avg set every second
  //
  wdt_cnt++;
  if (wdt_cnt > (WDT_CYCLES_PER_SEC-1))
  {
    wdt_cnt = 0;
    
    avg_long_term = filter_one_unsigned(avg_long_term,motNow*100,LONG_TERM_COEF);
    avg_short_term = filter_one_unsigned(avg_short_term,motNow*100,SHORT_TERM_COEF);

	// TODO: take integral & derivative averages from previous sample, add API to query

    motNow = 0;
  }
}

unsigned int avg_get_long_avg()
{
  return avg_long_term;
}

unsigned int avg_get_short_avg()
{
  return avg_short_term;
}

//------------------------------------
// PRIVATE
//------------------------------------

