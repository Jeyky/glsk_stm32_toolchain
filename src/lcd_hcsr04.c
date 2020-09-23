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


const sk_pin trig 	= {.port = SK_PORTA, .pin = 3, .isinverse = false};
const sk_pin echo = {.port = SK_PORTC, .pin = 0, .isinverse = false};

const sk_pin led_blue 	= {.port = SK_PORTD, .pin = 15, .isinverse = false};

bool show_on_speedometer = false;
double speed_of_sound = 331.3;
uint32_t counter = 0;


//void get_precise_distance(struct sk_lcd *lcd)
double hcsr04_get_precise_distance(void)
{
	double distance[3] = {0.0};
	double min_val;
	//char buffer[20];
	//lcd_set_cursor(lcd, 0, 0);
	meas();
	distance[0] = (counter * speed_of_sound / 10000) / 2;
	min_val = distance[0];
	//snprintf(buffer, sk_arr_len(buffer), "d=%.1f", (double)distance[i]);
	//lcd_send_string(lcd, buffer);
	for(int i =1; i < 3; i++) {
		meas();
		distance[i] = (counter * speed_of_sound / 10000) / 2;
		if(min_val > distance[i])
			min_val = distance[i];
		//snprintf(buffer, sk_arr_len(buffer), "d=%.1f", (double)distance[i]);
		//lcd_send_string(lcd, buffer);
	}
	return min_val;
}

double hcsr04_get_distance(void)
{
	meas();
	double distance = (counter * speed_of_sound / 10000) / 2;

	return distance;
}

void rotate_and_measure(struct sk_lcd *lcd)
{
	double distance;
	char buffer[20];
	for(int i = 0; i <= 180; i+= 5) {
		distance = hcsr04_get_distance();
		/*if(show_on_speedometer) {
			speedometer_set_speed((uint16_t) distance);
		}*/
		snprintf(buffer, sk_arr_len(buffer), "dist=%.2f     ", (double)distance);
		lcd_set_cursor(lcd, 1, 0);
		lcd_send_string(lcd, buffer);
		servo_rotate(i);
	}
	for(int i = 180; i >= 0;i-=5) {
		distance = hcsr04_get_distance();
		if(show_on_speedometer) {
			speedometer_set_speed((uint16_t) distance);
		}
		snprintf(buffer, sk_arr_len(buffer), "dist=%.2f     ", (double)distance);
		lcd_set_cursor(lcd, 1, 0);
		lcd_send_string(lcd, buffer);
		servo_rotate(i);
	}
}

void sk_inter_exti_init(sk_pin pin, enum exti_trigger_type trigger)
{
	exti_select_source((1 << pin.pin), sk_pin_port_to_gpio(pin.port));
	exti_set_trigger((1 << pin.pin), trigger);
	exti_enable_request((1 << pin.pin));
	exti_reset_request((1 << pin.pin));
}


void install_speed_of_sound(void)
{
	uint8_t tempraure = dht11_read_temprature();
	speed_of_sound = 331.3 + (0.59 * tempraure);
}

void exti0_isr(void) 
{
	//softdelay(200);
	if (sk_pin_read(echo)) {
		sk_pin_set(led_blue, 1);
		timer_set_period(TIM2, 0xFFFFFFFF - 1);
		timer_generate_event(TIM2, TIM_EGR_UG);
		__dmb();	
		timer_enable_counter(TIM2);
	} else {
		sk_pin_set(led_blue, 0);
		counter = TIM_CNT(TIM2);
	}
	exti_reset_request(EXTI0);
}

void meas(void)
{
	delay_ms(60);

	sk_pin_set(trig, 1);

	delay_us(10);

	sk_pin_set(trig, 0);

	delay_us(1000);
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


	//clock_hse_init(4, 168, 2, 7, 0);
	clock_hse_168MHZ_init();
	timer_delay_init(84 - 1);
	timer_init(84 - 1);
	pwm_init();
	servo_init();
	speedometer_init();


	sk_pin_mode_setup(echo, GPIO_MODE_INPUT, GPIO_PUPD_NONE);

	//sk_pin_mode_setup(out, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);
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

	char buffer[20];

	//delay_uss(1000);
	meas();

	meas();

	uint8_t tempraure = dht11_read_temprature();

	double distance = (counter * speed_of_sound / 10000) / 2;

	snprintf(buffer, sk_arr_len(buffer), "temp=%d", (uint8_t)tempraure);
	lcd_set_cursor(&lcd, 0, 0);
	lcd_send_string(&lcd, buffer);
	delay_ms(1500);
	snprintf(buffer, sk_arr_len(buffer), "dist=%.2f     ", (double)distance);
	lcd_set_cursor(&lcd, 1, 0);
	lcd_send_string(&lcd, buffer);

	sk_pin_set(sk_io_led_red, 1);

	//delay_ms(3000);
	rotate_and_measure(&lcd);
	/*servo_rotate(90);
	sk_pin_toggle(sk_io_led_red);
	delay_ms(800);
	servo_rotate(0);
	sk_pin_toggle(sk_io_led_red);
	delay_ms(800);
	servo_rotate(90);
	sk_pin_toggle(sk_io_led_red);
	delay_ms(800);
	servo_rotate(180);
	sk_pin_toggle(sk_io_led_red);
	delay_ms(800);*/

	/*for(int i = 0; i <= 180; i += 5) {
		servo_rotate(i);
		delay_ms(80);
	}
	for(int i = 180; i >= 0; i -= 5) {
		servo_rotate(i);
		delay_ms(80);
	}*/
	sk_pin_set(sk_io_led_red, 1);

	//get_precise_distance(&lcd); 

    while (1) {

    	sk_pin_set(sk_io_led_orange, false);
    	//distance = get_precise_distance();
    	distance = hcsr04_get_distance();
    	snprintf(buffer, sk_arr_len(buffer), "dis=%.2f    ", (double)distance);
    	if((distance < 4.0) && distance > 1.0) sk_pin_toggle(sk_io_led_green);
		lcd_set_cursor(&lcd, 1, 0);
		lcd_send_string(&lcd, buffer);
		//rotate_and_measure(&lcd);
		sk_pin_set(sk_io_led_orange, true);

    }
}
