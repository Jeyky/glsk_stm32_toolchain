//#include "lcd_hd44780.h"
#include "pin.h"
#include "timer_delay.h"
//#include "clkset.h"
#include "macro.h"
#include "spi_flash.h"
#include <printf.h>
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/pwr.h>
#include <libopencm3/stm32/rcc.h>
//#include <libopencm3/stm32/spi.h>
#include <stdint.h>
#include <stddef.h>

const sk_pin sk_io_spiflash_ce ={.port = SK_PORTD, .pin = 7, .isinverse = false};
/*
static struct sk_lcd lcd = {
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
*/

// Among all GL-SK peripherals, only external SPI flash (SST25VF016B) is connected via SPI
// PA5 - SPI1_SCK	- Alternate function AF5
// PB5 - SPI1_MOSI	- Alternate function AF5
// PB4 - SPI1_MISO	- Alternate function AF5
// PD7 - ~CS		- driven manually, use push-pull out with pullup (PULLUP IS IMPORTANT)
//
void spi_init(void)
{
	// Setup GPIO

	// Our SST25VF016B memory chip has maximum clock frequency of 80 MHz, so set speed to high
	rcc_periph_clock_enable(RCC_GPIOA);
	rcc_periph_clock_enable(RCC_GPIOB);
	rcc_periph_clock_enable(RCC_GPIOD);
	// Pins directly assigned to SPI peripheral
	gpio_set_output_options(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, 1 << 5);
	gpio_set_af(GPIOA, GPIO_AF5, 1 << 5);
	gpio_mode_setup(GPIOA, GPIO_MODE_AF, GPIO_PUPD_NONE, 1 << 5);

	gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, (1 << 5) | (1 << 4));
	gpio_set_af(GPIOB, GPIO_AF5, (1 << 5) | (1 << 4));
	gpio_mode_setup(GPIOB, GPIO_MODE_AF, GPIO_PUPD_NONE, (1 << 5) | (1 << 4));
	// CS Pin we drive manually
	gpio_set_output_options(GPIOD, GPIO_OTYPE_PP, GPIO_OSPEED_100MHZ, 1 << 7);
	gpio_mode_setup(GPIOD, GPIO_MODE_OUTPUT, GPIO_PUPD_PULLUP, 1 << 7);
	gpio_set(GPIOD, 1 << 7);

	rcc_periph_clock_enable(RCC_SPI1);
	// Disable SPI1 before configuring
	spi_disable(SPI1);		// not required here, SPI is disabled after Reset
	// SPI1 belongs to APB2 (84 MHz frequency)
	// We have /2, /4, /8, /16, /32, /64, /128 and /256 prescalers
	// Our SPI flash can work up to 50 MHz ... 80 MHz clock (depending on part marking)
	// But to be able to capture data with logic analyzer, which has maximum frequency of 24 MHz,
	// set spi baudrate to /32, which gives us 84/32 = 2.6 MHz SCLK frequency
	spi_set_baudrate_prescaler(SPI1, SPI_CR1_BR_FPCLK_DIV_32);
	// Our MCU is master and flash chip is slave
	spi_set_master_mode(SPI1);
	// We work in full duplex (simultaneous transmit and receive)
	spi_set_full_duplex_mode(SPI1);
	// Data frame format is 8 bit, not 16
	spi_set_dff_8bit(SPI1);
	// No CRC calculation is required
	spi_disable_crc(SPI1);
	// Our flash chip requires Most Significant Bit first (datasheet p. 5, Figure 3. SPI Protocol)
	spi_send_msb_first(SPI1);
	// Flash chip can work in Mode 0 (polarity 0, phase 0) and Mode 3 (polarity 1, phase 1)
	// But the chip inputs data on rising edge and outputs data on falling edge, so
	// MCU is better to read data on rising edge and output on falling edge -- select mode 3
	spi_set_clock_polarity_1(SPI1);
	spi_set_clock_phase_1(SPI1);
	// Set hardware control of NSS pin. Because disabling it can cause master to become slave
	// depending on NSS input state.
	// Normally NSS will be hard-wired to slave. But in our case we have other pin connected to
	// slave CS and should drive it manually
	spi_enable_ss_output(SPI1);

	// In this example we will work with flash without using interrupts
	// So simply enable spi peripheral
	spi_enable(SPI1);
}


// sets cs pin
void cs_set(bool state)
{
	sk_pin_set(sk_io_spiflash_ce, state);
}

uint8_t flash_read_status_register(uint32_t spi)
{
	uint8_t status;
	cs_set(0);
	spi_send(spi, 0x05);
	status = spi_read(spi);
	cs_set(1);
	return status;
}

static void flash_write_byte(uint32_t spi, uint8_t data)
{
	spi_send(spi, data);
	spi_read(spi);	
}

static uint8_t flash_read_byte(uint32_t spi)
{
    spi_send(spi, 0x00);
    return spi_read(spi);
}

static void flash_tx(uint32_t spi,uint32_t len, const void *data)
{
	uint8_t *d = data;
	if ((!len) || (NULL == d))
		return;

	for (int32_t i = len - 1; i >= 0; i--) {
		spi_send(spi, d[i]);
		spi_read(spi);		// dummy read to provide delay
	}
}

static void flash_rx(uint32_t spi,uint32_t len, void *data)
{

	uint8_t *d = data;
	if ((!len) || (NULL == d))
		return;

	for (int32_t i = len - 1; i >= 0; i--) {
		spi_send(spi, 0);
		d[i] = spi_read(spi);
	}
}


struct __attribute__((packed, aligned(1))) 
       __attribute__(( scalar_storage_order("little-endian") ))
	   flash_jedec_id {
	// order matters here
	uint16_t device_id;
	uint8_t manufacturer;
};

