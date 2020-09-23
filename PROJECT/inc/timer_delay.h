#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <stdint.h>

/**
 * Inits TIM2 to measure lenght of impulse
 * @prescaler: we set this value to make timers period last 1 microsecond.
 *
 * Note:
 * Used to count length of hcsr04 signal.
 */
 void timer_init(uint16_t prescaler);

/**
 * Inits TIM7 to use delay functions
 * @prescaler: we set this value to make timers period last 1 microsecond.
 *
 * Note:
 * On this timer based delay_us and delay_ms functions.
 */
 void timer_delay_init(uint16_t prescaler);

/**
 * Provides micrsecond delay
 * @us: number of microseconds delay
 *
 * Note:
 * Uses wfi during delay.
 */
 void delay_us(uint32_t us);

/**
 * Provides millisecond to delay
 * @ms: number of milliseconds to delay
 *
 * Note:
 * per one millisecond use loop of 1000 delay_us delays.
 */
 void delay_ms(uint32_t ms);

 void softdelay(uint32_t N);
