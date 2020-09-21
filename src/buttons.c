#include "pin.h"
#include "timer_delay.h"
#include "buttons.h"
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/exti.h>


const sk_pin btn_right 	= {.port = SK_PORTC, .pin = 11, .isinverse = true};
const sk_pin btn_middle = {.port = SK_PORTA, .pin = 15, .isinverse = true};

const sk_pin btn_up 	= {.port = SK_PORTC, .pin = 6, .isinverse = true};
const sk_pin btn_down 	= {.port = SK_PORTC, .pin = 8, .isinverse = true};
const sk_pin btn_left 	= {.port = SK_PORTC, .pin = 9, .isinverse = true};

bool btn_up_status = false;
bool btn_down_status = false;
bool btn_right_status = false;
bool btn_left_status = false;
bool btn_middle_status = false;

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
}

bool sk_btn_debounce(sk_pin pin)
{
    //debouncing
    int cnt = 0;
    for(int i = 0; i < 10; i++) {
        if((sk_pin_read(pin)))
            cnt++;
    }

    if(cnt >= 5){
        while((sk_pin_read(pin)));
        return true;
    } else
        return false;
}

void exti15_10_isr(void)
{
	if (exti_get_flag_status(EXTI15)) {

		if(sk_btn_debounce(btn_middle)) {
			sk_pin_toggle(sk_io_led_blue);
			sk_pin_toggle(sk_io_led_red);
			sk_pin_toggle(sk_io_led_green);
			sk_pin_toggle(sk_io_led_orange);
			btn_middle_status = !btn_middle_status;
		}
		exti_reset_request((1 << btn_middle.pin));
	}

	if(exti_get_flag_status(EXTI11)) {

		if(sk_btn_debounce(btn_right)) {
			sk_pin_toggle(sk_io_led_blue);
			btn_right_status = !btn_right_status;
		}
		exti_reset_request((1 << btn_right.pin));
	}	
		
}

void exti9_5_isr(void)
{
	
	if (exti_get_flag_status(EXTI6)) {
		if(sk_btn_debounce(btn_up)) {
			sk_pin_toggle(sk_io_led_red);
			btn_up_status = !btn_up_status;
		}
		exti_reset_request((1 << btn_up.pin));
	}

	if (exti_get_flag_status(EXTI8)) {
		if(sk_btn_debounce(btn_down)) {
			sk_pin_toggle(sk_io_led_green);
			btn_down_status = !btn_down_status;
		}
		exti_reset_request((1 << btn_down.pin));
	}
	if (exti_get_flag_status(EXTI9)) {
		if(sk_btn_debounce(btn_left)) {
			sk_pin_toggle(sk_io_led_orange);
			btn_left_status = !btn_left_status;
		}
		exti_reset_request((1 << btn_left.pin));
	}
}