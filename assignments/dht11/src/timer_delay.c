#include "timer_delay.h"

uint32_t count = 0;

void softdelay(uint32_t N)
{
	while (N--) __asm__("nop");
}

 void timer_delay_init(uint16_t prescaler)
 {
 	rcc_periph_clock_enable(RCC_TIM2);

 	timer_set_prescaler(TIM2, prescaler);

 	timer_disable_preload(TIM2);

 	timer_one_shot_mode(TIM2);

 	timer_update_on_overflow(TIM2);

 	timer_enable_irq(TIM2, TIM_DIER_UIE);
 	timer_clear_flag(TIM2, TIM_SR_UIF);

 	nvic_set_priority(NVIC_TIM2_IRQ, 3);
	nvic_enable_irq(NVIC_TIM2_IRQ);

	cm_enable_interrupts();
 }
  void timer_init(uint16_t prescaler)
 {
 	rcc_periph_clock_enable(RCC_TIM5);

 	timer_set_prescaler(TIM5, prescaler);

 	timer_disable_preload(TIM5);

 	timer_one_shot_mode(TIM5);

 	timer_update_on_overflow(TIM5);

 	timer_enable_irq(TIM5, TIM_DIER_UIE);
 	timer_clear_flag(TIM5, TIM_SR_UIF);

 	nvic_set_priority(NVIC_TIM5_IRQ, 2);
	nvic_enable_irq(NVIC_TIM5_IRQ);

	//cm_enable_interrupts();
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
void check(uint32_t us)
{
	
	timer_set_period(TIM5, 0xFFFF);
	timer_generate_event(TIM5, TIM_EGR_UG);
	timer_enable_counter(TIM5);
	__dmb();
	softdelay(us);
	timer_get_counter(TIM5_CNT);

	
}

void delay_ms(uint32_t ms)
{
	while (ms--)
	delay_us(1000);
}


void tim2_isr(void)
{
	timer_clear_flag(TIM2, TIM_SR_UIF);
}

void tim5_isr(void)
{
	timer_clear_flag(TIM5, TIM_SR_UIF);
}
/*
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

}*/