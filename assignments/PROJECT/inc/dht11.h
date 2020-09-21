#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

void dht11_start(void);

bool dht11_response(void);

uint8_t dht11_read_byte(void);

uint8_t dht11_read_temprature(void);

uint8_t dht11_read_humidity(void);

extern const sk_pin dht11;