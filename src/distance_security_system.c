#include <libopencm3/cm3/cortex.h>
#include <libopencm3/stm32/rcc.h>
#include "lcd_hd44780.h"
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
#include "adc_temp.h"
#include "buttons.h"
#include "menu.h"

static inline void alarm_check(double distance)
{
	(distance <= threshold ) ? alarm_on() : alarm_off();
}

void rotate_and_measure_and_display(struct sk_lcd *lcd)
{
	double distance;

	const uint8_t step = 5;

	char buffer[20];

	for(int i = 30; i <= 150;i += step) {

		distance = hcsr04_get_distance();

		alarm_check(distance);

		if(dashboard_status) {

			speedometer_set_speed((uint16_t) distance);
		} else {

			speedometer_set_speed(0);
		}

		snprintf(buffer, sk_arr_len(buffer), "distance=%.1f  ", (double)distance);
		lcd_set_cursor(lcd, 1, 0);
		lcd_send_string(lcd, buffer);

		servo_rotate(i);

		if (current_menu == rotation_menu) {
			lcd_rotation_menu_handler(lcd);

		} else if (current_menu == dashboard_menu) {
			lcd_dashboard_menu_handler(lcd);
		}

		if(current_menu != next_menu){
			current_menu = next_menu;
			lcd_print_menu(lcd, current_menu);
			return;
		}

		if (!rotation_status){
			servo_rotate(90);
			return;
		} 
	}



	for(int i = 150; i >= 30;i -= step) {

		distance = hcsr04_get_distance();

		alarm_check(distance);

		if(dashboard_status) {

			speedometer_set_speed((uint16_t) distance);

		} else {

			speedometer_set_speed(0);

		}


		snprintf(buffer, sk_arr_len(buffer), "distance=%.1f  ", (double)distance);
		lcd_set_cursor(lcd, 1, 0);
		lcd_send_string(lcd, buffer);

		servo_rotate(i);

		if (current_menu == rotation_menu) {
			lcd_rotation_menu_handler(lcd);
		} else if (current_menu == dashboard_menu) {
			lcd_dashboard_menu_handler(lcd);
		}

		if(current_menu != next_menu){
			current_menu = next_menu;
			lcd_print_menu(lcd, current_menu);
			return;
		}

		if (!rotation_status){
			servo_rotate(90);
			return;
		} 
	}

	
}

void rotate_and_measure(struct sk_lcd *lcd)
{
	double distance;

	const uint8_t step = 5;


	for(int i = 30; i <= 150;i += step) {

		distance = hcsr04_get_distance();

		alarm_check(distance);

		if(dashboard_status) {

			speedometer_set_speed((uint16_t) distance);
		} else {

			speedometer_set_speed(0);
		}

		servo_rotate(i);

		/*if(btn_middle_status){
			rotation_status = !rotation_status;
			btn_middle_status = false;
		}*/
		if (current_menu == lock_menu) {
			lcd_lock_menu_handler(lcd);
		} else if (current_menu == dialog_menu) {
			lcd_dialog_menu_handler(lcd);
		}

		if(current_menu != next_menu){
			current_menu = next_menu;
			lcd_print_menu(lcd, current_menu);
			return;

		}

		if (!rotation_status){
			servo_rotate(90);
			return;
		} 
		
	}

	for(int i = 150; i >= 30;i -= step) {

		distance = hcsr04_get_distance();

		alarm_check(distance);

		if(dashboard_status) {

			speedometer_set_speed((uint16_t) distance);

		} else {

			speedometer_set_speed(0);

		}

		servo_rotate(i);

		/*if(btn_middle_status){
			rotation_status = !rotation_status;
			btn_middle_status = false;
		}*/
		if (current_menu == lock_menu) {
			lcd_lock_menu_handler(lcd);
		} else if (current_menu == dialog_menu) {
			lcd_dialog_menu_handler(lcd);
		}

		if(current_menu != next_menu){
			current_menu = next_menu;
			lcd_print_menu(lcd, current_menu);
			return;
	
		}

		if (!rotation_status){
			servo_rotate(90);
			return;
		} 
	}

	
}

