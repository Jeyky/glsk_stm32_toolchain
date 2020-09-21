#include "lcd_hd44780.h"
#include "timer_delay.h"
#include "printf.h"
#include "libopencm3/cm3/cortex.h"
#include "libopencm3/cm3/nvic.h"
#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/flash.h"
#include "libopencm3/stm32/pwr.h"
#include "libopencm3/stm32/rcc.h"
#include "libopencm3/stm32/i2c.h"
#include "stdint.h"
#include "stddef.h"

const sk_pin eeprom_sda = {.port = SK_PORTB, .pin = 9, .isinverse = false};
const sk_pin eeprom_scl = {.port = SK_PORTB, .pin = 6, .isinverse = false};

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

void eeprom_init(uint32_t i2c)
{
	rcc_periph_clock_enable(RCC_GPIOB);

	sk_pin_set_output_options(eeprom_sda, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ);
	sk_pin_set_output_options(eeprom_scl, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ);

	sk_pin_set_af(eeprom_scl, GPIO_AF4);
	sk_pin_set_af(eeprom_sda, GPIO_AF4);

	sk_pin_mode_setup(eeprom_scl, GPIO_MODE_AF, GPIO_PUPD_NONE); 
	sk_pin_mode_setup(eeprom_sda, GPIO_MODE_AF, GPIO_PUPD_NONE);

	rcc_periph_clock_enable((i2c == I2C1) ? RCC_I2C1 : RCC_I2C2);

	i2c_peripheral_disable(i2c);

	i2c_set_clock_frequency(i2c, rcc_apb1_frequency / 1000000);

	i2c_set_fast_mode(i2c);

	i2c_set_dutycycle(i2c, I2C_CCR_DUTY_DIV2);

	i2c_set_ccr(i2c, (rcc_apb1_frequency/(400000ul * 3ul)));

	i2c_set_trise(i2c, (1ul + (rcc_apb1_frequency / 1000000ul)));

	/*i2c_set_clock_frequency(i2c, rcc_apb1_frequency / 1000000);
	
	i2c_set_standard_mode(i2c);		// set communication frequency to Sm (100 kHz)
	//i2c_set_dutycycle(i2c, I2C_CCR_DUTY_DIV2);		// Tlow / Thigh = 2 (relates only to Fm)

	// CCR = F_PCLK1 / (2 * F_i2c) = 42 MHz / (2 * 100 kHz) = 42e6 / (2 * 100e3) = 210
	i2c_set_ccr(i2c, (rcc_apb1_frequency / (2ul * 100000ul)));	// 100 kHz communication speed

	// Trise = 1 + Tmax / T_PCLK1 = 1 + F_PCLK1 * Tmax, where Tmax is given is I2C specs
	// for 100 kHz, Tmaz = 1000 ns = 1000e-9 s. => Trise = 1 + F_PCLK / 1e6
	i2c_set_trise(i2c, (1ul + rcc_apb1_frequency/1000000ul));*/

	//i2c_set_speed(I2C1, i2c_speed_fm_400k, rcc_apb1_frequency/1e6);

	i2c_peripheral_enable(i2c);

}

void eeprom_single_write(uint32_t i2c, uint8_t HW_address, uint8_t address, uint8_t data)
{
	i2c_send_start(i2c);
	//i2c_enable_ack(i2c);
	while (!((I2C_SR1(i2c) & I2C_SR1_SB) & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

	i2c_send_7bit_address(i2c, HW_address, 0);
	while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));

	//acknowlegment

	i2c_send_data(i2c, address);
	while (!(I2C_SR1(i2c) & I2C_SR1_BTF)); 

	i2c_send_data(i2c, data);
	while (!(I2C_SR1(i2c) & I2C_SR1_BTF)); 

	i2c_send_stop(i2c);
	delay_ms(3);
}

