
extern const sk_pin servo;
extern const sk_pin speedometer;
extern const sk_pin alarm;

extern const sk_pin btn_right;
extern const sk_pin btn_middle;

extern bool show_on_speedometer;

void pwm_init(void);

void servo_init(void);

void speedometer_init(void);

void speedometer_set_speed(uint16_t value);

void servo_rotate(uint16_t deg);

void alarm_init(void);

void button_init(void);