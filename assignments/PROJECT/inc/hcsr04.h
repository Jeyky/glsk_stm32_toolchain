#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>


//extern uint32_t counter;

void hcsr04_inint(void);

void hcsr04_strobe_trig(void);

double hcsr04_get_distance(void);

double hcsr04_get_precise_distance(void);

extern const sk_pin trig;
extern const sk_pin echo;

extern double speed_of_sound;