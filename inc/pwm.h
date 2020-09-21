
extern const sk_pin servo;
extern const sk_pin speedometer;
extern const sk_pin alarm;

//extern const sk_pin btn_right;
//extern const sk_pin btn_middle;

//extern bool show_on_speedometer;

extern bool rotate;

//extern double threshold;

void pwm_init(void);

void servo_init(void);

void speedometer_init(void);

void speedometer_set_speed(uint16_t value);

void servo_rotate(uint16_t deg);

void alarm_init(void);

void alarm_on(void);

void alarm_off(void);

//void button_init(void);

void sound_click(void);