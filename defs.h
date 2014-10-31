//
// use motion sensor to switch on water sprayer
//
// (c) esean_us@yahoo.com 6/14/12
//

#if XCODE_BUILD == 1
#include "local_undefs.h"
#endif


// Configuration defines
//---------------

// use 20depth array for input to sensor
#define USE_ARRAY_AVG     1

// use IIR for input to sensor
#define USE_IIR_AVG       1

// Drives relay PWM down based on contiguousness of motion detected from IR
#define USE_IR_INHIBIT    0

// TODO: add integral & derivative input to sensor

// Enables MSP PWM of relay switch (note, this PWM is too fast for switch, thus is off)
#define USE_MSP_PWM       0


// Misc
//---------------

#ifndef true
#define true 1
#endif

#ifndef false
#define false 0
#endif

