#include "lcd_hd44780.h"
#include "timer_delay.h"
#include "printf.h"
#include "buttons.h"
#include "menu.h"
#include "libopencm3/cm3/cortex.h"
#include "libopencm3/cm3/nvic.h"
#include "libopencm3/stm32/gpio.h"
#include "libopencm3/stm32/flash.h"
#include "libopencm3/stm32/pwr.h"
#include "libopencm3/stm32/rcc.h"
#include "stdint.h"
#include "stddef.h"
#include "pwm.h"
#include "spi_flash.h"

#define EMPTY_STR "                 "

menus next_menu = password_menu;

menus current_menu = password_menu;

uint8_t password[4];		//3 1 0 2

uint8_t displayed_pass[4] = {0, 0, 0, 0};

uint8_t attempts = 0;

bool dashboard_status = true;

bool rotation_status = false;

double sound_speed = 343.3;

uint8_t threshold = 20;

void set_zeros(uint8_t *arr, uint8_t arr_len)
{
	for(int i =0; i < arr_len; i++) {
		arr[i] = 0;
	}
}

void lcd_print_menu(struct sk_lcd *lcd, menus menu)
{
	char buffer[20];
	switch(menu) {
		case password_menu:
			flash_get_threshold(&threshold);
			flash_get_password(password);	
			lcd_set_cursor(lcd, 0, 0);
			lcd_send_string(lcd, "ENTER PASSWORD:");
			lcd_set_cursor(lcd, 1, 0);
			lcd_send_string(lcd, EMPTY_STR);
			lcd_set_cursor(lcd, 1, 6);

			snprintf(buffer, sk_arr_len(buffer), "%c%c%c%c", 	'*',
																'*',
																'*',
																'*');
			lcd_send_string(lcd, buffer);
			cursor_row = 1;
			cursor_col = 6;
			lcd_set_cursor(lcd, cursor_row, cursor_col);
			break;
		
		case info_menu:
			lcd_set_cursor(lcd, 0, 0);
			snprintf(buffer, sk_arr_len(buffer), "speed=%.1fm/s      ", sound_speed);
			lcd_send_string(lcd, buffer);
			lcd_set_cursor(lcd, 1, 0);
			snprintf(buffer, sk_arr_len(buffer), "threshold:%d      ", threshold);
			lcd_send_string(lcd, buffer);
			//lcd_set_cursor(lcd, cursor_row, cursor_col);
			break;
		case dashboard_menu:
			lcd_set_cursor(lcd, 0, 0);
			lcd_send_string(lcd, "dashboard:on       ");
			lcd_set_cursor(lcd, 1, 0);
			lcd_send_string(lcd, "distance:        ");
			lcd_set_cursor(lcd, cursor_row, cursor_col);
			break;
		case rotation_menu:
			lcd_set_cursor(lcd, 0, 0);
			lcd_send_string(lcd, "rotation:         ");
			lcd_set_cursor(lcd, 1, 0);
			lcd_send_string(lcd, "distance:        ");
			lcd_set_cursor(lcd, 0, 9);
			(rotation_status) ?  (snprintf(buffer, sk_arr_len(buffer), "%s", "on")) : (snprintf(buffer, sk_arr_len(buffer), "%s", "off")) ;
			lcd_send_string(lcd, buffer);
			lcd_set_cursor(lcd, cursor_row, cursor_col);
			break;
		case dialog_menu:
			lcd_set_cursor(lcd, 0, 0);
			lcd_send_string(lcd, "  LOCK SYSTEM?  ");
			lcd_set_cursor(lcd, 1, 0);
			lcd_send_string(lcd,"\"<\":NO   \">\":YES ");
			break;

		case lock_menu:	
			lcd_set_cursor(lcd, 0, 0);
			lcd_send_string(lcd, " Enter password: ");
			lcd_set_cursor(lcd, 1, 0);
			lcd_send_string(lcd, EMPTY_STR);
			lcd_set_cursor(lcd, 1, 6);

			snprintf(buffer, sk_arr_len(buffer), "%c%c%c%c", 	'*',
																'*',
																'*',
																'*');
			lcd_send_string(lcd, buffer);
			cursor_row = 1;
			cursor_col = 6;
			lcd_set_cursor(lcd, cursor_row, cursor_col);
			break;
		
	}
}
/*
void lcd_btn_menu_handler(struct sk_lcd *lcd) {
	if(btn_up_status) {
		cursor_row == 0 ? (cursor_row = 1) : (cursor_row = 0);
		btn_up_status = !btn_up_status;
	}
	if(btn_down_status) {
		cursor_row == 0 ? (cursor_row = 1) : (cursor_row = 0);
		btn_down_status = !btn_down_status;
	}
	if(btn_right_status) {
		(cursor_col < 15) ? cursor_col+=1 : (cursor_col = 0);
		btn_right_status = !btn_right_status;
	}
	if(btn_left_status) {
		(cursor_col > 0) ? cursor_col-=1 : (cursor_col = 15);
		btn_left_status = !btn_left_status;
	}
	if(btn_middle_status) {
		
		if(next_menu < 5) {
			next_menu+=1;
		}
		btn_middle_status = !btn_middle_status;
	}
	lcd_set_cursor(lcd, cursor_row, cursor_col);
}*/	
void print_stars(struct sk_lcd *lcd, uint8_t pos)
{
	char buffer[20];
	lcd_set_cursor(lcd, 1, 6);
	snprintf(buffer, sk_arr_len(buffer), "%c%c%c%c", 	'*',
														'*',
														'*',
														'*');
	lcd_send_string(lcd, buffer);
	lcd_set_cursor(lcd, 1, pos);
	snprintf(buffer, sk_arr_len(buffer), "%d", displayed_pass[pos - 6]);
	lcd_send_string(lcd, buffer);
}
void lcd_password_menu_handler(struct sk_lcd *lcd) 
{
	char buffer[20];
	bool verification = true;
	uint8_t pass[4]= {0};
	if(btn_up_status) {
		switch(cursor_col) {
			case 6:
				if(displayed_pass[0] < 9) {
					displayed_pass[0]++;
					pass[0]++;
				}

				break;
			case 7:
				if(displayed_pass[1] < 9) {
					displayed_pass[1]++;
					pass[1]++;
				}
				break;
			case 8:
				if(displayed_pass[2] < 9) {
					displayed_pass[2]++;
					pass[2]++;
				}
				break;
			case 9:
				if(displayed_pass[3] < 9) {
					displayed_pass[3]++;
					pass[3]++;
				}
				break;
		}
		print_stars(lcd, cursor_col);
		//cursor_col
		/*lcd_set_cursor(lcd, 1, 6);
		snprintf(buffer, sk_arr_len(buffer), "%d%d%d%d", 	displayed_pass[0],
															displayed_pass[1],
															displayed_pass[2],
															displayed_pass[3]);
		lcd_send_string(lcd, buffer);*/

		sound_click();

		btn_up_status = !btn_up_status;
	}
	if(btn_down_status) {
		switch(cursor_col) {
			case 6:
				if(displayed_pass[0] > 0) {
					displayed_pass[0]--;
					pass[0]--;
				}
				break;
			case 7:
				if(displayed_pass[1] > 0) {
					displayed_pass[1]--;
					pass[1]--;
				}
				break;
			case 8:
				if(displayed_pass[2] > 0) {
					displayed_pass[2]--;
					pass[2]--;
				}
				break;
			case 9:
				if(displayed_pass[3] > 0) {
					displayed_pass[3]--;
					pass[3]--;
				}
				break;
		}
		print_stars(lcd, cursor_col);
		/*lcd_set_cursor(lcd, 1, 6);
		snprintf(buffer, sk_arr_len(buffer), "%d%d%d%d", 	displayed_pass[0],
															displayed_pass[1],
															displayed_pass[2],
															displayed_pass[3]);
		lcd_send_string(lcd, buffer);*/

		sound_click();

		btn_down_status = !btn_down_status;
	}
	if(btn_right_status) {
		(cursor_col < 9) ? (cursor_col++) : (cursor_col = 6);

		print_stars(lcd, cursor_col);
		sound_click();

		btn_right_status = !btn_right_status;
	}
	if(btn_left_status) {
		(cursor_col > 6) ? (cursor_col--) : (cursor_col = 9);

		print_stars(lcd, cursor_col);
		sound_click();

		btn_left_status = !btn_left_status;
	}
	if(btn_middle_status) {
		for(uint8_t i = 0; i < 4; i++) {
			if(password[i] != displayed_pass[i])
				verification = false;
		}
		if(verification) {
			set_zeros(displayed_pass, sk_arr_len(displayed_pass));
			next_menu++;
		}
		else {
			lcd_set_cursor(lcd, 0, 0);
			lcd_send_string(lcd, "WRONG PASSWORD  ");
			delay_ms(attempts*3000 + 1000);
			set_zeros(displayed_pass, sk_arr_len(displayed_pass));
			//displayed_pass[0] = 0;
			//displayed_pass[1] = 0;
			//displayed_pass[2] = 0;
			//displayed_pass[3] = 0;

			attempts++;
			
			if(attempts < 4) {
				//delay_ms(attempts*1 + 1000);
				lcd_print_menu(lcd, password_menu);
			} else {
				lcd_set_cursor(lcd, 0, 0);
				lcd_send_string(lcd, "   ALARM ALARM  ");
				alarm_on();
			}
			
		}

		sound_click();

		btn_middle_status = !btn_middle_status;
	}
	
	lcd_set_cursor(lcd, cursor_row, cursor_col);
}

