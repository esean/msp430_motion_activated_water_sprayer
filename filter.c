//
// use motion sensor to switch on water sprayer
//
// (c) esean_us@yahoo.com 6/14/12
//

#include "defs.h"
#include "filter.h"

#define RADIX_BITS  10
#define RADIX_MAX   (unsigned long)FILTER_MAX_COEF_VAL            // 2^RADIX_BITS
#define RADIX_RND   (1<<(RADIX_BITS)-1)

// forward


//------------------------------------
// PUBLIC
//------------------------------------

unsigned int filter_one_unsigned(unsigned int prev, unsigned int new, unsigned int coef)
{
  return (unsigned int)( (((unsigned long)prev * (unsigned long)coef + RADIX_RND) + \
    ((unsigned long)new * (RADIX_MAX-(unsigned long)coef))) >> RADIX_BITS );
}

unsigned int filter_take_percentage_unsigned(unsigned int val, unsigned int percent)
{
  return (unsigned int)(((unsigned long)val*(unsigned long)percent)/100L);
}


//------------------------------------
// PRIVATE
//------------------------------------

