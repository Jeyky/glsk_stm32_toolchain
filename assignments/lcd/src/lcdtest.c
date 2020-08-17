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
//extern void lcd_rsrw_set(struct sk_lcd *lcd, bool rs, bool rw);
//extern void lcd_data_set_halfbyte(struct sk_lcd *lcd, uint8_t half);
//extern void lcd_data_set_byte(struct sk_lcd *lcd, uint8_t byte);
//extern void lcd_send_byte(struct sk_lcd *lcd, bool rs, uint8_t byte);

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
	
	lcd_init_4bit(&lcd);
	//lcd_send_byte(&lcd, false, 0b11111111);
	//lcd_rsrw_set(&lcd, true, false);
	//lcd_data_set_halfbyte(&lcd, '0' >> 4);
	//lcd_data_set_halfbyte(&lcd, '0' & 0x0F);
	lcd_send_string(&lcd, "slava ukraine heroyam slava");
	//0b11000000
	/*lcd_rsrw_set(&lcd, false, false);
	lcd_data_set_halfbyte(&lcd, 0b11000000 >> 4);
	lcd_data_set_halfbyte(&lcd,0b11000000 & 0x0F);*/
	//lcd_send_string(&lcd, "heroyam slava");
    while (1) {
		// dumb code for logic analyzer to test levels
		sk_pin_set(sk_io_led_orange, false);
		
		
		sk_tick_delay_ms(500);
		sk_pin_set(sk_io_led_orange, true);
		//lcd_send_byte(&lcd, true, 0b11111111);
		sk_tick_delay_ms(500);
		//lcd_send_byte(&lcd, true, 0b10110100);

    }
}
