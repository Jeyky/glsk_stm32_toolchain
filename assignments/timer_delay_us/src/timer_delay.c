#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include "libopencm3/stm32/gpio.h"

 void timer_init(uint16_t prescaler)
 {
 	rcc_periph_clock_enable(RCC_TIM2);

 	timer_set_prescaler(TIM2, prescaler);

 	timer_disable_preload(TIM2);

 	timer_one_shot_mode(TIM2);

 	timer_update_on_overflow(TIM2);

 	timer_enable_irq(TIM2, TIM_DIER_UIE);
 	timer_clear_flag(TIM2, TIM_SR_UIF);

 	nvic_set_priority(NVIC_TIM2_IRQ, 2);
	nvic_enable_irq(NVIC_TIM2_IRQ);


 }

void delay_us(uint32_t us)
{
	if(!us)
		return;
	timer_set_period(TIM2, us);

	timer_generate_event(TIM2, TIM_EGR_UG);

	__dmb();	
	timer_enable_counter(TIM2);
	__asm__ volatile ("wfi");
}


static void delay_ms(uint32_t ms)
{
	while (ms--)
	delay_us(1000);
}


void tim2_isr(void)
{
	timer_clear_flag(TIM2, TIM_SR_UIF);
}


int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOD);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, GPIO12 | GPIO13 | GPIO14 | GPIO15);

	timer_init(16 - 1);
	cm_enable_interrupts();

	while(1){
		gpio_toggle(GPIOD, GPIO12 | GPIO13 | GPIO14 | GPIO15);
		delay_ms(1000);
	}

}