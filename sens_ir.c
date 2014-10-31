//
// use motion sensor to switch on water sprayer
//
// (c) esean_us@yahoo.com 6/14/12
//

#include "defs.h"
#include "hw.h"
#include "sens_ir.h"
#include "filter.h"
#include "avg.h"
#include "mymath.h"
#include "timer.h"
#if XCODE_BUILD == 1
#include <stdio.h>
#endif

// largest motion measurement we have seen
static unsigned int max_sum = 0;
static unsigned int max_sum_longShortAvg = 0;
// 0-100% reading value should be reduced to drive down excessive on situation
static unsigned int persistant_on = 0;
// long/short-term avg memory from last computation, polled from watcher for relay PWM decay param
static unsigned int last_longShortAvg = 0;
// 20arrary last sum, ie "vote"
static unsigned int last_sum = 0;

// forward
static void sens_ir_inhibit();
static void sens_ir_vote();
static void sens_ir_vote_longShortAvgs();


//-------------------------------------
// PUBLIC
//-------------------------------------

// called every 250ms
void sens_ir_callback()
{
  sens_ir_inhibit();
  sens_ir_vote();
  sens_ir_vote_longShortAvgs();
#if XCODE_BUILD == 1
  printf(",%d,%d,%d,%d,%d",max_sum,max_sum_longShortAvg,persistant_on,last_longShortAvg,last_sum);
#endif
}

unsigned int sens_ir_get_last_sum()
{
  return last_sum;
}

unsigned int sens_ir_get_last_longShortAvg()
{
  return last_longShortAvg;
}

unsigned int sens_ir_get_last_persistant_on()
{
  return persistant_on;
}


//-------------------------------------
// PRIVATE
//-------------------------------------

static void sens_ir_inhibit()
{
  // whenever we see motion, increment 'persistant_on' otherwise let it filter back to 0
  if (hw_see_motion_now())
  {
    // inc. every sec
    if (((unsigned int)get_time() % WDT_CYCLES_PER_SEC) == 0)
      persistant_on += 1;
  }
  else if (persistant_on > 1)
  {
    // drive back to 0 slowly
    persistant_on = filter_one_unsigned( persistant_on, 0, 700 );
  }
  else
  {
    persistant_on = 0;
  }
  
  persistant_on = MYMIN(persistant_on,100);
}

static void sens_ir_vote()
{
  unsigned int ret;
  unsigned int sum;

  // 1. look at absolute value
  //-------------------

  sum = avg_get_short_avg();

  // post new val to max_sum
  // TODO: this coef value can be API'd to allow larger=slower changes
  //      ie, faster 
  max_sum = mymath_compute_new_max( max_sum, sum, 650, 1000 );

  ret = (max_sum == 0) ? 0 : (unsigned int)(((unsigned long)sum*100L)/max_sum);

  last_sum = MYMIN(ret,100);
}

static void sens_ir_vote_longShortAvgs()
{
  unsigned int ret = 0;
  unsigned int delta = 0;

  // 2. look at long/short avgs
  //-------------------
  if ( avg_get_short_avg() > avg_get_long_avg() )
  {
    delta = avg_get_short_avg() - avg_get_long_avg();
    max_sum_longShortAvg = mymath_compute_new_max( max_sum_longShortAvg, delta, 650, 1000 );
  }

  ret = ( max_sum_longShortAvg == 0 ) ? 0 : (unsigned int)(((unsigned long)delta*100L)/max_sum_longShortAvg);

  last_longShortAvg = MYMIN(ret,100);
}

