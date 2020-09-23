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

extern void initialise_monitor_handles(void);

inline __attribute__ ((always_inline)) void __WFI(void)
{
	__asm__ volatile ("wfi");
}

inline uint32_t get_current_tick(void)
{
	return __nticks;
}

void sys_tick_init(uint32_t period, const uint8_t irq_priority)
{
	//sk_pin_set(sk_io_led_green, true);

	//disable to have an ability to reconfigure systick
	systick_counter_disable();

	/*if(!(period % 8)) {
		period /=8;	
		systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
		sk_pin_set(sk_io_led_blue, true);
	} else {
		systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	}
	*/

	systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	systick_interrupt_enable();	 

	systick_set_reload(period);

	
	STK_CVR = period;

	nvic_set_priority(NVIC_SYSTICK_IRQ, irq_priority);
	nvic_enable_irq(NVIC_SYSTICK_IRQ);

	__nticks = 0;

	systick_counter_enable();
}

void sys_tick_handler(void)
{
	__nticks++;
}

uint32_t get_tick_rate(void)
{
	uint32_t tick_rate = rcc_ahb_frequency / systick_get_reload();
	// CLKSOURCE:
	// AHB/8 = 0
	// processor clock (AHB) = 1	
	//printf("rcc_ahb_frequency = %d\n", rcc_ahb_frequency);
	//printf("systick_get_reload = %d\n", systick_get_reload());
	if(!(STK_CSR & STK_CSR_CLKSOURCE)) {
		tick_rate = (rcc_ahb_frequency / 8) / systick_get_reload();
	}
}

static void delay_ms_systick(uint32_t ms)
{	
	uint32_t current = get_current_tick();
	uint32_t tick_rate = get_tick_rate();
	printf("tick_rate = %d\n", tick_rate);
	uint32_t delta = (tick_rate / 1000)*ms;
	
	uint32_t next = current + delta;
	
	if (next < current) {	// 32 bit overflow happened
		while (get_current_tick() > next)
			__WFI();
	} else {			// no overflow
		while (get_current_tick() <= next)
			__WFI();
	}
	sk_pin_set(sk_io_led_orange, true);
	
}

int main(void)
{
	initialise_monitor_handles();
	rcc_periph_clock_enable(RCC_GPIOD);

	glsk_pins_init(true);
	sk_pin_set(sk_io_led_red, true);

	sys_tick_init(16000000ul/10000ul, 2);
	cm_enable_interrupts();
	
	while(1){
		sk_pin_toggle(sk_io_led_green);
		delay_ms_systick(1000);
	}

}