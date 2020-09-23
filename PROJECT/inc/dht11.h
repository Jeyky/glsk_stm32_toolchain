#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

/**
 * Initializes start of conversation with dht11 sensor.
 *
 * Note:
 * Sets dht11 pin to output, sets to 0, waits for 18 ms, sets to 1 and set pin settings to input.
 */
void dht11_start(void);

/**
 * Gets response value of sensor
 * @return: responce value of sensor
 *
 */
bool dht11_response(void);

/**
 * reads one byte after all previous sequnce of checks and commands
 * @return: read byte.
 *
 * Note:
 * To read whole data from sensor we should read 5 times in a row.
 */
uint8_t dht11_read_byte(void);

/**
 * reads temrature value from sensor
 * @return: temprature byte.
 *
 * Note:
 * Reads only third byte, do not return other values (humidity etc.)
 */
uint8_t dht11_read_temprature(void);

/**
 * reads humidity percent value from sensor
 * @return: humidity byte
 *
 * Note:
 * Reads only first byte in byte sequence. Humidity value in percents. Do not return other values (humidity etc.)
 */
uint8_t dht11_read_humidity(void);

extern const sk_pin dht11;