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
#include "dht11.h"
#include "pwm.h"
#include "hcsr04.h"


//bool show_on_speedometer = true;

struct menu_page {
	char row1[20];
	char row2[20];
};

void rotate_and_measure(struct sk_lcd *lcd)
{
	double distance;
	const uint8_t step = 5;
	char buffer[20];
	for(int i = 30; i <= 150;i += step) {
		distance = hcsr04_get_distance();
		(distance < 20 && distance >3) ? alarm_on() : alarm_off();
		if(show_on_speedometer) {
			lcd_set_cursor(lcd, 0, 12);
			lcd_send_string(lcd, "on ");
			speedometer_set_speed((uint16_t) distance);
		} else {
			lcd_set_cursor(lcd, 0, 12);
			lcd_send_string(lcd, "off");
			speedometer_set_speed(0);
		}
		snprintf(buffer, sk_arr_len(buffer), "dist=%.2f     ", (double)distance);
		lcd_set_cursor(lcd, 1, 0);
		lcd_send_string(lcd, buffer);
		servo_rotate(i);
	}

	for(int i = 150; i >= 30;i -= step) {
		distance = hcsr04_get_distance();
		(distance < 20 && distance >3) ? alarm_on() : alarm_off();
		if(show_on_speedometer) {
			lcd_set_cursor(lcd, 0, 12);
			lcd_send_string(lcd, "on ");
			speedometer_set_speed((uint16_t) distance);
		} else {
			lcd_set_cursor(lcd, 0, 12);
			lcd_send_string(lcd, "off");
			speedometer_set_speed(0);
		}
		snprintf(buffer, sk_arr_len(buffer), "dist=%.2f     ", (double)distance);
		lcd_set_cursor(lcd, 1, 0);
		lcd_send_string(lcd, buffer);
		servo_rotate(i);
	}
}

void set_speed_of_sound(void)
{
	uint8_t tempraure = dht11_read_temprature();
	speed_of_sound = 331.3 + (0.59 * tempraure);
}

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

	hcsr04_inint();
	pwm_init();
	servo_init();
	speedometer_init();
	alarm_init();
	button_init();
	servo_rotate(90);

	sk_pin_set(sk_io_led_red, true);
	speedometer_set_speed(60);
	sk_pin_set(sk_io_led_red, false);

	delay_ms(2000);
	sk_pin_set(sk_io_led_orange, false);
	sk_pin_set(sk_io_led_blue, true);

	lcd_set_cursor(&lcd, 0, 0);
	lcd_send_string(&lcd, "panel scale:");

	char buffer[20];

	double distance = hcsr04_get_distance();
	set_speed_of_sound();
	snprintf(buffer, sk_arr_len(buffer), "speed=%.2f", (double)speed_of_sound);
	lcd_set_cursor(&lcd, 1, 0);
	lcd_send_string(&lcd, buffer);
	delay_ms(100);
	while(1){
		
		/*distance = hcsr04_get_precise_distance();
		speedometer_set_speed((uint16_t) distance);
		snprintf(buffer, sk_arr_len(buffer), "dist=%.2f   ", distance);
		lcd_set_cursor(&lcd, 1, 0);
		lcd_send_string(&lcd, buffer);*/
		rotate_and_measure(&lcd);
	}

	return 0;
}