void measure_and_display(struct sk_lcd *lcd)
{
	double distance;

	char buffer[20];

	servo_rotate(90);
	distance = hcsr04_get_distance();

	alarm_check(distance);

	snprintf(buffer, sk_arr_len(buffer), "distance=%.1f  ", (double) distance);
	lcd_set_cursor(lcd, 1, 0);
	lcd_send_string(lcd, buffer);

	if(dashboard_status) {
		speedometer_set_speed((uint16_t) distance);
	} else {
		speedometer_set_speed(0);
	}

	
}

void measure(void)
{
	double distance;

	char buffer[20];

	servo_rotate(90);
	distance = hcsr04_get_distance();

	alarm_check(distance);

	if(dashboard_status) {
		speedometer_set_speed((uint16_t) distance);
	} else {
		speedometer_set_speed(0);
	}

	
}

double get_speed_of_sound(void)
{

	//uint8_t temprature = dht11_read_temprature();
	uint8_t temprature;

	dht11_start();
	//temprature = dht11_read_temprature(); 

	if (dht11_response()) {

		dht11_read_byte();
		dht11_read_byte();
		temprature = dht11_read_byte();
		dht11_read_byte();
		dht11_read_byte();
	} else {
		temprature = adc_get_temp();
	}

	
	speed_of_sound = 331.3 + (0.59 * temprature);
	return speed_of_sound;
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
		.delay_func_us = &delay_us,
		//.delay_func_ms = &abs_delay_ms,
		.delay_func_ms = &delay_ms,
		.is4bitinterface = true
	};

	sk_lcd_set_backlight(&lcd, 0xFF);

	for(int i =0; i < 5; i++)
		lcd_init_4bit(&lcd);

	adc_temp_init();

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

	
	sk_pin_set(sk_io_led_orange, false);
	sk_pin_set(sk_io_led_blue, true);

	lcd_set_cursor(&lcd, 0, 0);

	lcd_send_string(&lcd, "pnl:    thd:");

	char buffer[20];

	float temp = adc_get_temp();

	/*snprintf(buffer, sk_arr_len(buffer), "temp=%.2f", (float)temp);
	lcd_set_cursor(&lcd, 1, 0);
	lcd_send_string(&lcd, buffer);
	delay_ms(2000);*/

	sound_speed = get_speed_of_sound();

	snprintf(buffer, sk_arr_len(buffer), "speed=%.2f", (double)speed_of_sound);
	lcd_set_cursor(&lcd, 1, 0);
	lcd_send_string(&lcd, buffer);

	double distance = hcsr04_get_distance();

	double current, prev;

	prev = hcsr04_get_distance();
	uint32_t tmp;
	
	delay_ms(2000);

	lcd_print_menu(&lcd, password_menu);

	while(1){
		
		switch(current_menu) {
			case password_menu:

				lcd_password_menu_handler(&lcd);
				break;

			case info_menu:

				lcd_info_menu_handler(&lcd);
				break;

			case dashboard_menu:

				lcd_dashboard_menu_handler(&lcd);

				if(rotation_status)
					rotate_and_measure_and_display(&lcd);
				else 
					measure_and_display(&lcd);

				break;

			case rotation_menu:

				lcd_rotation_menu_handler(&lcd);

				if(rotation_status)
					rotate_and_measure_and_display(&lcd);
				else 
					measure_and_display(&lcd);
				break; 

			case dialog_menu:

				lcd_dialog_menu_handler(&lcd);

				if(rotation_status){
					rotate_and_measure(&lcd);
				}
				else 
					measure();
				break; 

			case lock_menu:
				lcd_lock_menu_handler(&lcd);

				if(rotation_status)
					rotate_and_measure(&lcd);
				else 
					measure();
				break; 
		}
		if(current_menu != next_menu){
			current_menu = next_menu;
			lcd_print_menu(&lcd, current_menu);
		}
		
	}

	return 0;
}