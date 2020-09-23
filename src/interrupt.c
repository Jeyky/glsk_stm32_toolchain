#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
//#include "abs_systick.h"
//#include "pin.h"
#include "timer_delay.h"
#include "clkset.h"
//#include "stdio.h"
#include "lcd_hd44780.h"

const sk_pin led_green 	= {.port = SK_PORTD, .pin = 12, .isinverse = false};
const sk_pin led_orange	= {.port = SK_PORTD, .pin = 13, .isinverse = false};
const sk_pin led_red 	= {.port = SK_PORTD, .pin = 14, .isinverse = false};
const sk_pin led_blue 	= {.port = SK_PORTD, .pin = 15, .isinverse = false};

const sk_pin btn_up 	= {.port = SK_PORTC, .pin = 6, .isinverse = true};
const sk_pin btn_down 	= {.port = SK_PORTC, .pin = 8, .isinverse = true};
const sk_pin btn_left 	= {.port = SK_PORTC, .pin = 9, .isinverse = true};
const sk_pin btn_right 	= {.port = SK_PORTC, .pin = 11, .isinverse = true};
const sk_pin btn_middle = {.port = SK_PORTA, .pin = 15, .isinverse = true};

//====================================================================================================
/*struct sk_lcd lcd = {
		.pin_group_data = &sk_io_lcd_data,
		.pin_rs = &sk_io_lcd_rs,
		.pin_en = &sk_io_lcd_en,
		.pin_rw = &sk_io_lcd_rw,
		.pin_bkl = &sk_io_lcd_bkl,
		//.set_backlight_func = &test_bkl_func,
		.delay_func_us = NULL,
		.delay_func_ms = &delay_ms,
		.is4bitinterface = true
	};*/
//====================================================================================================

uint32_t counter = 0;
static bool __rising_falling = false;

extern uint32_t count;
uint32_t countt = 1;

const sk_pin trig 	= {.port = SK_PORTA, .pin = 3, .isinverse = false};
const sk_pin btn_user = {.port = SK_PORTA, .pin = 0, .isinverse = false};
const sk_pin out = {.port = SK_PORTA, .pin = 1, .isinverse = false};


/*void delay_uss(uint32_t us)
{
	if(!us)
		return;
	timer_set_period(TIM5, 0xFFFF);

	timer_generate_event(TIM5, TIM_EGR_UG);

	__dmb();	
	timer_enable_counter(TIM5);

	softdelay(1000);
	//countt = TIM_CNT(TIM2);
	counter = TIM_CNT(TIM5);
	//countt = timer_get_counter(TIM2_CNT);
	__asm__ volatile ("wfi");
	//countt = timer_get_counter(TIM2_CNT);
	//countt = TIM_CNT(TIM2);

}*/

void delay_uus(uint32_t us)
{
	if(!us)
		return;
	timer_set_period(TIM2, us);

	timer_generate_event(TIM2, TIM_EGR_UG);

	__dmb();	
	timer_enable_counter(TIM2);
	softdelay(1000);
	count = timer_get_counter(TIM2_CNT);
	__asm__ volatile ("wfi");
}

