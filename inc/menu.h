

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


void lcd_print_menu(struct sk_lcd *lcd, menus menu);

void lcd_btn_menu_handler(struct sk_lcd *lcd);

void lcd_password_menu_handler(struct sk_lcd *lcd);