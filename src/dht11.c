#include <libopencm3/cm3/cortex.h>
#include <libopencm3/stm32/rcc.h>
#include "timer_delay.h"
#include "pin.h"
#include "dht11.h"

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

		if (sk_pin_read(dht11)) {

			byte |= (1 << (7 - i));
		} else {

		 	byte &= ~(1 << (7 - i));
		 }
			
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
