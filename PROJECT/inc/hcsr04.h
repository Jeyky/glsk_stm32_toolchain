#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


//extern uint32_t counter;

/**
 * Set hcsr04 pin settings and exti settings
 *
 * Note:
 * hcsr04 meassures distance with ultrasonic soundwawes.
 */
void hcsr04_inint(void);

/**
 * set trig pin to 1 wait for 10 microseconds and set pin to 0
 *
 * Note:
 * Waits for interrup while delay.
 */
void hcsr04_strobe_trig(void);

/**
 * strobes trigger and calculate distance from counted time of echo pin
 * @return: calculated distance
 *
 * Note:
 * Result depends on speed_of_sound global variable.
 */
double hcsr04_get_distance(void);

/**
 * Makes 3 meassures and return the lowess value.
 * @return: the lowest value from measured distances
 *
 * Note:
 * Uses hcsr04_get_distance() function
 */
double hcsr04_get_precise_distance(void);

extern const sk_pin trig;
extern const sk_pin echo;

extern double speed_of_sound;