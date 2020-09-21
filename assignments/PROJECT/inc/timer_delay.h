#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <stdint.h>

 void timer_init(uint16_t prescaler);

 void timer_delay_init(uint16_t prescaler);

 void delay_us(uint32_t us);

 void delay_ms(uint32_t ms);

 void check(uint32_t us);
 
 void softdelay(uint32_t N);
