/**
 * Set buttons on GLSK pins settings 
 *
 * Note:
 * Also set exti triggers on buttons.
 */
void button_init(void);

extern const sk_pin btn_right;
extern const sk_pin btn_middle;

extern const sk_pin btn_up;
extern const sk_pin btn_down;
extern const sk_pin btn_left;

extern bool btn_up_status;
extern bool btn_down_status;
extern bool btn_right_status;
extern bool btn_left_status;
extern bool btn_middle_status;