/**
 * abstrction systick - provides systick support
 * 
 * abs_systick may be used to provide delay functions
 */

#include "macro.h"
#include <stdint.h>
#include <stdbool.h>

extern volatile uint32_t __nticks;

inline void abs_set_current_tick(uint32_t tick)
{
	__nticks = tick;
}

inline uint32_t abs_get_current_tick(void)
{
	return __nticks;
}

/**
 * Initialize SysTick
 * @period: Tick period in clock cycles. The period value can be any value in
 * the range 0x00000001-0x00FFFFFF
 * @irq_priority: Systick interrupt priority 
 * @return: If init success returns true, on failure returns false
 *
 * Note:
 * If period multiple of 8, divides period and ahb by 8. 
 */
bool abs_sys_tick_init(uint32_t period, uint8_t irq_priority);

/** 
 * Default SysTick ISR handler.
 */
void sys_tick_handler(void);

/**
 * Returns tick rate in Hz
 * Note: the value may be rounded due to the use of integers.
 */
uint32_t abs_get_tick_rate(void);

/**
 * Millisecond delay provided by counting ticks
 * Note: puts microcontroller in sleep mode by WFI command until needed tick value is reached.
 */
void abs_delay_ms(uint32_t ms);
