#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include "adc.h"


void adc_init() {

    ADMUX = (1<<REFS0);
    ADCSRA = (1<<ADPS2) | (1<<ADPS1) | (1<<ADPS0) | (1<<ADEN);  
    
    
}

int adc_get(uint8_t adc_pin) {

    ADCSRA |= (1<<ADSC);
    ADMUX |= (adc_pin & 0xF);
    
    ADCSRA |= (1<<ADSC);

    while (ADCSRA & (1<<ADSC));

   

    return ADC;
    
}