void lcd_info_menu_handler(struct sk_lcd *lcd) 
{
	char buffer[20];

	if(btn_up_status) {

		threshold+=5;

		lcd_set_cursor(lcd, 1, 10);
		snprintf(buffer, sk_arr_len(buffer), "%d", threshold);
		lcd_send_string(lcd, buffer);

		sound_click();

		btn_up_status = !btn_up_status;
	}
	if(btn_down_status) {

		if(threshold > 5)
			threshold-=5;

		lcd_set_cursor(lcd, 1, 10);
		snprintf(buffer, sk_arr_len(buffer), "%d    ", threshold);
		lcd_send_string(lcd, buffer);

		sound_click();

		btn_down_status = !btn_down_status;
	}
	if(btn_right_status) {
		next_menu++;

		sound_click();

		btn_right_status = !btn_right_status;
	}
	if(btn_left_status) {
		//next_menu--;
		btn_left_status = !btn_left_status;
	}
	if(btn_middle_status) {
		next_menu++;

		sound_click();

		btn_middle_status = !btn_middle_status;
	}
	
	//lcd_set_cursor(lcd, cursor_row, cursor_col);
}

void lcd_dashboard_menu_handler(struct sk_lcd *lcd) 
{

	if(btn_up_status) {

		dashboard_status = !dashboard_status;

		lcd_set_cursor(lcd, 0, 10);
		//snprintf(buffer, sk_arr_len(buffer), "%s  ", "on")
		dashboard_status ? (lcd_send_string(lcd, "on ")) : (lcd_send_string(lcd, "off "));
		//lcd_send_string(lcd, buffer);

		btn_up_status = !btn_up_status;
		sound_click();
	}
	if(btn_down_status) {

		dashboard_status = !dashboard_status;

		lcd_set_cursor(lcd, 0, 10);
		dashboard_status ? (lcd_send_string(lcd, "on ")) : (lcd_send_string(lcd, "off "));

		btn_down_status = !btn_down_status;
		sound_click();
	}
	if(btn_right_status) {
		next_menu++;
		btn_right_status = !btn_right_status;
		//sound_click();
	}
	if(btn_left_status) {
		next_menu--;
		btn_left_status = !btn_left_status;
		sound_click();
	}
	if(btn_middle_status) {
		dashboard_status = !dashboard_status;
		sound_click();

		lcd_set_cursor(lcd, 0, 10);
		dashboard_status ? (lcd_send_string(lcd, "on ")) : (lcd_send_string(lcd, "off "));

		btn_middle_status = !btn_middle_status;
	}
	
	//lcd_set_cursor(lcd, cursor_row, cursor_col);
}