uint16_t flash_read_id(uint32_t spi)
{
	while(flash_read_status_register(spi) & (1 << 0));
	cs_set(0);
	uint32_t read_id= 0x90000001;
	flash_tx(spi, 4, &read_id);
	uint16_t id;
	flash_rx(spi, 2, &id);
	cs_set(1);
	return id;
}
uint8_t flash_status(uint32_t spi)
{
	uint8_t cmd = 0x05;

	cs_set(0);
	flash_tx(spi, 1, &cmd);
	uint8_t status;
	flash_rx(spi, 1, &status);
	cs_set(1);
	return status;

}
void flash_wr_disable(uint32_t spi)
{
	uint8_t cmd = 0x04;

	cs_set(0);
	flash_tx(spi, 1, &cmd);
	cs_set(1);
}

void flash_wr_enable(uint32_t spi)
{
	uint8_t cmd = 0x06;
	cs_set(0);
	flash_tx(spi, 1, &cmd);
	cs_set(1);
}

void flash_erase_4kbyte(uint32_t spi, uint32_t address)
{
	uint8_t cmd = 0x20;
	cs_set(0);
	flash_tx(spi, 1, &cmd);
	flash_tx(spi, 3, &address);
	cs_set(1);
}

void flash_byte_program(uint32_t spi, uint32_t address, uint8_t data)
{
	uint8_t cmd = 0x02;
	cs_set(0);
	flash_tx(spi, 1, &cmd);
	flash_tx(spi, 3, &address);
	flash_tx(spi, 1, &data);
	cs_set(1);
}

void flash_read_data(uint32_t spi,uint32_t address, uint8_t *data, uint32_t len)
{
	while(flash_read_status_register(spi) & (1 << 0));
	cs_set(0);
	flash_write_byte(spi, 0x03);
	flash_write_byte(spi, (uint8_t)(address >> 16));
	flash_write_byte(spi, (uint8_t)(address >> 8));
	flash_write_byte(spi, (uint8_t)(address));
	for (uint32_t i = 0; i < len; i++) {
		data[i] = flash_read_byte(spi);
	}
	cs_set(1);
}

void flash_get_password(uint8_t *pass_arr)
{
	uint8_t data[2];
	flash_read_data(SPI1,0x000000, data, 2);

	pass_arr[0] = (data[0] & 0xE0) >> 6;
	pass_arr[1] = (data[0] & 0x13) >> 4;

	pass_arr[2] = (data[1] & 0xE0) >> 5;
	pass_arr[3] = (data[1] & 0x13);
}

void flash_get_threshold(uint8_t *pass_arr)
{
	uint8_t data[2];
	flash_read_data(SPI1,0x000001, pass_arr, 1);

}
/*
int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOE);		// lcd is connected to port E
	rcc_periph_clock_enable(RCC_GPIOD);		// DISCOVERY LEDS are connected to Port E
	glsk_pins_init(false);

	sk_pin_set(sk_io_led_green, true);	// turn on LED until everything is configured
	//clock_hse_init(4, 168, 2, 7, 0);
	clock_hse_168MHZ_init();
	sk_pin_set(sk_io_led_green, false);

	//abs_sys_tick_init(168000000ul / 10000ul, (2 << 2 | 0));
	timer_delay_init(84 - 1);

	cm_enable_interrupts();

	sk_lcd_set_backlight(&lcd, 0xFF);
	lcd_init_4bit(&lcd);

	spi_init();



	
		// SPI communication demo
	sk_pin_set(sk_io_led_green, true);
	cs_set(0);		// assert enable signal

	const uint8_t cmd_jedec_id_get = 0x9F;
	flash_tx(SPI1, 1, &cmd_jedec_id_get);

	struct flash_jedec_id jedec_id = { 0 };
	flash_rx(SPI1, sizeof(jedec_id), &jedec_id);

	cs_set(1);
	sk_pin_set(sk_io_led_green, false);

	char buffer[20];
	lcd_set_cursor(&lcd, 0, 0);
	snprintf(buffer, sizeof(buffer), "Man:%Xh", (unsigned int)jedec_id.manufacturer);
	lcd_send_string(&lcd, buffer);

	lcd_set_cursor(&lcd, 1, 0);
	snprintf(buffer, sizeof(buffer), "Serial:%Xh", (unsigned int)jedec_id.device_id);
	lcd_send_string(&lcd, buffer);

	sk_pin_toggle(sk_io_led_orange);
	delay_ms(1000);
	uint16_t flash_id = flash_read_id(SPI1);
	lcd_set_cursor(&lcd,0,0);
	snprintf(buffer, sizeof(buffer), "dev id:%Xh", (uint16_t) flash_id);
	lcd_send_string(&lcd, buffer);

	flash_wr_enable(SPI1);
	//flash_erase_4kbyte(SPI1, 0x00000000);
	//flash_wr_enable(SPI1);
	//flash_byte_program(SPI1, 0x00000005,228);

	uint8_t data = 0;
	flash_read_data(SPI1,0x00000005, &data, 1);


	//flash_chip_erase(SPI1);					///////
	//flash_4kbyte_erase(SPI1, 0x00000000);

	//flash_write_enable(SPI1);
	//if(!(flash_read_status_register(SPI1) & (1 << 1))) {
	//	sk_pin_set(sk_io_led_blue,1);
	//}
	//flash_wr_disable(SPI1);
	//uint8_t st= flash_status(SPI1);
	lcd_set_cursor(&lcd,1,0);
	snprintf(buffer, sizeof(buffer), "data:%uh     ", data);
	lcd_send_string(&lcd, buffer);


	while (1) {

	}
}*/