void i2c_write7(uint32_t i2c, int addr, uint8_t *data, size_t n)
 {
         while ((I2C_SR2(i2c) & I2C_SR2_BUSY)) {
         }
  
         i2c_send_start(i2c);
  
         
         while (!((I2C_SR1(i2c) & I2C_SR1_SB)
                 & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));
  
         i2c_send_7bit_address(i2c, addr, I2C_WRITE);
  
        
         while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));
  
         (void)I2C_SR2(i2c);
  
         for (size_t i = 0; i < n; i++) {
                 i2c_send_data(i2c, data[i]);
                 while (!(I2C_SR1(i2c) & (I2C_SR1_BTF)));
         }
 }

void eeporm_write_byte(uint32_t i2c, uint8_t HW_address, uint8_t address, uint8_t data)
{
	i2c_write7(i2c, HW_address, &address, 1);
	i2c_write7(i2c, HW_address, &data, 1);
	delay_ms(10);
}

uint8_t eeprom_single_read(uint32_t i2c, uint8_t HW_address, uint8_t address)
{
	uint8_t data;
	//i2c_enable_ack(i2c);
	i2c_send_start(i2c);
	
	while (!((I2C_SR1(i2c) & I2C_SR1_SB) & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

	i2c_disable_ack(i2c);
	i2c_send_7bit_address(i2c, HW_address, I2C_WRITE);
	while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));
	i2c_enable_ack(i2c);


	i2c_send_data(i2c, address);
	while (!(I2C_SR1(i2c) & I2C_SR1_BTF));

	i2c_send_start(i2c);
	while (!((I2C_SR1(i2c) & I2C_SR1_SB) & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

	i2c_send_7bit_address(i2c, HW_address, I2C_READ);
	while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));

	
	while (!(I2C_SR1(i2c) & I2C_SR1_RxNE));
	data = i2c_get_data(i2c);

	i2c_disable_ack(i2c);
	i2c_send_stop(i2c);
	delay_ms(3);
	return data;
}

uint8_t eeprom_current_read(uint32_t i2c, uint8_t HW_address)
{
	uint8_t data;

	i2c_send_start(i2c);
	while (!((I2C_SR1(i2c) & I2C_SR1_SB) & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

	//i2c_disable_ack(i2c);
	//i2c_nack_current(i2c);
	i2c_send_7bit_address(i2c, HW_address, I2C_READ);
	i2c_nack_current(i2c);
	//while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));
	//while (!(I2C_SR1(i2c)));
	sk_pin_set(sk_io_led_red, 1);

	i2c_disable_ack(i2c);

	while (!(I2C_SR1(i2c) & I2C_SR1_RxNE));
	data = i2c_get_data(i2c);
	
	i2c_send_stop(i2c);
	//delay_ms(3);
}

void i2c_burst_read(uint32_t i2c, uint8_t HW_address, uint8_t addr, uint8_t n_data, uint8_t *data)
{
	while ((I2C_SR2(i2c) & I2C_SR2_BUSY));

	i2c_send_start(i2c);
	while (!((I2C_SR1(i2c) & I2C_SR1_SB) & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

	i2c_send_7bit_address(i2c, HW_address, I2C_WRITE);
	while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));

	i2c_send_data(i2c, addr);
	while (!(I2C_SR1(i2c) & I2C_SR1_BTF));

	i2c_send_stop(i2c);
	i2c_send_start(i2c);
	while (!((I2C_SR1(i2c) & I2C_SR1_SB) & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

	i2c_send_7bit_address(i2c, HW_address, I2C_READ);
	while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));

	i2c_enable_ack(i2c);
	uint8_t i =0;
	while(n_data--) {
		if(!n_data) i2c_disable_ack(i2c);
			while (!(I2C_SR1(i2c) & I2C_SR1_RxNE));
                 data[i] = i2c_get_data(i2c);
                 i++;
	}
	i2c_disable_ack(i2c);
	i2c_send_stop(i2c);
	delay_ms(10);


}


void i2c_read7(uint32_t i2c, int addr, uint8_t *res, size_t n)
 {
 		while(1) {

 			i2c_send_start(i2c);
         	i2c_enable_ack(i2c);
 			while (!((I2C_SR1(i2c) & I2C_SR1_SB) & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));
  
         	i2c_send_7bit_address(i2c, addr, I2C_READ);
 		}
         i2c_send_start(i2c);
         i2c_enable_ack(i2c);
  
         /* Wait for master mode selected */
         while (!((I2C_SR1(i2c) & I2C_SR1_SB) & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));
  
         i2c_send_7bit_address(i2c, addr, I2C_READ);
  
         /* Waiting for address is transferred. */
         while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));
         /* Clearing ADDR condition sequence. */
         (void)I2C_SR2(i2c);
  
         for (size_t i = 0; i < n; ++i) {
                 if (i == n - 1) {
                         i2c_disable_ack(i2c);
                 }
                 while (!(I2C_SR1(i2c) & I2C_SR1_RxNE));
                 res[i] = i2c_get_data(i2c);
         }
         i2c_send_stop(i2c);
  
         return;
 }