void lcd_rotation_menu_handler(struct sk_lcd *lcd) 
{

	if(btn_up_status) {

		rotation_status = !rotation_status;

		lcd_set_cursor(lcd, 0, 9);
		//snprintf(buffer, sk_arr_len(buffer), "%s  ", "on")
		rotation_status ? (lcd_send_string(lcd, "on ")) : (lcd_send_string(lcd, "off "));
		//lcd_send_string(lcd, buffer);

		btn_up_status = !btn_up_status;
		sound_click();
	}
	if(btn_down_status) {

		rotation_status = !rotation_status;

		lcd_set_cursor(lcd, 0, 9);
		rotation_status ? (lcd_send_string(lcd, "on ")) : (lcd_send_string(lcd, "off "));

		btn_down_status = !btn_down_status;
		sound_click();
	}
	if(btn_right_status) {
		next_menu++;
		btn_right_status = !btn_right_status;
		sound_click();
	}
	if(btn_left_status) {
		next_menu--;
		btn_left_status = !btn_left_status;
		sound_click();
	}
	if(btn_middle_status) {
		rotation_status = !rotation_status;
		sound_click();

		lcd_set_cursor(lcd, 0, 9);
		rotation_status ? (lcd_send_string(lcd, "on ")) : (lcd_send_string(lcd, "off "));

		btn_middle_status = !btn_middle_status;
	}
	
	//lcd_set_cursor(lcd, cursor_row, cursor_col);
}

