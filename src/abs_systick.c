#include "abs_systick.h"
#include "intrinsics.h"
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/systick.h>
#include <libopencm3/stm32/rcc.h>

volatile uint32_t __nticks = 0;


bool abs_sys_tick_init(uint32_t period, uint8_t irq_priority)
{
	if(0 == period)
		return false;

	//in case user wants to reconfigure systick
	systick_counter_disable();

	if (!(period % 8)) {
		period /= 8;
		systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
	} else {
		systick_set_clocksource(STK_CSR_CLKSOURCE_AHB);
	}

	systick_interrupt_enable();	 

	// set STK_RVR
	systick_set_reload(period);

	//STK_CVR holds the current value of the counter
	STK_CVR = period;

	nvic_set_priority(NVIC_SYSTICK_IRQ, irq_priority);
	nvic_enable_irq(NVIC_SYSTICK_IRQ);

	abs_set_current_tick(0);

	systick_counter_enable();

	return true;
}

void sys_tick_handler(void)
{
	__nticks++;
}

uint32_t abs_get_tick_rate(void)
{
	uint32_t tick_rate = rcc_ahb_frequency / systick_get_reload();

	// AHB/8 = 0
	// system clock speed (AHB) = 1
	if (!(STK_CSR & STK_CSR_CLKSOURCE)) {

		tick_rate /=8;
	}

	return tick_rate;
}

// maximum ms value depends on period parameter set by user 
void abs_delay_ms(uint32_t ms)
{
	uint32_t current = abs_get_current_tick();
	uint32_t tick_rate = abs_get_tick_rate();

	uint32_t delta = (tick_rate/1000)*ms;
	uint32_t next = abs_get_current_tick() + delta;

	// overflow precautions
	if (next < current) {	
		while (abs_get_current_tick() > next)
			__WFI();
	} else {			
		while (abs_get_current_tick() <= next)
			__WFI();
	}
}

