#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include "pin.h"
#include "abs_systick.h"



int main(void)
{
	rcc_periph_clock_enable(GPIOA);
	rcc_periph_clock_enable(GPIOD);

	glsk_pins_init(true);

	sk_pin_set(sk_io_led_orrange, 1);






	while(1){
		//everything is in interrupt
	}
}
