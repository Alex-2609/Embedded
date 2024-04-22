#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>

#define F_CPU 16000000UL

#define TRIGGER_VALUE 19
#define DELAY_VALUE 12499


ISR(TIMER0_COMPA_vect) {

    PORTB = 0x00;

}


ISR(TIMER1_COMPA_vect){

    
    TCCR0A = 0;
    TCCR0B = 0;
    TCNT0 = 0;

    TCCR0A = (1<<WGM01); /*enable CTC*/
    TCCR0B |= (1<<CS01); /*setting Prescaler to 2Mhz*/


    OCR0A = TRIGGER_VALUE; /*Value to trigger the pin of the SR04 every 10us*/
    
    TIMSK0 |= (1<<OCIE0A);

    PORTB = 0x01;
   
   
}




void start_sr04(void){


    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    OCR1A = DELAY_VALUE;

    TCCR1B |= (1<<CS11) | (1<<CS10) | (1<<WGM12);

    TIMSK1 |= (1<<OCIE1A);

    

    sei();


    
    
}

