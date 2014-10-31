Motion-activated sprinkler childrens toy.
--------------------------------------------
 (c) esean_us@yahoo.com 6/14/12


Ideally this system will only use microphone and
photodiodes to minimize product cost.


HW:
==========
MSP430g2231 (LaunchPad)
Parallax PID IR motion sensor
#Matronix(?) ultrasonic sensor
#photodiodes
#electret microphone


TODO:
==========
Each module has methods that can be used to get info from those files, avgs, integrals direct all access thru enum list to sensor.c, each 
   sensor registers its fcn ptrs at boot, list of enum array pointing to a set of fcn pointers for getting info.
   Then also, each module has knobs that can be turned so as upper layer may twiddle the settings while searching
   for some reward, both loud sounds in vocal range of children and trying to minimize ultrasonic range finder distance ('keep them
   close and entertained').



06-17-2012sbh

