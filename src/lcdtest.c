#include "lcd_hd44780.h"
//#include "tick.h"
//#include "abs_systick.h"
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/cm3/scb.h>
#include <libopencm3/stm32/exti.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "timer_delay.h"
#include "clkset.h"
#include "stdio.h"
#include <stddef.h>
#include "printf.h"


const sk_pin trig 	= {.port = SK_PORTA, .pin = 3, .isinverse = false};
const sk_pin echo = {.port = SK_PORTA, .pin = 0, .isinverse = false};
const sk_pin out = {.port = SK_PORTA, .pin = 1, .isinverse = false};

const sk_pin led_blue 	= {.port = SK_PORTD, .pin = 15, .isinverse = false};

static bool __rising_falling = false;
uint32_t counter = 0;
uint32_t countt = 0;

void sk_inter_exti_init(sk_pin pin, enum exti_trigger_type trigger)
{
	exti_select_source(EXTI0, sk_pin_port_to_gpio(pin.port));
	exti_set_trigger(EXTI0, trigger);
	exti_enable_request(EXTI0);
	exti_reset_request(EXTI0);
}
void test_trig_echo(void)
{
	sk_pin_set(trig, 1);
	delay_ms(800);
	sk_pin_set(trig, 0);
}

void exti0_isr(void) 
{
	/*
	sk_pin_toggle(led_blue);
	__rising_falling = !__rising_falling;
	if (__rising_falling) {
		timer_set_period(TIM5, 0xFFFFFFFF - 1);
		timer_generate_event(TIM5, TIM_EGR_UG);
		__dmb();	
		timer_enable_counter(TIM5);
	} else {
		countt = TIM_CNT(TIM5);
	}
	exti_reset_request(EXTI0);*/
	softdelay(200);
	if (sk_pin_read(echo)) {
		sk_pin_set(led_blue, 1);
		timer_set_period(TIM5, 0xFFFFFFFF - 1);
		timer_generate_event(TIM5, TIM_EGR_UG);
		__dmb();	
		timer_enable_counter(TIM5);
	} else {
		sk_pin_set(led_blue, 0);
		countt = TIM_CNT(TIM5);
	}
	exti_reset_request(EXTI0);
}

void delay_uus(uint32_t us)
{
	if(!us)
		return;
	timer_set_period(TIM2, us);

	timer_generate_event(TIM2, TIM_EGR_UG);

	__dmb();	
	timer_enable_counter(TIM2);
	softdelay(10000);
	counter = timer_get_counter(TIM2_CNT);
	countt = TIM_CNT(TIM2);
	__asm__ volatile ("wfi");
}
void delay_uss(uint32_t us)
{
	if(!us)
		return;
	timer_set_period(TIM5, us);

	timer_generate_event(TIM5, TIM_EGR_UG);

	__dmb();	
	timer_enable_counter(TIM5);
	//softdelay(10000);
	delay_us(8);
	//counter = timer_get_counter(TIM5_CNT);
	//countt = TIM_CNT(TIM5);
	counter = timer_get_counter(TIM5_CNT);
	countt = TIM_CNT(TIM5);
	__asm__ volatile ("wfi");
	
}

void meas(void)
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
	//return countt;

}


static void test_bkl_func(uint8_t val)
{
	sk_pin_set(sk_io_lcd_bkl, (bool)val);
}


extern void lcd_init_4bit(struct sk_lcd *lcd);
//extern void lcd_rsrw_set(struct sk_lcd *lcd, bool rs, bool rw);
//extern void lcd_data_set_halfbyte(struct sk_lcd *lcd, uint8_t half);
//extern void lcd_data_set_byte(struct sk_lcd *lcd, uint8_t byte);
//extern void lcd_send_byte(struct sk_lcd *lcd, bool rs, uint8_t byte);

int main(void)
{
    rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOE);		// lcd is connected to port E
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOC);

	glsk_pins_init(false);
	sk_pin_group_set(sk_io_lcd_data, 0x00);
	sk_pin_set(sk_io_led_orange, true);


	clock_hse_init(4, 168, 2, 7, 0);
	timer_delay_init(84 - 1);
	timer_init(84 - 1);

	sk_pin_mode_setup(echo, GPIO_MODE_INPUT, GPIO_PUPD_NONE);

	sk_pin_mode_setup(out, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
	sk_pin_mode_setup(trig, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);

	sk_pin_mode_setup(led_blue, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);

	scb_set_priority_grouping(SCB_AIRCR_PRIGROUP_GROUP4_SUB4);

	const uint8_t group = 2;
	const uint8_t subgroup = 0;

	nvic_set_priority(NVIC_EXTI0_IRQ, (group << 2) | subgroup);

	rcc_periph_clock_enable(RCC_SYSCFG);

	sk_inter_exti_init(echo, EXTI_TRIGGER_BOTH);
	nvic_enable_irq(NVIC_EXTI0_IRQ);

	//abs_sys_tick_init(16000000ul / 10000ul, 2);
	//sk_tick_init(16000000ul / 10000ul, 2);
	cm_enable_interrupts();

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

	char tmp[20], buffer[20];
	//snprintf(tmp, sk_arr_len(tmp), "%5.1f", temp);
	//snprintf(buffer, sk_arr_len(buffer), "T=%-5s°C A=%u", tmp, (unsigned int)adcval);

	//delay_uss(1000);
	meas();

	double distance = (countt * 0.0343) / 2;
	//snprintf(buffer, sk_arr_len(buffer), "cr=%u", (uint32_t)counter);
	//lcd_send_string(&lcd, buffer);
	snprintf(buffer, sk_arr_len(buffer), "ct=%d", (uint32_t)countt);
	lcd_set_cursor(&lcd, 0, 0);
	lcd_send_string(&lcd, buffer);
	snprintf(buffer, sk_arr_len(buffer), "ct=%.2f", (double)distance);
	lcd_set_cursor(&lcd, 1, 0);
	lcd_send_string(&lcd, buffer);
	//lcd_send_string(&lcd, "slava!");
	//lcd_send_cmd(&lcd, 0xc0);
	//lcd_send_string(&lcd, "ukraine");
	//lcd_set_cursor(&lcd, 1, 0);

    while (1) {
		// dumb code for logic analyzer to test levels
		sk_pin_set(sk_io_led_orange, false);
		
		
		delay_ms(10);
		//sk_pin_toggle(led_blue);
		meas();

		double distance = (countt * 0.0343) / 2;
		__dmb();
		snprintf(buffer, sk_arr_len(buffer), "ct=%d", (uint32_t)countt);
		lcd_set_cursor(&lcd, 0, 0);
		lcd_send_string(&lcd, buffer);
		snprintf(buffer, sk_arr_len(buffer), "ct=%.2f", (double)distance);
		lcd_set_cursor(&lcd, 1, 0);
		lcd_send_string(&lcd, buffer);

		sk_pin_set(sk_io_led_orange, true);
		//lcd_send_byte(&lcd, true, 0b11111111);
		delay_ms(10);
    		
		
		//lcd_send_byte(&lcd, true, 0b10110100);

    }
}
