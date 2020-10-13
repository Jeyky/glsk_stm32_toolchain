#include "pin.h"
#include "adc_temp.h"
#include "intrinsics.h"
#include "printf.h"
#include "timer_delay.h"
#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/adc.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>


void adc_temp_init(void)
{

	rcc_periph_clock_enable(RCC_GPIOB);

	gpio_set_output_options(GPIOB, GPIO_OTYPE_PP, GPIO_OSPEED_2MHZ, 1 << 1);

	gpio_mode_setup(GPIOB, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, 1 << 1);

	rcc_periph_clock_enable(RCC_ADC1);

	adc_set_clk_prescale(ADC_CCR_ADCPRE_BY8);

	adc_set_resolution(ADC1, ADC_CR1_RES_12BIT);

	adc_set_right_aligned(ADC1);

	uint8_t channels[16];

	for (int i = 0; i < sk_arr_len(channels); i++) {
		adc_set_sample_time(ADC1, i, ADC_SMPR_SMP_480CYC);
		channels[i] = 9;	
	}

	adc_set_regular_sequence(ADC1, sk_arr_len(channels), channels);

	adc_eoc_after_each(ADC1);

	adc_enable_scan_mode(ADC1);

	adc_set_single_conversion_mode(ADC1);

	adc_enable_eoc_interrupt(ADC1);
	adc_enable_overrun_interrupt(ADC1);

	nvic_set_priority(NVIC_ADC_IRQ, 10);
	nvic_enable_irq(NVIC_ADC_IRQ);

	adc_power_on(ADC1);
	
	delay_ms(10);
}

volatile uint16_t __adc_avgval = 0;

void adc_isr(void)
{
	static volatile uint32_t sum = 0;
	static volatile uint32_t cnt = 0;

	if (adc_get_overrun_flag(ADC1)) {
		// means we got to interrupt because of overrun flag
		// we have not read all data and overrun occured -> reset everything
		sum = cnt = 0;
		adc_clear_flag(ADC1, ADC_SR_OVR);	// reset flag to avoid infinite interrupting
		return;
	}

	// otherwise we got here because one of channels was converted
	sum += adc_read_regular(ADC1) & 0x00000FFF;
	cnt++;

	if (cnt >= 16) {		// all sequence was read
		__adc_avgval = sum / cnt;
		sum = cnt = 0;
		adc_clear_flag(ADC1, ADC_SR_STRT);	// clear regular channel
	}

	adc_clear_flag(ADC1, ADC_SR_EOC);	// clear end of conversion flag not to cycle
}


uint16_t adc_acquire(void)
{
	// start conversion
	adc_start_conversion_regular(ADC1);
	
	// wait for conversion to start
	while (! adc_get_flag(ADC1, ADC_SR_STRT)) {
		__WFI();
	}
	// sleep while adc has not finished coverting all channels in group
	while (adc_get_flag(ADC1, ADC_SR_STRT)) {
		__WFI();
	}

	return __adc_avgval;	// converted value after averaging in ISR
}


inline float adc_temp_convert(uint16_t adcval)
{
	return 99.61 - 0.03896 * adcval;
}

float adc_get_temp(void)
{
	uint16_t adcval = adc_acquire();
	return adc_temp_convert(adcval);
}
