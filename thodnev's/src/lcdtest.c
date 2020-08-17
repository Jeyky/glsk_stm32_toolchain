#include "lcd_hd44780.h"
#include "tick.h"
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

static void test_bkl_func(uint8_t val)
{
	sk_pin_set(sk_io_lcd_bkl, (bool)val);
}


extern void lcd_init_4bit(struct sk_lcd *lcd);


int main(void)
{
    rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOE);		// lcd is connected to port E
	glsk_pins_init(true);
	sk_pin_group_set(sk_io_lcd_data, 0x00);
	sk_pin_set(sk_io_led_orange, true);

	sk_tick_init(16000000ul / 10000ul, 2);
	cm_enable_interrupts();

	struct sk_lcd lcd = {
		.pin_group_data = &sk_io_lcd_data,
		.pin_rs = &sk_io_lcd_rs,
		.pin_en = &sk_io_lcd_en,
		.pin_rw = &sk_io_lcd_rw,
		.pin_bkl = &sk_io_lcd_bkl,
		//.set_backlight_func = &test_bkl_func,
		.delay_func_us = NULL,
		.delay_func_ms = &sk_tick_delay_ms,
		.is4bitinterface = true
	};

	sk_lcd_set_backlight(&lcd, 0xFF);
	

    while (1) {
		// dumb code for logic analyzer to test levels
		sk_pin_set(sk_io_led_orange, false);
		lcd_init_4bit(&lcd);
		sk_pin_set(sk_io_led_orange, true);
		sk_tick_delay_ms(500);
    }
}
