#define F_CPU 16000000UL
#define PRESCALER 64
#define MS_TO_SEC 1000

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#include "Servo.h"





ISR(TIMER1_COMPA_vect) {

    PORTB ^= 0x01;


}



void init_servo(float start, float end){

    start_position = start;
    end_position = end;
    mid_position = end_position / 2;
}


void set_angle(float angle_to_set) {

    angle = (float) (1 + (angle_to_set / end_position));

    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    TCCR1B |= (1<<WGM12) | (1<<CS11) | (1<<CS10);
    TIMSK1 |= (1<<OCIE1A);

    OCR1A = (uint16_t) ((F_CPU / 64) * angle / 1000); 

    sei();
   
}

void set_start_position(){
    set_angle(start_position);
}

void set_end_position(){
    set_angle(end_position);
}

void set_mid_position(){
    set_angle(mid_position);
}
