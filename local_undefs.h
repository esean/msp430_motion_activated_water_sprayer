#if XCODE_BUILD == 1

int dummy;

#ifdef P1OUT
#undef P1OUT
#endif
#define P1OUT dummy

#define P1IN    dummy
#define P1IES   dummy
#define P1SEL   dummy
#define P1DIR   dummy
#define P1REN   dummy
#define P1IFG   dummy
#define P1IE    dummy
#define IFG1    dummy
#define WDTIFG  dummy
#define TACCTL0 dummy
#define TACCTL1 dummy
#define CCIFG   dummy

#define BIT0    0 
#define BIT1    0 
#define BIT2    0 
#define BIT3    0 
#define BIT4    0 
#define BIT5    0 
#define BIT6    0 
#define BIT7    0 

#define GIE     0
#define IE1     dummy

#define WDTCTL  dummy
#define WDTIE   0
#define WDT_ADLY_250    0
#define WDT_ADLY_1_9    0

#define CALBC1_1MHZ     dummy
#define CALDCO_1MHZ     dummy

#define BCSCTL1         dummy
#define DCOCTL          dummy

#define LPM3_bits   0

#define __interrupt 
#define _BIS_SR(x)
#define __low_power_mode_off_on_exit()

#endif
