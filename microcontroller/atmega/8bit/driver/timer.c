#define F_CPU 16000000UL

#include <avr/io.h>
#include <avr/interrupt.h>

#include "timer.h"

void init_timer0(double time_us) {

    TCCR0A = 0;
    TCCR0B = 0;
    TCNT0 = 0;

    TCCR0A |= (1<<WGM01);

    OCR0A = (uint8_t) ((F_CPU / 8) * time_us / 1000000 ) - 1;

    TCCR0B |= (1<<CS01); /*Setting Prescaler to 8*/

    TIMSK0 |= (1<<OCIE0A);
    

}


void init_timer1(double time_ms) {

    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    if(time_ms <= 250) {
        OCR1A = (uint16_t) ((F_CPU / 64) * time_ms / 1000) - 1; /*Formula calculate value for OCR1A to achieve the interrupt in ms*/
        TCCR1B |= (1<<CS11) | (1<<CS10) | (1<<WGM12); /*Setting Prescaler to 64 and enable CTC*/
    } else {
        OCR1A = (uint16_t) ((F_CPU / 256) * time_ms / 1000) - 1; 
        TCCR1B |= (1<<CS12) | (1<<WGM12); /*Setting Prescaler to 256 and enable CTC*/
    }

    TIMSK1 |= (1<<OCIE1A);
    sei();

}

void init_pwm() {

    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    OCR1A = 1249;

    TCCR1A |= (1<<COM1A0) | (1<<WGM10) | (1<<WGM11);
    TCCR1B |= (1<<WGM13) | (1<<WGM12) | (1<<CS12);


    TIMSK1 |= (1<<OCIE1A);
}

void pwm(uint8_t degree) {

    TCCR1A = (1<<COM1A0) | (1<<WGM10) | (1<<WGM11) | (1<<WGM12) | (1<<WGM13);
    TCCR1B = (1<<CS11) | (1<<CS10);
    OCR1A = 14999;
    TCNT1 = 0;

    TIMSK1 = (1<<OCIE1A);

    sei();
    
}