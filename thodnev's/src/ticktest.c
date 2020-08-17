#include "tick.h"
#include "pin.h"
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <stdint.h>
#include <stdbool.h>


int main(void)
{
    rcc_periph_clock_enable(RCC_GPIOD);
    //gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12);
	glsk_pins_init(true);
	sk_pin_set(sk_io_led_orange, true);

	sk_tick_init(16000000ul / 10000ul, 2);
	cm_enable_interrupts();

    while (1) {
		sk_pin_toggle(sk_io_led_orange);
        //for (int i = 0; i < 15; i++) delay_us(65535);
		sk_tick_delay_ms(500);
    }
}
