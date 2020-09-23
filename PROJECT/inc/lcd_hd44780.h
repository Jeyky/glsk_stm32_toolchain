/**
 * libsk GL-SK on-board LCD abstraction layer
 *
 * The display is WH1602B (based on HD44780 controller)
 */

#include "errors.h"
#include "pin.h"
#include <stdint.h>


/** Pointer to delay(uint32_t var) function defined as type */
typedef void (*sk_delay_func_t)(uint32_t);

extern uint8_t cursor_col;
extern uint8_t cursor_row;

struct sk_lcd {
	/** HD44780 data pins (DB0..DB7 or DB4..DB7) represented as :c:type:`sk_pin_group` */
	sk_pin_group *pin_group_data;
	/** HD44780 register select pin (RS) represented as :c:type:`sk_pin` */
	sk_pin *pin_rs;
	/** HD44780 enable pin (E) represented as :c:type:`sk_pin` */
	sk_pin *pin_en;
	/** HD44780 enable pin (R/W) represented as :c:type:`sk_pin`.
     *  Set to NULL if not used (always grounded on board) */
	sk_pin *pin_rw;
	/** Display backlight pin. Set to NULL if not used */
	sk_pin *pin_bkl;
	/** Pointer to backlight control function (i.e. to set backlight LED PWM level).
      * Set to NULL if not used. This way only two levels will be possible
      * (0 for OFF and != 0 for ON) */
	void (*set_backlight_func)(uint8_t);
	/** Pointer to user-provided delay function with microsecond resolution.
	  * Set to NULL to use ms delay as a fallback */
	sk_delay_func_t delay_func_us;
	/** Pointer to user-provided delay function with millisecond resolution.
      * Set to NULL to use us delay as a fallback */
	sk_delay_func_t delay_func_ms;
	/** True for 4-bit HD44780 interface, False for 8-bit. Only 4-bit IF is supported for now */
	unsigned int is4bitinterface : 1;
};

/**
 * Sets background brightness of display.
 * @lcd:  structure of certain display pins.
 * @level: desired level of display background brightness. 
 *
 * Note:
 * This function does not provide delay, it is a pointer to users delay function.
 */
sk_err sk_lcd_set_backlight(struct sk_lcd *lcd, uint8_t level);

/**
 * User sets delay instead of this function.
 * @lcd:  structure of certain display pins
 * @us: desired delay in microseconds.
 *
 * Note:
 * This function does not provide delay, it is a pointer to users delay function.
 */
void lcd_delay_us(struct sk_lcd *lcd, uint32_t us);

/**
 * Sends 4 bits to display
 * @lcd:  structure of certain display pins
 * @half: desired value to send to the display.
 *
 * Note:
 * Sends data in 4 bits of data, so better to use 2 times in a row to send a byte.
 */
void lcd_data_set_halfbyte(struct sk_lcd *lcd, uint8_t half);

/**
 * Sends byte to display
 * @lcd:  structure of certain display pins
 * @byte: desired value to send to the display.
 *
 * Note:
 * Uses lcd_data_set_halfbyte() two times to send a byte.
 */
void lcd_data_set_byte(struct sk_lcd *lcd, uint8_t byte);

/**
 * set rs and rw bits to send command or data or to write something in RAM of display.
 * @lcd:  structure of certain display pins
 * @rs: rs pin of display
 * @rw: rw pin of display.
 *
 * Note:
 * Used in variety of lcd functions.
 */
void lcd_rsrw_set(struct sk_lcd *lcd, bool rs, bool rw);
//void lcd_send_byte(struct sk_lcd *lcd, bool rs, uint8_t byte);

/**
 * Puts the string on display
 * @lcd:  structure of certain display pins
 * @str: array of bytes to show on display.
 *
 * Note:
 * Some symbols could be sent out of the range of display.
 */
void lcd_send_string(struct sk_lcd *lcd, const uint8_t *str);

/**
 * Sends cmd to display
 * @lcd:  structure of certain display pins
 * @byte: command that send to display.
 *
 * Note:
 * commands sends in 4bit mode.
 */
void lcd_send_cmd(struct sk_lcd *lcd, uint8_t byte);

/**
 * Sends data to display(not command).
 * @lcd:  structure of certain display pins
 * @byte: data to send to display
 *
 * Note:
 * data sends in 4bit mode.
 */
void lcd_send_data(struct sk_lcd *lcd, uint8_t byte);

/**
 * Add symbols to certain display
 * @lcd:  structure of certain display pins
 * @position: position of the symbol in CGRAM.
 * @symbol: array of values that forms the symbol.
 *
 * Note:
 * Symbols are placed in CGRAM their address starts from 0 
 */
void lcd_add_symbol(struct sk_lcd *lcd, uint8_t position, uint8_t *symbol);

/**
 * Set display cursor in desired position
 * @lcd: structure of certain display pins
 * @raw: postion on display responsible for rows
 * @column: postion on display responsible for columns
 */
void lcd_set_cursor(struct sk_lcd *lcd, uint8_t raw, uint8_t column);

void lcd_set_addr(struct sk_lcd *lcd, uint8_t addr);

/**
 * Initializes display in 4 bit mode
 * @lcd: structure of pins that relate to the certain display.
 *
 * Note:
 * It's better to initialize display in loop several times
 */
void lcd_init_4bit(struct sk_lcd *lcd);

void lcd_increase_column_cursor(struct sk_lcd *lcd);
void lcd_increase_row_cursor(struct sk_lcd *lcd);