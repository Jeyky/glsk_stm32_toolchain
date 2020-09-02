#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "abs_systick.h"
#include "pin.h"

const sk_pin led_green 	= {.port = SK_PORTD, .pin = 12, .isinverse = false};
const sk_pin led_orange	= {.port = SK_PORTD, .pin = 13, .isinverse = false};
const sk_pin led_red 	= {.port = SK_PORTD, .pin = 14, .isinverse = false};
const sk_pin led_blue 	= {.port = SK_PORTD, .pin = 15, .isinverse = false};

const sk_pin btn_up 	= {.port = SK_PORTC, .pin = 6, .isinverse = true};
const sk_pin btn_down 	= {.port = SK_PORTC, .pin = 8, .isinverse = true};
const sk_pin btn_left 	= {.port = SK_PORTC, .pin = 9, .isinverse = true};
const sk_pin btn_right 	= {.port = SK_PORTC, .pin = 11, .isinverse = true};
const sk_pin btn_middle = {.port = SK_PORTA, .pin = 15, .isinverse = true};

const sk_pin btn_user = {.port = SK_PORTA, .pin = 0, .isinverse = false};


void softdelay(uint32_t N)
{
	while (N--) __asm__("nop");
}

void sk_inter_exti_init(sk_pin pin, enum exti_trigger_type trig)
{
	exti_select_source((1 << pin.pin), sk_pin_port_to_gpio(pin.port));
	exti_set_trigger((1 << pin.pin), trig);
	exti_enable_request((1 << pin.pin));
	exti_reset_request((1 << pin.pin));
}

void exti0_isr(void) 
{
	//abs_delay_ms(1);
	softdelay(200);
	if(sk_pin_read(btn_user))
	{
		sk_pin_toggle(led_green);
		sk_pin_toggle(led_orange);
		sk_pin_toggle(led_blue);
		sk_pin_toggle(led_red);
	}
	exti_enable_request(EXTI0);
}
void exti15_10_isr(void)
{
	softdelay(200);
	if (sk_pin_read(btn_right)) {
		sk_pin_toggle(led_blue);
		exti_reset_request((1 << btn_right.pin));
	}
	if (sk_pin_read(btn_middle)) {
		sk_pin_toggle(led_green);
		sk_pin_toggle(led_orange);
		sk_pin_toggle(led_blue);
		sk_pin_toggle(led_red);
		exti_reset_request((1 << btn_middle.pin));
	}
}


void exti9_5_isr(void)
{
	softdelay(200);
	if (sk_pin_read(btn_left)) {
		sk_pin_toggle(led_orange);
		exti_reset_request((1 << btn_left.pin));
	}
	if (sk_pin_read(btn_up)) {
		sk_pin_toggle(led_red);
		exti_reset_request((1 << btn_up.pin));
	}
	if (sk_pin_read(btn_down)) {
		sk_pin_toggle(led_green);
		exti_reset_request((1 << btn_down.pin));
	}
}

int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOC);

	abs_sys_tick_init(16000000ul / 1000ul, 2);

	sk_pin_mode_setup(led_green, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
    sk_pin_mode_setup(led_orange, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
    sk_pin_mode_setup(led_red, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
    sk_pin_mode_setup(led_blue, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);

    sk_pin_mode_setup(btn_up, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP);
	sk_pin_mode_setup(btn_left, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP);
	sk_pin_mode_setup(btn_right, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP);
	sk_pin_mode_setup(btn_down, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP);
	sk_pin_mode_setup(btn_middle, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP);

	sk_pin_mode_setup(btn_user, GPIO_MODE_INPUT, GPIO_PUPD_NONE);


	scb_set_priority_grouping(SCB_AIRCR_PRIGROUP_GROUP4_SUB4);

	const uint8_t group = 2;
	const uint8_t subgroup = 0;

	nvic_set_priority(NVIC_EXTI9_5_IRQ, (group << 2) | subgroup);
	nvic_set_priority(NVIC_EXTI15_10_IRQ, ((1 + group) << 2) | subgroup);

	rcc_periph_clock_enable(RCC_SYSCFG);

	sk_inter_exti_init(btn_up, EXTI_TRIGGER_FALLING);
	sk_inter_exti_init(btn_right, EXTI_TRIGGER_FALLING);
	sk_inter_exti_init(btn_down, EXTI_TRIGGER_FALLING);
	sk_inter_exti_init(btn_left, EXTI_TRIGGER_FALLING);
	sk_inter_exti_init(btn_middle, EXTI_TRIGGER_FALLING);

	sk_inter_exti_init(btn_user, EXTI_TRIGGER_RISING);

	nvic_enable_irq(NVIC_EXTI0_IRQ);
	nvic_enable_irq(NVIC_EXTI9_5_IRQ);
	nvic_enable_irq(NVIC_EXTI15_10_IRQ);

	cm_enable_interrupts();

	while(1){
		//gpio_set(GPIOD, GPIO13);
	}
}

