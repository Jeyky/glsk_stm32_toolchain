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
/*
const sk_pin btn_right 	= {.port = SK_PORTC, .pin = 11, .isinverse = true};
const sk_pin btn_middle = {.port = SK_PORTA, .pin = 15, .isinverse = true};

const sk_pin btn_up 	= {.port = SK_PORTC, .pin = 6, .isinverse = true};
const sk_pin btn_down 	= {.port = SK_PORTC, .pin = 8, .isinverse = true};
const sk_pin btn_left 	= {.port = SK_PORTC, .pin = 9, .isinverse = true};
*/
//double threshold = 20.0;

//bool show_on_speedometer = true;

//bool rotate = false;
/*
void button_init(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOC);

	sk_pin_mode_setup(btn_right, 	GPIO_MODE_INPUT, GPIO_PUPD_PULLUP);
	sk_pin_mode_setup(btn_middle,	GPIO_MODE_INPUT, GPIO_PUPD_PULLUP);

	sk_pin_mode_setup(btn_up,	GPIO_MODE_INPUT, GPIO_PUPD_PULLUP);
	sk_pin_mode_setup(btn_down,	GPIO_MODE_INPUT, GPIO_PUPD_PULLUP);
	sk_pin_mode_setup(btn_left,	GPIO_MODE_INPUT, GPIO_PUPD_PULLUP);


	scb_set_priority_grouping(SCB_AIRCR_PRIGROUP_GROUP4_SUB4);

	const uint8_t group = 2;
	const uint8_t subgroup = 0;

	nvic_set_priority(NVIC_EXTI9_5_IRQ, 	(group << 2) | subgroup);
	nvic_set_priority(NVIC_EXTI15_10_IRQ, 	((1 + group) << 2) | subgroup);

	rcc_periph_clock_enable(RCC_SYSCFG);

	sk_inter_exti_init(btn_right, 	EXTI_TRIGGER_FALLING);
	sk_inter_exti_init(btn_middle, 	EXTI_TRIGGER_FALLING);

	sk_inter_exti_init(btn_up, 		EXTI_TRIGGER_FALLING);
	sk_inter_exti_init(btn_down, 	EXTI_TRIGGER_FALLING);
	sk_inter_exti_init(btn_left, 	EXTI_TRIGGER_FALLING);

	nvic_enable_irq(NVIC_EXTI9_5_IRQ);
	nvic_enable_irq(NVIC_EXTI15_10_IRQ);

	cm_enable_interrupts();
}*/
/*
void exti15_10_isr(void)
{
	if (sk_pin_read(btn_right)) {

		rotate = !rotate;

		exti_reset_request((1 << btn_right.pin));		
	}

	if (sk_pin_read(btn_middle)) {

		show_on_speedometer = !show_on_speedometer;

		exti_reset_request((1 << btn_middle.pin));
	}
	
		
}

void exti9_5_isr(void)
{
	//softdelay(200);
	
	if (sk_pin_read(btn_up)) {

		if (threshold <= 235) {
			threshold+=5;
		}

		exti_reset_request((1 << btn_up.pin));
	}

	if (sk_pin_read(btn_down)) {

		if (threshold > 5) {
			threshold-=5;
		}

		exti_reset_request((1 << btn_down.pin));
	}

	if (sk_pin_read(btn_left)) {

		rotate = !rotate;

		exti_reset_request((1 << btn_left.pin));
	}
}
*/

void pwm_init(void)
{
	
	rcc_periph_clock_enable(RCC_TIM3);	// tim3 84mhz 16bit
	rcc_periph_clock_enable(RCC_TIM9); 	// tim9 168mhz 16 bit
	rcc_periph_clock_enable(RCC_TIM5);



	timer_set_mode(TIM3, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);	// timer 3 responds for servo motor
	timer_set_mode(TIM9, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP); // timer 9 responds for speedometer
	timer_set_mode(TIM5, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP); // timer 5 responds for alarm

	
	timer_set_prescaler(TIM3, 467 - 1);
	timer_set_prescaler(TIM9, 1000 -1);
	timer_set_prescaler(TIM5, 84 - 1);


	timer_enable_preload(TIM3);
	timer_continuous_mode(TIM3);

	timer_enable_preload(TIM9);
	timer_continuous_mode(TIM9);

	timer_enable_preload(TIM5);
	timer_continuous_mode(TIM5);

	timer_set_repetition_counter(TIM3, 0);
	timer_set_repetition_counter(TIM9, 0);
	timer_set_repetition_counter(TIM5, 0);

	timer_set_period(TIM3, 3600 - 1);
	timer_set_period(TIM9, 2000 - 1);
	timer_set_period(TIM5, 1000 - 1);

	timer_disable_oc_output(TIM3, TIM_OC1);
	timer_disable_oc_output(TIM9, TIM_OC1);
	timer_disable_oc_output(TIM5, TIM_OC2);


	timer_set_oc_mode(TIM3, TIM_OC1, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM9, TIM_OC1, TIM_OCM_PWM1);
	timer_set_oc_mode(TIM5, TIM_OC2, TIM_OCM_PWM1);

	timer_enable_oc_preload(TIM3, TIM_OC1);
	timer_enable_oc_preload(TIM9, TIM_OC1);
	timer_enable_oc_preload(TIM5, TIM_OC2);


	timer_set_oc_value(TIM3, TIM_OC1, 0);
	timer_set_oc_value(TIM9, TIM_OC1, 1500);
	timer_set_oc_value(TIM5, TIM_OC2, 0);

	timer_enable_oc_output(TIM3, TIM_OC1);
	timer_enable_oc_output(TIM9, TIM_OC1);
	timer_enable_oc_output(TIM5, TIM_OC2);

	timer_generate_event(TIM3, TIM_EGR_UG);
	timer_generate_event(TIM9, TIM_EGR_UG);
	timer_generate_event(TIM5, TIM_EGR_UG);

	timer_enable_counter(TIM3);
	timer_enable_counter(TIM9);
	timer_enable_counter(TIM5);
}

void alarm_init(void)
{
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
	delay_ms(20);
	timer_set_oc_value(TIM5, TIM_OC2, 0);
}
void alarm_off(void)
{
	timer_set_oc_value(TIM5, TIM_OC2, 0);
}

void servo_init(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);

	sk_pin_mode_setup(servo, GPIO_MODE_AF, GPIO_PUPD_NONE);

	sk_pin_set_output_options(servo, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ);

	sk_pin_set_af(servo, GPIO_AF2);
}

void speedometer_init(void)
{
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

