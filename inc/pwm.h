
extern const sk_pin servo;
extern const sk_pin speedometer;
extern const sk_pin alarm;

//extern const sk_pin btn_right;
//extern const sk_pin btn_middle;

//extern bool show_on_speedometer;

extern bool rotate;

//extern double threshold;

/**
 * Inits PWM output on certain channel
 * @tim: Timer register address base
 * @rcc_tim: rcc_periph_clken timer RCC
 * @oc: enum tim_oc_id OC channel designators TIM_OCx where x=1..4, TIM_OCxN where x=1..3 (no action taken)
 * @prescaler: Prescaler values 0...0xFFFF.
 * @period: Period in counter clock ticks.
 *
 * Note:
 * This function allows to init only one PW< channel at once
 */
//void pwm_init(void);
void pwm_init(uint32_t tim, enum rcc_periph_clken rcc_tim, enum tim_oc_id oc, uint32_t prescaler, uint32_t period);

/**
 * Setup servo motor PWM and pin settings 
 *
 * Note:
 * No ability to change pins and timers, pins and timers are chosen strictly.
 */
void servo_init(void);

/**
 * Setup speedometer PWM and pin settings 
 *
 * Note:
 * No ability to change pins and timers, pins and timers are chosen strictly.
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
 * Setup alarm PWM and pin settings 
 *
 * Note:
 * No ability to change pins and timers, pins and timers are chosen strictly.
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
 * Uses when buttons are clicked.
 */
void sound_click(void);