#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/exti.h>
#include "pin.h"
#include "pwm.h"
#include "timer_delay.h"

const sk_pin servo  		= {.port = SK_PORTA, .pin = 6, .isinverse = false};
const sk_pin speedometer 	= {.port = SK_PORTE, .pin = 5, .isinverse = false};
const sk_pin alarm 			= {.port = SK_PORTA, .pin = 1, .isinverse = false};

//void pwm_init(void)
void pwm_init(uint32_t tim, enum rcc_periph_clken rcc_tim, enum tim_oc_id oc, uint32_t prescaler, uint32_t period)
{

	rcc_periph_clock_enable(rcc_tim);

	timer_set_mode(tim, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);

	timer_set_prescaler(tim, prescaler);

	timer_enable_preload(tim);
	
	timer_continuous_mode(tim);

	timer_set_repetition_counter(tim, 0);
	
	timer_set_period(tim, period);

	timer_disable_oc_output(tim, oc);

	timer_set_oc_mode(tim, oc, TIM_OCM_PWM1);

	timer_enable_oc_preload(tim, oc);

	timer_set_oc_value(tim, oc, 0);

	timer_enable_oc_output(tim, oc);

	timer_generate_event(tim, TIM_EGR_UG);

	timer_enable_counter(tim);
}

void alarm_init(void)
{
	pwm_init(TIM5, RCC_TIM5, TIM_OC2, 84 - 1, 1000 - 1);		// timer 5 responds for alarm

	rcc_periph_clock_enable(RCC_GPIOA);

	sk_pin_mode_setup(alarm, GPIO_MODE_AF, GPIO_PUPD_NONE);

	sk_pin_set_output_options(alarm, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ);

	sk_pin_set_af(alarm, GPIO_AF2);
}

void alarm_on(void)
{
	timer_set_oc_value(TIM5, TIM_OC2, 10);
	delay_ms(800);

}
void sound_click(void)
{
	timer_set_oc_value(TIM5, TIM_OC2, 10);
	delay_ms(40);
	timer_set_oc_value(TIM5, TIM_OC2, 0);
}
void alarm_off(void)
{
	timer_set_oc_value(TIM5, TIM_OC2, 0);
}

void servo_init(void)
{
	pwm_init(TIM3, RCC_TIM3, TIM_OC1, 467 - 1, 3600 - 1);		// timer 3 responds for servo motor

	rcc_periph_clock_enable(RCC_GPIOA);

	sk_pin_mode_setup(servo, GPIO_MODE_AF, GPIO_PUPD_NONE);

	sk_pin_set_output_options(servo, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ);

	sk_pin_set_af(servo, GPIO_AF2);
}

void speedometer_init(void)
{
	pwm_init(TIM9, RCC_TIM9, TIM_OC1, 1000 -1, 2000 - 1);		// timer 9 responds for speedometer

	rcc_periph_clock_enable(RCC_GPIOE);

	sk_pin_mode_setup(speedometer, GPIO_MODE_AF, GPIO_PUPD_NONE);

	sk_pin_set_output_options(speedometer, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ);

	sk_pin_set_af(speedometer, GPIO_AF3);

}

void speedometer_set_speed(uint16_t value)
{
	uint16_t prescaler = 84000/(6.7*value);

	timer_set_prescaler(TIM9, prescaler - 1);

	//timer_generate_event(TIM9, TIM_EGR_UG);
	//__dmb();

	timer_enable_counter(TIM9);
}

void servo_rotate(uint16_t deg)
{
	//if (!rotate) return;
	uint16_t position = 100 + deg*2;
	timer_set_oc_value(TIM3, TIM_OC1, position);
}

