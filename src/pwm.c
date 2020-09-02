#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include "abs_systick.h"
#include "pin.h"
#include "clkset.h"

void pwm_init(void)
{
	rcc_periph_clock_enable(RCC_TIM4);

	timer_set_mode(TIM4, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	timer_set_prescaler(TIM4, 999);

	timer_enable_preload(TIM4);
	timer_continuous_mode(TIM4);

	timer_set_repetition_counter(TIM4, 0);

	timer_set_period(TIM4, 255);

	

	timer_disable_oc_output(TIM4, TIM_OC1);
	timer_disable_oc_output(TIM4, TIM_OC2);
	timer_disable_oc_output(TIM4, TIM_OC3);
	timer_disable_oc_output(TIM4, TIM_OC4);

	timer_set_oc_mode(TIM4, TIM_OC1, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM4, TIM_OC2, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM4, TIM_OC3, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM4, TIM_OC4, TIM_OCM_PWM1);

	timer_enable_oc_preload(TIM4, TIM_OC1);
	timer_enable_oc_preload(TIM4, TIM_OC2);
	timer_enable_oc_preload(TIM4, TIM_OC3);

	timer_set_oc_value(TIM4, TIM_OC1, 0);
	timer_set_oc_value(TIM4, TIM_OC2, 0);
	timer_set_oc_value(TIM4, TIM_OC3, 0);
	timer_set_oc_value(TIM4, TIM_OC4, 0);

	timer_enable_oc_output(TIM4, TIM_OC1);
	timer_enable_oc_output(TIM4, TIM_OC2);
	timer_enable_oc_output(TIM4, TIM_OC3);
	timer_enable_oc_output(TIM4, TIM_OC4);

	timer_generate_event(TIM4, TIM_EGR_UG);

	timer_enable_counter(TIM4);
}

const sk_pin sk_led_orange = { .port=SK_PORTD, .pin=13, .isinverse=false };
const sk_pin sk_led_red    = { .port=SK_PORTD, .pin=14, .isinverse=false };
const sk_pin sk_led_green  = { .port=SK_PORTD, .pin=12, .isinverse=false };
const sk_pin sk_led_blue   = { .port=SK_PORTD, .pin=15, .isinverse=false };
int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOD);
	//gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO12 | GPIO13 | GPIO14 | GPIO15);
	sk_pin_mode_setup(sk_led_orange, GPIO_MODE_AF, GPIO_PUPD_NONE);
	sk_pin_mode_setup(sk_led_red, GPIO_MODE_AF, GPIO_PUPD_NONE);
	sk_pin_mode_setup(sk_led_green, GPIO_MODE_AF, GPIO_PUPD_NONE);
	sk_pin_mode_setup(sk_led_blue, GPIO_MODE_AF, GPIO_PUPD_NONE);

	//gpio_set_output_options(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO12 | GPIO13 | GPIO14 | GPIO15);
	sk_pin_set_output_options(sk_led_orange, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ);
	sk_pin_set_output_options(sk_led_red, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ);
	sk_pin_set_output_options(sk_led_green, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ);
	sk_pin_set_output_options(sk_led_blue, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ);
	
	//gpio_set_af(GPIOD, GPIO_AF2, GPIO12 | GPIO13 | GPIO14 | GPIO15);
 	sk_pin_set_af(sk_led_orange, GPIO_AF2);
 	sk_pin_set_af(sk_led_red, GPIO_AF2);
 	sk_pin_set_af(sk_led_green, GPIO_AF2);
 	sk_pin_set_af(sk_led_blue, GPIO_AF2);

	clock_hse_init(4, 168, 2, 7, 0);
	abs_sys_tick_init(168000000ul/100000ul, 2);
	
	pwm_init();
	while(1){
		for(int duty = 0; duty <= 255; duty++){
			abs_delay_ms(5);
			timer_set_oc_value(TIM4, TIM_OC1, duty);
			timer_set_oc_value(TIM4, TIM_OC2, duty);
			timer_set_oc_value(TIM4, TIM_OC3, duty);
			timer_set_oc_value(TIM4, TIM_OC4, duty);
		}
		for(int duty = 255; duty >= 0; duty--){
			abs_delay_ms(5);
			timer_set_oc_value(TIM4, TIM_OC1, duty);
			timer_set_oc_value(TIM4, TIM_OC2, duty);
			timer_set_oc_value(TIM4, TIM_OC3, duty);
			timer_set_oc_value(TIM4, TIM_OC4, duty);
		}
	}



}