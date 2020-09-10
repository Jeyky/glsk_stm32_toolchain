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
	rcc_periph_clock_enable(RCC_TIM3);
	rcc_periph_clock_enable(RCC_TIM1);


	timer_set_mode(TIM4, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
	timer_set_mode(TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	timer_set_prescaler(TIM4, 999);
	timer_set_prescaler(TIM3, 467 - 1);
	timer_set_prescaler(TIM1, 999)

	timer_enable_preload(TIM4);
	timer_continuous_mode(TIM4);

	timer_enable_preload(TIM3);
	timer_continuous_mode(TIM3);

	timer_enable_preload(TIM1);
	timer_continuous_mode(TIM1)

	timer_set_repetition_counter(TIM4, 0);
	timer_set_repetition_counter(TIM3, 0);
	timer_set_repetition_counter(TIM1, 0);

	timer_set_period(TIM4, 99);
	timer_set_period(TIM3, 3600 - 1);
	timer_set_period(TIM1, 2000 - 1);

	

	timer_disable_oc_output(TIM4, TIM_OC1);
	timer_disable_oc_output(TIM4, TIM_OC2);
	timer_disable_oc_output(TIM4, TIM_OC3);
	timer_disable_oc_output(TIM4, TIM_OC4);

	timer_disable_oc_output(TIM3, TIM_OC1);

	timer_disable_oc_output(TIM1, TIM_OC1);

	timer_set_oc_mode(TIM4, TIM_OC1, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM4, TIM_OC2, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM4, TIM_OC3, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM4, TIM_OC4, TIM_OCM_PWM1);

	timer_set_oc_mode(TIM3, TIM_OC1, TIM_OCM_PWM1);

	timer_set_oc_mode(TIM1, TIM_OC1, TIM_OCM_PWM1);


	timer_enable_oc_preload(TIM4, TIM_OC1);
	timer_enable_oc_preload(TIM4, TIM_OC2);
	timer_enable_oc_preload(TIM4, TIM_OC3);

	timer_enable_oc_preload(TIM3, TIM_OC1);

	timer_enable_oc_preload(TIM1, TIM_OC1);

	timer_set_oc_value(TIM4, TIM_OC1, 0);
	timer_set_oc_value(TIM4, TIM_OC2, 0);
	timer_set_oc_value(TIM4, TIM_OC3, 0);
	timer_set_oc_value(TIM4, TIM_OC4, 0);

	timer_set_oc_value(TIM3, TIM_OC1, 0);

	timer_set_oc_value(TIM1, TIM_OC1, 1000);

	timer_enable_oc_output(TIM4, TIM_OC1);
	timer_enable_oc_output(TIM4, TIM_OC2);
	timer_enable_oc_output(TIM4, TIM_OC3);
	timer_enable_oc_output(TIM4, TIM_OC4);

	timer_enable_oc_output(TIM3, TIM_OC1);

	timer_enable_oc_output(TIM1, TIM_OC1);

	timer_generate_event(TIM4, TIM_EGR_UG);

	timer_generate_event(TIM3, TIM_EGR_UG);

	timer_generate_event(TIM1, TIM_EGR_UG);

	timer_enable_counter(TIM4);

	timer_enable_counter(TIM3);

	timer_enable_counter(TIM1);
}

const sk_pin sk_led_orange = { .port=SK_PORTD, .pin=13, .isinverse=false };
const sk_pin sk_led_red    = { .port=SK_PORTD, .pin=14, .isinverse=false };
const sk_pin sk_led_green  = { .port=SK_PORTD, .pin=12, .isinverse=false };
const sk_pin sk_led_blue   = { .port=SK_PORTD, .pin=15, .isinverse=false };

const sk_pin shim  = { .port=SK_PORTA, .pin=6, .isinverse=false };


void set_speed(uint16_t value)
{
	uint16_t prescaler = 84000/(6.7*value);
	prescaler = 84000/prescaler;
	timer_set_prescaler(TIM1, prescaler - 1);
	timer_generate_event(TIM1, TIM_EGR_UG);
	timer_enable_counter(TIM1);
}

void rotate_servo(uint16_t deg)
{
	uint16_t position = 100 + deg*2;
	timer_set_oc_value(TIM3, TIM_OC1, position);
}

int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOA);
	//gpio_mode_setup(GPIOD, GPIO_MODE_AF, GPIO_PUPD_NONE, GPIO12 | GPIO13 | GPIO14 | GPIO15);
	sk_pin_mode_setup(sk_led_orange, GPIO_MODE_AF, GPIO_PUPD_NONE);
	sk_pin_mode_setup(sk_led_red, GPIO_MODE_AF, GPIO_PUPD_NONE);
	sk_pin_mode_setup(sk_led_green, GPIO_MODE_AF, GPIO_PUPD_NONE);
	sk_pin_mode_setup(sk_led_blue, GPIO_MODE_AF, GPIO_PUPD_NONE);

	sk_pin_mode_setup(shim, GPIO_MODE_AF, GPIO_PUPD_NONE);

	//gpio_set_output_options(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, GPIO12 | GPIO13 | GPIO14 | GPIO15);
	sk_pin_set_output_options(sk_led_orange, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ);
	sk_pin_set_output_options(sk_led_red, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ);
	sk_pin_set_output_options(sk_led_green, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ);
	sk_pin_set_output_options(sk_led_blue, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ);

	sk_pin_set_output_options(shim, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ);
	
	//gpio_set_af(GPIOD, GPIO_AF2, GPIO12 | GPIO13 | GPIO14 | GPIO15);
 	sk_pin_set_af(sk_led_orange, GPIO_AF2);
 	sk_pin_set_af(sk_led_red, GPIO_AF2);
 	sk_pin_set_af(sk_led_green, GPIO_AF2);
 	sk_pin_set_af(sk_led_blue, GPIO_AF2);

 	sk_pin_set_af(shim, GPIO_AF2);

	clock_hse_init(4, 168, 2, 7, 0);
	abs_sys_tick_init(168000000ul/100000ul, 2);
	abs_delay_ms(2000);
	pwm_init();

	/*timer_set_oc_value(TIM3, TIM_OC1, 180);
	timer_set_oc_value(TIM4, TIM_OC1, 99);
	abs_delay_ms(2000);
	timer_set_oc_value(TIM3, TIM_OC1, 270);
	timer_set_oc_value(TIM4, TIM_OC2, 99);
	abs_delay_ms(2000);
	timer_set_oc_value(TIM3, TIM_OC1, 360);
	timer_set_oc_value(TIM4, TIM_OC3, 99);*/
	//timer_set_oc_value(TIM3, TIM_OC1, 20);
	uint8_t a=4;
	while(a--){
		abs_delay_ms(500);
		rotate_servo(180);
		timer_set_oc_value(TIM4, TIM_OC3, 0);
		abs_delay_ms(500);
		timer_set_oc_value(TIM4, TIM_OC3, 99);
		rotate_servo(0);
	}
	
	/*
	timer_set_oc_value(TIM3, TIM_OC1, 20);
	abs_delay_ms(3000);
	timer_set_oc_value(TIM3, TIM_OC1, 450);*/
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