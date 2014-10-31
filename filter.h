//
// use motion sensor to switch on water sprayer
//
// (c) esean_us@yahoo.com 6/14/12
//


#define FILTER_MAX_COEF_VAL   1024

// coef is 10bit for max 1024, returns filtered prev with new using coef
unsigned int filter_one_unsigned(unsigned int prev, unsigned int new, unsigned int coef);

// returns [percent] % of [val]
unsigned int filter_take_percentage_unsigned(unsigned int val, unsigned int percent);

