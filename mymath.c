//
// use motion sensor to switch on water sprayer
//
// (c) esean_us@yahoo.com 6/14/12
//

#include "defs.h"
#include "mymath.h"
#include "filter.h"

// forward


//------------------------------------
// PUBLIC
//------------------------------------

unsigned int mymath_compute_new_max( unsigned int output_val, unsigned int new_val, unsigned int coef_A, unsigned int coef_B )
{
  if (output_val == 0)
  {
    // output is undefined, set to new value
    return new_val;
  }
  else if (new_val > output_val)
  {
    // drive output up to new value
    return filter_one_unsigned( output_val, new_val, coef_A );
  }
  else
  {
    // always try to drive output_val back down
    return filter_one_unsigned( output_val, new_val, coef_B );
  }
}


//------------------------------------
// PRIVATE
//------------------------------------

