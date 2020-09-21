#include <libopencm3/cm3/cortex.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/exti.h>
#include "pin.h"
#include "hcsr04.h"
//#include "clkset.h"
#include "timer_delay.h"

//#include "lcd_hd44780.h"
//#include "printf.h"

const sk_pin trig 	= {.port = SK_PORTA, .pin = 3, .isinverse = false};
const sk_pin echo = {.port = SK_PORTC, .pin = 0, .isinverse = false};

double speed_of_sound = 343.3;
uint32_t counter = 0;

void hcsr04_inint(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOC);

	sk_pin_mode_setup(echo, GPIO_MODE_INPUT, GPIO_PUPD_NONE);
	sk_pin_mode_setup(trig, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);

	scb_set_priority_grouping(SCB_AIRCR_PRIGROUP_GROUP4_SUB4);

	const uint8_t group = 2;
	const uint8_t subgroup = 0;

	nvic_set_priority(NVIC_EXTI0_IRQ, (group << 2) | subgroup);

	rcc_periph_clock_enable(RCC_SYSCFG);

	sk_inter_exti_init(echo, EXTI_TRIGGER_BOTH);
	nvic_enable_irq(NVIC_EXTI0_IRQ);

	cm_enable_interrupts();
}

void exti0_isr(void)
{
	if (sk_pin_read(echo)) {
		timer_set_period(TIM2, 0xFFFFFFFF - 1);
		timer_generate_event(TIM2, TIM_EGR_UG);
		__dmb();
		timer_enable_counter(TIM2);
	} else {
		counter = TIM_CNT(TIM2);
	}
	exti_reset_request(EXTI0);	
}

void hcsr04_strobe_trig(void)
{
	delay_ms(60);

	sk_pin_set(trig, 1);

	delay_us(10);

	sk_pin_set(trig, 0);

	delay_us(1000);
}

double hcsr04_get_distance(void)
{
	delay_ms(10);
	hcsr04_strobe_trig();
	double distance = (counter * speed_of_sound / 10000) / 2;
	if(distance > 240.0) {
		distance = 240.0;
	}
	return distance;
}

double hcsr04_get_precise_distance(void)
{
	double distance[3] = {0.0};
	double min_val;

	//hcsr04_strobe_trig();
	//distance[0] = (counter * speed_of_sound / 10000) / 2;
	distance[0] = hcsr04_get_distance();
	min_val = distance[0];

	for(int i =1; i < 3; i++) {
		//hcsr04_strobe_trig();
		//distance[i] = (counter * speed_of_sound / 10000) / 2;
		distance[i] = hcsr04_get_distance();
		if(min_val > distance[i])
			min_val = distance[i];
	}
	return min_val;
}
/*
static void test_bkl_func(uint8_t val)
{
	sk_pin_set(sk_io_lcd_bkl, (bool)val);
}

extern void lcd_init_4bit(struct sk_lcd *lcd);

int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOE);
	rcc_periph_clock_enable(RCC_GPIOD);
	

	glsk_pins_init(false);

	sk_pin_group_set(sk_io_lcd_data, 0x00);
	sk_pin_set(sk_io_led_orange, true);

	clock_hse_168MHZ_init();
	timer_delay_init(84 - 1);
	timer_init(84 - 1);

	hcsr04_inint();

	struct sk_lcd lcd = {
		.pin_group_data = &sk_io_lcd_data,
		.pin_rs = &sk_io_lcd_rs,
		.pin_en = &sk_io_lcd_en,
		.pin_rw = &sk_io_lcd_rw,
		.pin_bkl = &sk_io_lcd_bkl,
		//.set_backlight_func = &test_bkl_func,
		.delay_func_us = NULL,
		//.delay_func_ms = &abs_delay_ms,
		.delay_func_ms = &delay_ms,
		.is4bitinterface = true
	};

	sk_lcd_set_backlight(&lcd, 0xFF);
	
	lcd_init_4bit(&lcd);
	sk_pin_set(sk_io_led_orange, false);
	sk_pin_set(sk_io_led_blue, true);
	lcd_set_cursor(&lcd, 0, 0);
	lcd_send_string(&lcd, "test prog");
	char buffer[20];
	double distance = hcsr04_get_distance();
	snprintf(buffer, sk_arr_len(buffer), "dis=%.2f   ", distance);
	lcd_set_cursor(&lcd, 1, 0);
	lcd_send_string(&lcd, buffer);

	while(1){
		distance = hcsr04_get_precise_distance();
		snprintf(buffer, sk_arr_len(buffer), "dis=%.2f   ", distance);
		lcd_set_cursor(&lcd, 1, 0);
		lcd_send_string(&lcd, buffer);
	}

	return 0;
}
*/