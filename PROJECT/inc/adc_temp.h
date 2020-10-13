#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

extern volatile uint16_t __adc_avgval;

/**
 * Setup adc and pin options for LM335
 *
 * Note:
 * No ability to change pins and adc, pins and adc are chosen strictly.
 */
void adc_temp_init(void);

//void adc_isr(void);

/**
 * Converts adc values
 * @return: converted value after averaging in ISR
 */
uint16_t adc_acquire(void);

/**
 * Uses formula to convert adc value to temperature.
 * @adcval: adc value to convert into temperature
 * @return: float temperature value.
 */
inline float adc_temp_convert(uint16_t adcval);

/**
 * Inits conversion and converts adc values to temperature in celsius
 * @return: float temperature value.
 */
float adc_get_temp(void);