void lcd_dialog_menu_handler(struct sk_lcd *lcd) 
{

	if(btn_up_status) {

		btn_up_status = !btn_up_status;
		//sound_click();
	}
	if(btn_down_status) {

		btn_down_status = !btn_down_status;
		//sound_click();
	}
	if(btn_right_status) {
		next_menu++;
		btn_right_status = !btn_right_status;
		sound_click();
	}
	if(btn_left_status) {
		next_menu--;
		btn_left_status = !btn_left_status;
		sound_click();
	}
	if(btn_middle_status) {

		btn_middle_status = !btn_middle_status;
	}
	
	//lcd_set_cursor(lcd, cursor_row, cursor_col);
}

void lcd_lock_menu_handler(struct sk_lcd *lcd)
{
	char buffer[20];
	bool verification = true;
	if(btn_up_status) {
		switch(cursor_col) {
			case 6:
				if(displayed_pass[0] < 9)
					displayed_pass[0]++;
				break;
			case 7:
				if(displayed_pass[1] < 9)
					displayed_pass[1]++;
				break;
			case 8:
				if(displayed_pass[2] < 9)
					displayed_pass[2]++;
				break;
			case 9:
				if(displayed_pass[3] < 9)
					displayed_pass[3]++;
				break;
		}
		
		print_stars(lcd, cursor_col);

		sound_click();

		btn_up_status = !btn_up_status;
	}
	if(btn_down_status) {
		switch(cursor_col) {
			case 6:
				if(displayed_pass[0] > 0)
					displayed_pass[0]--;
				break;
			case 7:
				if(displayed_pass[1] > 0)
					displayed_pass[1]--;
				break;
			case 8:
				if(displayed_pass[2] > 0)
					displayed_pass[2]--;
				break;
			case 9:
				if(displayed_pass[3] > 0)
					displayed_pass[3]--;
				break;
		}

		print_stars(lcd, cursor_col);

		sound_click();

		btn_down_status = !btn_down_status;
	}
	if(btn_right_status) {
		(cursor_col < 9) ? (cursor_col++) : (cursor_col = 6);

		print_stars(lcd, cursor_col);
		sound_click();

		btn_right_status = !btn_right_status;
	}
	if(btn_left_status) {
		(cursor_col > 6) ? (cursor_col--) : (cursor_col = 9);

		print_stars(lcd, cursor_col);
		sound_click();

		btn_left_status = !btn_left_status;
	}
	if(btn_middle_status) {
		for(uint8_t i = 0; i < 4; i++) {
			if(password[i] != displayed_pass[i])
				verification = false;
		}
		if(verification) {
			set_zeros(displayed_pass, sk_arr_len(displayed_pass));
			next_menu=1;
		}
		else {
			lcd_set_cursor(lcd, 0, 0);
			lcd_send_string(lcd, "WRONG PASSWORD  ");
			set_zeros(displayed_pass, sk_arr_len(displayed_pass));
			//displayed_pass[0] = 0;
			//displayed_pass[1] = 0;
			//displayed_pass[2] = 0;
			//displayed_pass[3] = 0;

			attempts++;
			if(attempts < 4) {
				delay_ms(attempts*1 + 1000);
				lcd_print_menu(lcd, password_menu);
			} else {
				lcd_set_cursor(lcd, 0, 0);
				lcd_send_string(lcd, "   ALARM ALARM  ");
				alarm_on();
			}
		}

		sound_click();

		btn_middle_status = !btn_middle_status;
	}
	
	lcd_set_cursor(lcd, cursor_row, cursor_col);
}
