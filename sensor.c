//
// use motion sensor to switch on water sprayer
//
// (c) esean_us@yahoo.com 6/14/12
//

#include "defs.h"
#include "hw.h"
#include "sensor.h"
#include "filter.h"
#include "sens_ir.h"

// only factor in long/short-term avg if above this threshold
#define THRESHOLD_ir_longShortAvg   33

#if USE_ARRAY_AVG == 1
// values from IR motion sensor
static unsigned int ir_percent = 0;
#endif
#if USE_IR_INHIBIT == 1
static unsigned int ir_inhibit = 0;
#endif
#if USE_IIR_AVG == 1
// long/short-term avg from IR motion sensor
static unsigned int ir_longShortAvg = 0;
#endif
// computed value, watcher.c calls getter for value to send to relay pwm
static unsigned int final_percent = 0;

// forward


//-------------------------------------
// PUBLIC
//-------------------------------------

// called every 250ms
void sensor_callback()
{
  unsigned int cnt = 0;
  
  //================
  // SENSOR SAMLPING
  //================
  sens_ir_callback();
  
  //================
  // SENSOR CALC
  //================

  // IR MOTION 
  //-------------------
#if USE_ARRAY_AVG == 1
  {
//    unsigned int ir_percent_prev = ir_percent;
    ir_percent = sens_ir_get_last_sum();
#if 0
    // TODO: need getters for these int&deriv values
    //
    // Is there some more generalized way to do this, can I just pass a value
    //	into some method and then query it for the avg result ??
    //
    // TODO: need these fcns ! they are also run thru filter_one_unsigned()
    ir_percent_integral = filter_integral(ir_percent_prev,ir_percent);
    ir_percent_derivative= filter_derivative(ir_percent_prev,ir_percent);
    // TODO: need these fcns ! they are also run thru filter_one_unsigned()
#endif  // 0
    cnt++;
  }
#endif
  
#if USE_IR_INHIBIT == 1
  {
//    unsigned int ir_inhibit_prev = ir_inhibit;
    // See? we're doing the same thing here...
    ir_inhibit = sens_ir_get_last_persistant_on();
#if 0
    ir_inhibit_integral = filter_integral(ir_inhibit_prev,ir_inhibit);
    ir_inhibit_derivative= filter_derivative(ir_inhibit_prev,ir_inhibit);
    // TODO: need these fcns ! they are also run thru filter_one_unsigned()
    // TODO: need getters for these int&deriv values
#endif // 0
    if (ir_inhibit > 75)
      ir_inhibit = 75;
    // reduce ir_percent by [ir_inhibit] percent
    // TODO: need to have our own scale of how much we think we should be inhibiting (such as at startup, etc)
    ir_percent = filter_take_percentage_unsigned(ir_percent,100-ir_inhibit);
  }
#endif
  
#if USE_IIR_AVG == 1
  // long/short-term avg
  ir_longShortAvg = sens_ir_get_last_longShortAvg();
  // only factor in this input if above some threshold
  if (ir_longShortAvg > THRESHOLD_ir_longShortAvg)
    cnt++;
#endif
  
  // TODO: else: consider ultrasonic input ? 
  //-------------------
  // TODO: microphone input ?
  //-------------------
  // TODO: photosensors ?
  //-------------------
  
  // TODO: +ultrasonic + photosensor + ...) / count for avg
#if USE_IIR_AVG == 1
  // if there was just a sudden increase in movement, drive pwm faster
  if (ir_longShortAvg > (THRESHOLD_ir_longShortAvg * 2))
    final_percent = (ir_percent + ir_longShortAvg*2) / (cnt+1);
  else
    final_percent = (ir_percent*4 + ir_longShortAvg) / (cnt+4);
#else
  final_percent = (ir_percent*4 + ir_longShortAvg) / (cnt+4);
#endif
}

unsigned int sensor_get_final_percent()
{
  return final_percent;
}


//-------------------------------------
// PRIVATE
//-------------------------------------

