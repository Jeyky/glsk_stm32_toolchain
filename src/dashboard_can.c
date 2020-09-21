#include "lcd_hd44780.h"
#include "timer_delay.h"
#include "printf.h"
#include "pwm.h"
#include "libopencm3/cm3/cortex.h"
#include "libopencm3/cm3/nvic.h"
#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/flash.h"
#include "libopencm3/stm32/pwr.h"
#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/can.h"
#include "stdint.h"
#include "stddef.h"


static struct sk_lcd lcd = {
	.pin_group_data = &sk_io_lcd_data,
	.pin_rs = &sk_io_lcd_rs,
	.pin_en = &sk_io_lcd_en,
	.pin_rw = &sk_io_lcd_rw,
	.pin_bkl = &sk_io_lcd_bkl,
	.set_backlight_func = NULL,
	.delay_func_us = NULL,
	.delay_func_ms = &delay_ms,
	.is4bitinterface = true,
	//.charmap_func = &sk_lcd_charmap_rus_cp1251
};

const sk_pin can_tx = {.port = SK_PORTD, .pin = 1, .isinverse = false};
const sk_pin can_rx = {.port = SK_PORTD, .pin = 0, .isinverse = false};

void dashboard_can_init(void)
{
	rcc_periph_clock_enable(RCC_GPIOD);
	
	sk_pin_set_output_options(can_tx, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ);
	sk_pin_set_output_options(can_rx, GPIO_OTYPE_OD, GPIO_OSPEED_50MHZ);

	sk_pin_set_af(can_tx, GPIO_AF9);
	sk_pin_set_af(can_rx, GPIO_AF9);

	sk_pin_mode_setup(can_tx, GPIO_MODE_AF, GPIO_PUPD_NONE);
	sk_pin_mode_setup(can_rx, GPIO_MODE_AF, GPIO_PUPD_NONE);

	rcc_periph_clock_enable(RCC_CAN1);

	can_reset(CAN1);

	if (can_init (CAN1, false, false, false, false, false, false, CAN_BTR_SJW_2TQ, CAN_BTR_TS1_12TQ, CAN_BTR_TS2_2TQ, 28, false, false))
		sk_pin_set(sk_io_led_red, 1);
	can_filter_id_mask_32bit_init(0, 0, 0, 0, true);
	//can_enable_irq(CAN1, 3);




}



int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOE);

	glsk_pins_init(false);

	clock_hse_168MHZ_init();

	timer_delay_init(84 - 1);

	sk_lcd_set_backlight(&lcd, 0xFF);
	
	for (int i = 0; i < 5; i++)
		lcd_init_4bit(&lcd);

	lcd_set_cursor(&lcd, 1, 0);
	lcd_send_string(&lcd, "CAN EXAMPLE");

	

	pwm_init();
	speedometer_init();

	speedometer_set_speed(100);

	//delay_ms(2000);


	int32_t res=100;				//    //
	uint8_t stmp[8] = {0x05, 0x62, 0xFF, 11, 0x65, 0x12, 0, 62};
	uint8_t stmp2[8] = {0x08, 0x00, 0x00, 0x00, 0xAB, 0x00, 0x00, 0x00};
	uint8_t stmp3[8] = {0x00, 0xAB, 0x8c, 0x08, 0x00, 0xFE, 0x00, 0x00};
	uint8_t stmp4[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	dashboard_can_init();
	res = can_transmit(CAN1, 0x316, false, false, 8, &stmp);

	char buffer[20];
	lcd_set_cursor(&lcd, 0, 0);
	snprintf(buffer,sk_arr_len(buffer), "res=%d  ", res);
	lcd_send_string(&lcd, buffer);

	//delay_ms(200);

	res = can_transmit(CAN1, 0x545, false, false, 8, &stmp2);

	
	lcd_set_cursor(&lcd, 0, 0);
	snprintf(buffer,sk_arr_len(buffer), "res=%d   ", res);
	lcd_send_string(&lcd, buffer);
	//can_available_mailbox(CAN1);
	//uint8_t tt;
	//can_fifo_release(CAN1, tt);

	delay_ms(200);

	res = can_transmit(CAN1, 0x329, false, false, 8, &stmp3);


	lcd_set_cursor(&lcd, 0, 0);
	snprintf(buffer,sk_arr_len(buffer), "res=%d   ", res);
	lcd_send_string(&lcd, buffer);

	delay_ms(200);

	//res = can_transmit(CAN1, 0x153, false, false, 8, &stmp4);

	lcd_set_cursor(&lcd, 0, 0);
	snprintf(buffer,sk_arr_len(buffer), "res=%d   ", res);
	lcd_send_string(&lcd, buffer);
	while(1) {

	}
}