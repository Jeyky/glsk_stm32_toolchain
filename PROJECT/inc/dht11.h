#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef void (*sk_delay_func_t)(uint32_t);

struct sk_dht11 {
	sk_pin *data_pin;

	sk_delay_func_t delay_func_ms;

	sk_delay_func_t delay_func_us;


};

/**
 * Initializes start of conversation with dht11 sensor.
 *
 * Note:
 * Sets dht11 pin to output, sets to 0, waits for 18 ms, sets to 1 and set pin settings to input.
 */
void dht11_start(struct sk_dht11 *dht11);

/**
 * Gets response value of sensor
 * @return: responce value of sensor
 *
 */
bool dht11_response(struct sk_dht11 *dht11);

/**
 * reads one byte after all previous sequnce of checks and commands
 * @return: read byte.
 *
 * Note:
 * To read whole data from sensor we should read 5 times in a row.
 */
uint8_t dht11_read_byte(struct sk_dht11 *dht11);

/**
 * reads temrature value from sensor
 * @return: temprature byte.
 *
 * Note:
 * Reads only third byte, do not return other values (humidity etc.)
 */
uint8_t dht11_read_temprature(struct sk_dht11 *dht11);

/**
 * reads humidity percent value from sensor
 * @return: humidity byte
 *
 * Note:
 * Reads only first byte in byte sequence. Humidity value in percents. Do not return other values (humidity etc.)
 */
uint8_t dht11_read_humidity(struct sk_dht11 *dht11);

extern const sk_pin dht11;