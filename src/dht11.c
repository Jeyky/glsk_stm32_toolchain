#include <libopencm3/cm3/cortex.h>
#include <libopencm3/stm32/rcc.h>
#include "timer_delay.h"
#include "pin.h"
#include "dht11.h"

const sk_pin dht11 	= {.port = SK_PORTD, .pin = 11, .isinverse = false};

void dht11_start(struct sk_dht11 *dht11)
{
	//rcc_periph_clock_enable(RCC_GPIOD);

	sk_pin_mode_setup(*dht11->data_pin, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE);

	sk_pin_set(*dht11->data_pin, 0);

	delay_ms(18);

	sk_pin_set(*dht11->data_pin, 1);

	delay_us(20);

	sk_pin_mode_setup(*dht11->data_pin, GPIO_MODE_INPUT, GPIO_PUPD_NONE);
}
bool dht11_response(struct sk_dht11 *dht11)
{
	bool response = false;

	delay_us(40);
	if (!sk_pin_read(*dht11->data_pin)) {

		delay_us(80);
	}
	if (sk_pin_read(*dht11->data_pin)) {

		response = true;

		while(sk_pin_read(*dht11->data_pin));
	} 

	return response;
}

uint8_t dht11_read_byte(struct sk_dht11 *dht11) 
{
	uint8_t byte = 0;
	for (int i; i < 8; i++) {

		while(!(sk_pin_read(*dht11->data_pin)));

		delay_us(40);

		if (sk_pin_read(*dht11->data_pin)) {

			byte |= (1 << (7 - i));
		} else {

		 	byte &= ~(1 << (7 - i));
		 }
			
		while(sk_pin_read(*dht11->data_pin));
	}
	return byte;
}

uint8_t dht11_read_temprature(struct sk_dht11 *dht11) 
{
	uint8_t temp;

	dht11_start(dht11);

	dht11_response(dht11);

	dht11_read_byte(dht11);
	dht11_read_byte(dht11);
	temp = dht11_read_byte(dht11);
	dht11_read_byte(dht11);
	dht11_read_byte(dht11);

	return temp;
}

uint8_t dht11_read_humidity(struct sk_dht11 *dht11) 
{
	uint8_t humidity;

	dht11_start(dht11);

	dht11_response(dht11);

	humidity = dht11_read_byte(dht11);
	dht11_read_byte(dht11);
	dht11_read_byte(dht11);
	dht11_read_byte(dht11);
	dht11_read_byte(dht11);

	return humidity;
}
