//
// use motion sensor to switch on water sprayer
//
// (c) esean_us@yahoo.com 6/14/12
//


#ifndef MYMAX
#define MYMAX(a,b)  (((a)>(b)) ? (a) : (b))
#endif

#ifndef MYMIN
#define MYMIN(a,b)  (((a)<(b)) ? (a) : (b))
#endif

// returns output of previous output_val using new_val as steering
unsigned int mymath_compute_new_max( unsigned int output_val, unsigned int new_val, unsigned int coef_A, unsigned int coef_B );
