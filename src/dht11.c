#include "lcd_hd44780.h"
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

const sk_pin dht11 	= {.port = SK_PORTD, .pin = 11, .isinverse = false};

void dht11_start(void)
{
	rcc_periph_clock_enable(RCC_GPIOD);
	sk_pin_mode_setup(dht11, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
	sk_pin_set(dht11, 0);
	delay_ms(18);
	sk_pin_set(dht11, 1);
	delay_us(20);
	sk_pin_mode_setup(dht11, GPIO_MODE_INPUT, GPIO_PUPD_NONE);
}
bool dht11_response(void)
{
	bool response = false;
	delay_us(40);
	if (!sk_pin_read(dht11)) {
		delay_us(80);
	}
	if (sk_pin_read(dht11)) {
		response = true;
		while(sk_pin_read(dht11));
	} 

	return response;
}

uint8_t dht11_read_byte(void) 
{
	uint8_t byte = 0;
	for (int i; i < 8; i++) {
		while(!(sk_pin_read(dht11)));
		delay_us(40);
		if (sk_pin_read(dht11)) 
			byte |= (1 << (7 - i));
		 else 
			byte &= ~(1 << (7 - i));
		while(sk_pin_read(dht11));
	}
	return byte;
}

uint8_t dht11_read_temprature(void) 
{
	uint8_t temp;

	dht11_start();

	dht11_response();

	dht11_read_byte();
	dht11_read_byte();
	temp = dht11_read_byte();
	dht11_read_byte();
	dht11_read_byte();

	return temp;
}

uint8_t dht11_read_humidity(void) 
{
	uint8_t humidity;

	dht11_start();

	dht11_response();

	humidity = dht11_read_byte();
	dht11_read_byte();
	dht11_read_byte();
	dht11_read_byte();
	dht11_read_byte();

	return humidity;
}

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
	//dht11_start();
	//if(dht11_response())
	//	sk_pin_set(sk_io_led_red, 1);
	uint8_t rh_integr, rh, temp_integ, temp, check_sum;
	/*rh_integr = dht11_read_byte();
	rh = dht11_read_byte();
	temp_integ = dht11_read_byte();
	temp = dht11_read_byte();
	check_sum = dht11_read_byte();*/

	char temp_buffer[20];
	char hum_buffer[20];

	
	/*sk_pin_toggle(sk_io_led_blue);
	rh_integr = dht11_read_byte();
	dht11_read_byte();
	temp_integ = dht11_read_byte();
	dht11_read_byte();
	dht11_read_byte();

	snprintf(temp_buffer, sk_arr_len(temp_buffer), "temp=%u", (uint8_t)temp_integ);
	lcd_set_cursor(&lcd, 0, 0);
	lcd_send_string(&lcd, temp_buffer);

	snprintf(hum_buffer, sk_arr_len(hum_buffer), "t=%u", (uint8_t)rh_integr);
	lcd_set_cursor(&lcd, 1, 0);
	lcd_send_string(&lcd, hum_buffer);	*/

	temp_integ = dht11_read_hum();
	snprintf(temp_buffer, sk_arr_len(temp_buffer), "hum=%u", temp_integ);
	lcd_set_cursor(&lcd, 0, 0);
	lcd_send_string(&lcd, temp_buffer);
	
/*
	snprintf(buffer, sk_arr_len(buffer), "hum=%u", (uint8_t)rh_integr);
	lcd_set_cursor(&lcd, 0, 0);
	lcd_send_string(&lcd, buffer);
	snprintf(buffer, sk_arr_len(buffer), "t=%u", (uint8_t)temp_integ);
	lcd_set_cursor(&lcd, 1, 0);
	lcd_send_string(&lcd, buffer);
*/
	while(1) {
		delay_ms(1500);
		sk_pin_toggle(sk_io_led_orange);
		temp_integ = dht11_read_hum();
		snprintf(temp_buffer, sk_arr_len(temp_buffer), "hum=%u", temp_integ);
		lcd_set_cursor(&lcd, 0, 0);
		lcd_send_string(&lcd, temp_buffer);

	}
}