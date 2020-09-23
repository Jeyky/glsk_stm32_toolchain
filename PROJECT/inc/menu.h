

enum menus {
	password_menu = 0,
	info_menu = 1,
	dashboard_menu = 2,
	rotation_menu = 3,
	dialog_menu = 4,
	lock_menu = 5
};

typedef enum menus menus;

extern menus next_menu;

extern menus current_menu;

extern uint8_t password[4];

extern uint8_t displayed_pass[4];

extern uint8_t attempts;

extern double sound_speed;

extern uint8_t threshold;

extern bool dashboard_status;

extern bool rotation_status;

/**
 * Prints menu on display
 * @lcd: lcd where to print menu
 * @menu: enum of menus.
 *
 * Note:
 * Has certain number of values to print
 */
void lcd_print_menu(struct sk_lcd *lcd, menus menu);

void lcd_btn_menu_handler(struct sk_lcd *lcd);

/**
 * Gives ability to use buttons in password menu
 * @lcd: lcd where to move cursor
 *
 * Note:
 * We can only change password and try to verify it. We use all buttons.
 * Up and low buttons increase or decrease value of PIN CODE.
 * Right and left buttons moves our cursor in right and left.
 * Middle button verifies password if is correct.
 */
void lcd_password_menu_handler(struct sk_lcd *lcd);

/**
 * Gives ability to use buttons in info menu
 * @lcd: lcd where to move cursor
 *
 * Note:
 * We can only change threshold value.
 * Up and low buttons increase or decrease value of threshold of security system.
 * Right button switches to next menu.
 */
void lcd_info_menu_handler(struct sk_lcd *lcd);

/**
 * Gives ability to use buttons in dashboard. menu
 * @lcd: lcd where to move cursor
 *
 * Note:
 * Up, down and middle buttons change status of showing distance on dashboard speedometer.
 * Left and right buttons swithces between menus.
 */
void lcd_dashboard_menu_handler(struct sk_lcd *lcd);

/**
 * Gives ability to use buttons in rotation menu
 * @lcd: lcd where to move cursor
 *
 * Note:
 * Up, down and middle buttons change status of rotation ability of servo motor.
 * Left and right buttons swithces between menus.
 */
void lcd_rotation_menu_handler(struct sk_lcd *lcd);

/**
 * Gives ability to use buttons in dialog menu
 * @lcd: lcd where to move cursor
 *
 * Note:
 * Left and right buttons swithces between menus.
 */
void lcd_dialog_menu_handler(struct sk_lcd *lcd);

/**
 * Gives ability to use buttons in password menu
 * @lcd: lcd where to move cursor
 *
 * Note:
 * We can only change password and try to verify it. We use all buttons.
 * Up and low buttons increase or decrease value of PIN CODE.
 * Right and left buttons moves our cursor in right and left.
 * Middle button verifies password if is correct.
 */
void lcd_lock_menu_handler(struct sk_lcd *lcd);

/**
 * Not cursor password digits are shown like stars 
 * @lcd: desired lcd to change password values
 * Note:
 * We use it in lock and password menus.
 */
void print_stars(struct sk_lcd *lcd, uint8_t pos);