#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <stdint.h>
#include "abs_systick.h"
#include "clkset.h"

/**
 * Simple illustrative softdelay (inefficient though)
 * @N: abstract number of cycles. Not CPU cycles at all
 *
 * Note:
 * This way of delaying is really really inefficient:
 * What it does is makes CPU spend time in empty loop consuming energy
 * and heating up the atmosphere. Whenever delay is required, there are better ways
 * of achieving it. CPU can sleep saving some energy or do some useful calculations while waiting.
 * However, softdelays are sometimes used for achieving ultra-small delays (i.e. in bit-bang). But
 * in that case, softdelays are fine-tuned against CPU frequency and other stuff for fine-grained
 * and more deterministic delay times.
 */
/*void softdelay(volatile uint32_t N)
{
    while (N--);
    // or: while (N--) __asm__("nop");
}*/

int main(void)
{
    clock_hse_init(4, 168, 2, 7, 0);
    rcc_periph_clock_enable(RCC_GPIOD);
    abs_sys_tick_init(168000000ul/100000ul, 2);
    gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12 | GPIO13 | GPIO14 | GPIO15);
    gpio_set(GPIOD, GPIO12 | GPIO13 | GPIO14 | GPIO15);
    while (1) {
        gpio_clear(GPIOD, GPIO12 | GPIO13 | GPIO14 | GPIO15);
        abs_delay_ms(1000);
        gpio_set(GPIOD, GPIO12 | GPIO13 | GPIO14 | GPIO15);
        abs_delay_ms(1000);
        // or:
        //     gpio_toggle(GPIOD, GPIO12);
        //     softdelay(800000);
    }
}