uint8_t eeprom_trans(uint32_t i2c, uint8_t HW_address, uint8_t addr)
{
	uint8_t data;
	i2c_disable_ack(i2c);
	i2c_nack_current(i2c);
	i2c_nack_next(i2c);
	i2c_transfer7(i2c, HW_address, &addr, 1, &data, 1);
	return data;
}
uint8_t ee_test(uint32_t i2c, uint8_t HW_address, uint8_t addr)
{
	uint8_t data;

	i2c_send_start(i2c);
	while (!((I2C_SR1(i2c) & I2C_SR1_SB) & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

	//i2c_disable_ack(i2c);
	//i2c_nack_current(i2c);
	i2c_send_7bit_address(i2c, HW_address, I2C_WRITE);

	i2c_send_stop(i2c);

	i2c_send_start(i2c);
	while (!((I2C_SR1(i2c) & I2C_SR1_SB) & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

	i2c_send_7bit_address(i2c, HW_address, I2C_READ);
	//i2c_nack_current(i2c);
	//while (!(I2C_SR1(i2c) & I2C_SR1_ADDR));
	//while (!(I2C_SR1(i2c)));
	sk_pin_set(sk_io_led_red, 1);
	uint8_t tmp = 4;
	//i2c_disable_ack(i2c);
	while(tmp--) {
		while (!(I2C_SR1(i2c) & I2C_SR1_RxNE));
		data = i2c_get_data(i2c);
	}
	
	
	i2c_send_stop(i2c);
	//delay_ms(3);
	return data;
}
int main(void)
{
	
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOC);
	rcc_periph_clock_enable(RCC_GPIOD);
	rcc_periph_clock_enable(RCC_GPIOE);

	clock_hse_168MHZ_init();
	timer_delay_init(84 - 1);
	glsk_pins_init(false);
	sk_lcd_set_backlight(&lcd, 0xFF);
	
	for (int i = 0; i < 5; i++)
		lcd_init_4bit(&lcd);
	lcd_set_cursor(&lcd, 0, 0);
	lcd_send_string(&lcd, "MDAAA");
	delay_ms(2000);
	eeprom_init(I2C1);
	
	delay_ms(1000);
	uint8_t data;
	char buff[20];
	uint8_t res[1];

	//eeprom_single_write(I2C1, 0x50, 0x26, 0x01);
	//eeporm_write_byte(I2C1, 0b1010111 , 0x31, 0x31);
	//uint8_t MAC_Address[6] = { 0 };
	//i2c_burst_read(I2C1, 0x50, 0xFA, 6, MAC_Address);
	//data = eeprom_single_read(I2C1, 0x58, 0xFA);
	//data = eeprom_current_read(I2C1, 0x50);
	//i2c_read7(I2C1, 0x50, res, 1);
	data = ee_test(I2C1, 0x50, 0xFA);
	//data = eeprom_trans(I2C1, 0x50, 0xFA);
	delay_ms(40);
	//data = eeprom_single_read(I2C1, 0x50, 0x31);

	lcd_set_cursor(&lcd, 1, 0);
	snprintf(buff, sk_arr_len(buff), "data=%d", res[0]);
	lcd_send_string(&lcd, buff);
	while(1) {
		delay_ms(1000);
		sk_pin_toggle(sk_io_led_blue);
	}
}
/*
#include "lcd_hd44780.h"
//#include "pin.h"
#include "timer_delay.h"
#include "macro.h"
#include "clkset.h"
#include "printf.h"
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/i2c.h>
#include <stdint.h>
#include <stddef.h>
//#include <math.h>



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




static void i2c_init(uint32_t i2c)
{
	// Setup GPIO
	rcc_periph_clock_enable(RCC_GPIOB);
	gpio_set_output_options(GPIOB, GPIO_OTYPE_OD, GPIO_OSPEED_2MHZ, (1 << 6) | (1 << 9));
	gpio_set_af(GPIOB, GPIO_AF4, (1 << 6) | (1 << 9));
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, (1 << 6) | (1 << 9));

	rcc_periph_clock_enable((i2c == I2C1) ? RCC_I2C1 : RCC_I2C2);
	i2c_peripheral_disable(i2c);

	// set input clock frequency (which comes from APB1)
	i2c_set_clock_frequency(i2c, rcc_apb1_frequency / 1000000);
	
	i2c_set_standard_mode(i2c);		// set communication frequency to Sm (100 kHz)
	//i2c_set_dutycycle(i2c, I2C_CCR_DUTY_DIV2);		// Tlow / Thigh = 2 (relates only to Fm)

	// CCR = F_PCLK1 / (2 * F_i2c) = 42 MHz / (2 * 100 kHz) = 42e6 / (2 * 100e3) = 210
	i2c_set_ccr(i2c, (rcc_apb1_frequency / (2ul * 100000ul)));	// 100 kHz communication speed

	// Trise = 1 + Tmax / T_PCLK1 = 1 + F_PCLK1 * Tmax, where Tmax is given is I2C specs
	// for 100 kHz, Tmaz = 1000 ns = 1000e-9 s. => Trise = 1 + F_PCLK / 1e6
	i2c_set_trise(i2c, (1ul + rcc_apb1_frequency/1000000ul));

	i2c_peripheral_enable(i2c);
}


static float veclen(float x, float y, float z)
{
	return x*x + y*y + z*z;
}



static void simplei2c_communicate_loop(uint32_t i2c)
{
	// slave addr for magnetic sensor is 0x1E (bits 7...1)
	uint8_t saddr = 0x1E;
	
	uint8_t cmd_set_contigious[] = {
		0x22,	// register 22h | (0 << 7)
		0x00
	};
	i2c_transfer7(i2c, saddr, cmd_set_contigious, sk_arr_len(cmd_set_contigious), 0, 0);

	while (1) {
		int16_t x = 0, y = 0, z = 0;
		uint8_t var[6] = {0};
		uint8_t cmd_read_xyz = 0x28 | (1 << 7);		// auto increment
		i2c_transfer7(i2c, saddr, &cmd_read_xyz, 1, var, sk_arr_len(var));
		x = *(int16_t *)(var);
		y = *(int16_t *)(&var[2]);
		z = *(int16_t *)(&var[4]);

		float abs = veclen(x, y, z);

		char buffer[20];
		//sk_lcd_cmd_setaddr(&lcd, 0x00, false);
		lcd_set_cursor(&lcd, 0, 0);
		snprintf(buffer, sizeof(buffer), "%-6dx %-6dy", (int)x, (int)y);
		lcd_send_string(&lcd, buffer);
		lcd_set_cursor(&lcd, 1, 0);
		snprintf(buffer, sizeof(buffer), "%-6dz %-5dabs", (int)z, (int)abs);
		lcd_send_string(&lcd, buffer);
	}
}


int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOE);		// lcd is connected to port E
	rcc_periph_clock_enable(RCC_GPIOD);		// DISCOVERY LEDS are connected to Port E
	glsk_pins_init(false);

	sk_pin_set(sk_io_led_green, true);	// turn on LED until everything is configured
	clock_hse_168MHZ_init();
	i2c_init(I2C1);
	sk_pin_set(sk_io_led_green, false);

	timer_delay_init(84 - 1);

	cm_enable_interrupts();

	delay_ms(1000);
	for (int i = 0; i < 5; i++)
		lcd_init_4bit(&lcd);
	sk_lcd_set_backlight(&lcd, 200);

	simplei2c_communicate_loop(I2C1);
}*/