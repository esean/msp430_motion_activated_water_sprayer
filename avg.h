//
// use motion sensor to switch on water sprayer
//
// (c) esean_us@yahoo.com 6/14/12
//


// called from HW when edge-transition - it saw motion
void avg_hw_saw_motion();

// returns the long/short-term avgs, re-calc'd every sec
unsigned int avg_get_long_avg();
unsigned int avg_get_short_avg();

// CB
void avg_callback();

