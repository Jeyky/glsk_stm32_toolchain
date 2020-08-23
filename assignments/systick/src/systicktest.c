#include "pin.h"
#include <libopencm3/cm3/sync.h>
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include "stdint.h"
#include "stdio.h"
#include "stdbool.h"

volatile uint32_t __nticks = 0;

const uint32_t TICK_RATE_HZ = 10000ul;

inline __attribute__ ((always_inline)) void __WFI(void)
{
	__asm__ volatile ("wfi");
}

inline uint32_t get_current_tick(void)
{
	return __nticks;
}

void sys_tick_init(void)
{
	//sk_pin_set(sk_io_led_green, true);
	systick_counter_disable();

	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);

	systick_interrupt_enable();	 
	//10 kHz  = 16 000 000 / 1600 = (100 us)
	uint32_t period = 16000000ul/10000ul;
	systick_set_reload(period);

	
	STK_CVR = period;

	nvic_set_priority(NVIC_SYSTICK_IRQ, 2);
	nvic_enable_irq(NVIC_SYSTICK_IRQ);

	__nticks = 0;

	systick_counter_enable();
}

void sys_tick_handler(void)
{
	__nticks++;
}

static void delay_ms_systick(uint32_t ms)
{
	uint32_t delta = (TICK_RATE_HZ/1000)*ms;
	uint32_t next = get_current_tick() + delta;	
	while(get_current_tick() <= next){
		__WFI();
	}
}

int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOD);

	glsk_pins_init(true);
	sk_pin_set(sk_io_led_red, true);

	sys_tick_init();
	cm_enable_interrupts();

	while(1){
		sk_pin_toggle(sk_io_led_red);
		delay_ms_systick(500);
	}

}