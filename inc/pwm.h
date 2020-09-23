
extern const sk_pin servo;
extern const sk_pin speedometer;
extern const sk_pin alarm;

//extern const sk_pin btn_right;
//extern const sk_pin btn_middle;

//extern bool show_on_speedometer;

extern bool rotate;

//extern double threshold;

/**
 * set pwm settings for speedometer, speaker and servo motor 
 *
 * Note:
 * Uses 3 timers(TIM3, TIM5, TIM9) for different purposes and we cannot have the same settings for all this stuff.
 * Servo frequensy is 50Hz and duty cycle from 0.5 to 2.5 milliseconds. Alarm frequency
 * is 1kHz. Dashboard speedometer frequency depends on desired value on scale (6.7Hz per 1kmph).
 */
void pwm_init(void);
/**
 * Set servo motor pin settings 
 *
 * Note:
 * Set output options and alternative functions for servo.
 */
void servo_init(void);

/**
 * Set dashboard speedometer pin settings 
 *
 * Note:
 * Set output options and alternative functions for speedometer
 */
void speedometer_init(void);

/**
 * Sends pwm setting for speedometer
 * @value: desired value on scale of speedometer
 *
 * Note:
 * Uses formula to change pwm frequency output. Changes prescaler, ARR does not change.
 */
void speedometer_set_speed(uint16_t value);

/**
 * Rotates servo motor in desired position
 * @deg: degree position where to set servo motor.
 *
 * Note:
 * changes duty cycle in range of 0.5ms to 2.5ms(from -90 degrees to 90 degrees).
 */
void servo_rotate(uint16_t deg);

/**
 * Set alarm speaker pin settings
 *
 * Note:
 * Set output options and alternative functions for alarm speaker
 */
void alarm_init(void);

/**
 * Turns speaker on by sending PWM signal
 *
 * Note:
 * Signal lasts some time. Frequency 1kHz.
 */
void alarm_on(void);

/**
 * Turns off PWM signal that comes on alarm speaker.
 */
void alarm_off(void);

/**
 * sends pwm signal on speaker for 20 milliseconds.
 *
 * Note:
 * Useswhen buttons are clicked.
 */
void sound_click(void);