void sk_inter_exti_init(sk_pin pin, enum exti_trigger_type trigger)
{
	exti_select_source((1 << pin.pin), sk_pin_port_to_gpio(pin.port));
	exti_set_trigger((1 << pin.pin), trigger);
	exti_enable_request((1 << pin.pin));
	exti_reset_request((1 << pin.pin));
}
uint32_t meas(void)
{
	//printf("in meas\n");
	//uint32_t max_val = 1000000;
	//sk_pin_set(out, 1);
	delay_ms(60);
	counter = 0;
	delay_ms(1);
	sk_pin_set(out, 1);
	sk_pin_set(trig, 1);
	delay_us(10);
	sk_pin_set(out, 0);
	sk_pin_set(trig, 0);
	delay_us(1000);
	//while(max_val--);
	return counter;

}
uint32_t measure(void)
{
	//printf("in measure\n");
	//uint32_t max_val = 0;
	delay_ms(60);
	counter = 0;
	delay_ms(1);
	sk_pin_set(trig, 1);
	delay_us(10);
	sk_pin_set(trig, 0);
	//while(!sk_pin_read(btn_user) && max_val < 20000000) max_val++;
	//printf("in measure after 1 while\n");
	while(sk_pin_read(btn_user));
	//printf("in measure after 2 while\n");
	//printf("counter = %d\n", counter);
	return counter;

}
void exti0_isr(void) 
{
	//abs_delay_ms(1);
	//printf("IN ISR\n");
	__rising_falling = !__rising_falling;
	printf("bool=%d\n", __rising_falling);
	if (__rising_falling) {
		timer_set_period(TIM1, 0xFFFF);
		timer_generate_event(TIM1, TIM_EGR_UG);
		__dmb();	
		timer_enable_counter(TIM2);
		printf("IN IF\n");
	} else {
		counter = timer_get_counter(TIM1_CNT);
		printf("IN ELSE\nCOUNTER=%d\n", counter);
	}
	//uint32_t i=0;
	//printf("counter = %d\n", counter);
	//sk_pin_set(out, 0);
	//exti_enable_request(EXTI0);
	exti_reset_request((1 << echo.pin));
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
	glsk_pins_init(false);					//

	struct sk_lcd lcd = {
		.pin_group_data = &sk_io_lcd_data,
		.pin_rs = &sk_io_lcd_rs,
		.pin_en = &sk_io_lcd_en,
		.pin_rw = &sk_io_lcd_rw,
		.pin_bkl = &sk_io_lcd_bkl,
		//.set_backlight_func = &test_bkl_func,
		.delay_func_us = NULL,
		.delay_func_ms = &delay_ms,
		.is4bitinterface = true
	};

	sk_lcd_set_backlight(&lcd, 0xFF);
	
	lcd_init_4bit(&lcd);

	//lcd_send_string(&lcd, "slava!");
	//lcd_send_cmd(&lcd, 0xc0);
	//lcd_send_string(&lcd, "ukraine");
	lcd_set_cursor(&lcd, 1, 0);
	char *buffer = "kek";
	//snprintf(buffer, 20, "Count=%d", counter);
	lcd_send_string(&lcd, buffer);
	//snprintf(tmp, sk_arr_len(tmp), "%5.1f", temp);
	//snprintf(buffer, sk_arr_len(buffer), "T=%-5s°C A=%u", tmp, (unsigned int)adcval);

	//abs_sys_tick_init(16000000ul / 1000ul, 2);
	clock_hse_init(4, 168, 2, 7, 0);
	timer_delay_init(84 - 1);
	timer_init(84 - 1);

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

	sk_pin_mode_setup(out, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
	sk_pin_mode_setup(trig, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);


	scb_set_priority_grouping(SCB_AIRCR_PRIGROUP_GROUP4_SUB4);

	const uint8_t group = 2;
	const uint8_t subgroup = 0;

	nvic_set_priority(NVIC_EXTI0_IRQ, (group << 2) | subgroup);
	//nvic_set_priority(NVIC_EXTI15_10_IRQ, ((1 + group) << 2) | subgroup);

	rcc_periph_clock_enable(RCC_SYSCFG);

	sk_inter_exti_init(btn_up, EXTI_TRIGGER_FALLING);
	sk_inter_exti_init(btn_right, EXTI_TRIGGER_FALLING);
	sk_inter_exti_init(btn_down, EXTI_TRIGGER_FALLING);
	sk_inter_exti_init(btn_left, EXTI_TRIGGER_FALLING);
	sk_inter_exti_init(btn_middle, EXTI_TRIGGER_FALLING);

	sk_inter_exti_init(btn_user, EXTI_TRIGGER_BOTH);

	nvic_enable_irq(NVIC_EXTI0_IRQ);
	//nvic_enable_irq(NVIC_EXTI9_5_IRQ);
	//nvic_enable_irq(NVIC_EXTI15_10_IRQ);

	cm_enable_interrupts();

	initialise_monitor_handles();	

	delay_uss(1000);
	printf("COUNTT = %d\n", countt);
	printf("COUNTER = %d\n", counter);


	uint32_t time = meas();
	//printf("time = %f\n", time);
	double distance = time * 0.034 / 2;

	//printf("distance = %f\n", distance);

	for(int i =0; i<10; i++){
		time = meas();
		printf("time = %f\n", time);
		distance = time * 0.034 / 2;

		printf("distance = %f\n", distance);
	}
	while(1){
		//gpio_set(GPIOD, GPIO13);
	}
}

