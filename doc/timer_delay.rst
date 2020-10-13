Timer initialization library
=======================================

Library to init timer and delay functions that provides delays in micro and milli seconds.
Also inits function that counts lenght of pulse from hc-sr04 sensor

.. c:autodoc:: inc/timer_delay.h src/timer_delay.c
   :clang: -I/lib/clang/10.0.0/include, -I/inc, -std=gnu17, -DHAWKMOTH
