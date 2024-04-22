#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

void adc_init();
int adc_get(uint8_t adc_pin);
