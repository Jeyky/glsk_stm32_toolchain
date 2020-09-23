#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

extern volatile uint16_t __adc_avgval;

void adc_temp_init(void);
void adc_isr(void);
uint16_t adc_acquire(void);
inline float adc_temp_convert(uint16_t adcval);
float adc_get_temp(void);