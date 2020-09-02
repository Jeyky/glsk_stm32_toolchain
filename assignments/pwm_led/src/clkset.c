/*#include <libopencm3/cm3/cortex.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/flash.h>
#include <libopencm3/stm32/pwr.h>
*/
#include "clkset.h"


void clock_hse_init(uint32_t pllm, uint32_t plln, uint32_t pllp,uint32_t pllq,uint32_t pllr)
{

	rcc_osc_bypass_enable(RCC_HSE);

	rcc_osc_on(RCC_HSE);

	while(!rcc_is_osc_ready(RCC_HSE)); 

	rcc_periph_clock_enable(RCC_PWR);
	pwr_set_vos_scale(PWR_SCALE1);
	rcc_periph_clock_disable(RCC_PWR);

	rcc_osc_off(RCC_PLL);

	//rcc_set_main_pll_hse(4, 168, 2, 7, 0);
	rcc_set_main_pll_hse(pllm, plln, pllp, pllq, pllr);

	rcc_css_disable();
	rcc_osc_on(RCC_PLL);
	while(!rcc_is_osc_ready(RCC_PLL));

	// Set all prescalers.
	// (!) Important. Different domains have different maximum allowed clock frequencies
	//     So we need to set corresponding prescalers before switching AHB to PLL
	// AHB should not exceed 168 MHZ		-- divide 168 MHz by /1
	// APB1 should not exceed 42 MHz		-- divide AHB by /4 = 168 / 4 = 42 MHz
	// APB2 should not exceed 84 MHz		-- divide AHB by /2 = 168 / 2 = 84 MHz
	rcc_set_hpre(RCC_CFGR_HPRE_DIV_NONE);
    rcc_set_ppre1(RCC_CFGR_PPRE_DIV_4);
    rcc_set_ppre2(RCC_CFGR_PPRE_DIV_2);

	// Enable caches. Flash is slow (around 30 MHz) and CPU is fast (168 MHz)
	flash_dcache_enable();
	flash_icache_enable();

	// IMPORTANT! We must increase flash wait states (latency)
	// otherwise fetches from flash will ultimately fail
	flash_set_ws(FLASH_ACR_LATENCY_7WS);

	// Select PLL as AHB bus (and CPU clock) source
    rcc_set_sysclk_source(RCC_CFGR_SW_PLL);
	// Wait for clock domain to be changed to PLL input
	rcc_wait_for_sysclk_status(RCC_PLL);

	// set by hand since we've not used rcc_clock_setup_pll
	rcc_ahb_frequency = 168000000ul;
	rcc_apb1_frequency = rcc_ahb_frequency / 4;
	rcc_apb2_frequency = rcc_ahb_frequency / 2;

	// Disable internal 16 MHz RC oscillator (HSI)
	rcc_osc_off(RCC_HSI);
}

/*
int main(void)
{
	rcc_periph_clock_enable(RCC_GPIOD);
	glsk_pins_init(true);
	clock_hse_init(4, 168, 2, 7, 0);
	abs_sys_tick_init(168000000ul/100000ul, 2);
	sk_pin_set(sk_io_led_orange, true);
	while(1){
		abs_delay_ms(1000);
		sk_pin_toggle(sk_io_led_orange);
		sk_pin_toggle(sk_io_led_blue);
	}

}
*/