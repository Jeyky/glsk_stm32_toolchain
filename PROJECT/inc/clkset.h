/*#include <stdint.h>
#include <stddef.h>
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/pwr.h>
*/


/**
 * Setup clock on 168MHz 
 * @lcd:  structure of certain display pins.
 * @level: desired level of display background brightness. 
 *
 * Note:
 * clocks from HSE, APB1=42MHz, APB2=84MHz. HSI diseables.
 */
void clock_hse_168MHZ_init(void);