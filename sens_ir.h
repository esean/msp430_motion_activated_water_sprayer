//
// use motion sensor to switch on water sprayer
//
// (c) esean_us@yahoo.com 6/14/12
//


// getter for last sum, called by watcher for tweak relay interpulse timing
// returns percent it thinks sprayer should be turned on, 0=off, 100%=on continuously
unsigned int sens_ir_get_last_sum();

// getter for last long/short-term avg, called by watcher for tweak relay pwm decay
// returns percent based on long vs. short term avgs
unsigned int sens_ir_get_last_longShortAvg();

// getter for last persistant_on
// returns percent that should be used to inhibit above vote percent
unsigned int sens_ir_get_last_persistant_on();

// CB
void sens_ir_callback();
