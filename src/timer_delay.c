#include "timer_delay.h"
#include "pin.h"
#include "stdbool.h"


uint32_t count = 0;

const sk_pin led_bluee = {.pin = 15, .port = SK_PORTD, .isinverse = false};

void softdelay(uint32_t N)
{
	while (N--) __asm__("nop");
}

 void timer_delay_init(uint16_t prescaler)
 {
 	rcc_periph_clock_enable(RCC_TIM7);

 	timer_set_prescaler(TIM7, prescaler);

 	timer_disable_preload(TIM7);

 	timer_one_shot_mode(TIM7);

 	timer_update_on_overflow(TIM7);

 	timer_enable_irq(TIM7, TIM_DIER_UIE);
 	timer_clear_flag(TIM7, TIM_SR_UIF);

 	nvic_set_priority(NVIC_TIM7_IRQ, 2);
	nvic_enable_irq(NVIC_TIM7_IRQ);

	cm_enable_interrupts();
 }
  void timer_init(uint16_t prescaler)
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

	//cm_enable_interrupts();
 }


void delay_us(uint32_t us)
{
	if(!us)
		return;
	timer_set_period(TIM7, us);

	timer_generate_event(TIM7, TIM_EGR_UG);

	__dmb();	
	timer_enable_counter(TIM7);
	__asm__ volatile ("wfi");
}


void delay_ms(uint32_t ms)
{
	while (ms--)
	delay_us(1000);
}


void tim7_isr(void)
{
	timer_clear_flag(TIM7, TIM_SR_UIF);
}

void tim2_isr(void)
{
	timer_clear_flag(TIM2, TIM_SR_UIF);
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