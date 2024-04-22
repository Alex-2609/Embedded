#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "SPI.h"


#define ST7735_DDR      DDRB
#define ST7735_PORT     PORTB
#define ST7735_RS       PINB4
#define ST7735_RESET    PINB1  

#define NO_OPERATION    0x00
#define SW_RESET        0x01

#define DISPLAY_INVERSION_ON    0x20
#define DISPLAY_INVERSION_OFF   0x21
#define DISPLAY_OFF             0x28
#define DISPLAY_ON              0x29

void st7735_init() {

    ST7735_DDR = (1<<ST7735_RS) | (1<<ST7735_RESET);

}

void st7735_reset() {

    ST7735_PORT &= ~(1<<ST7735_RESET);
    ST7735_PORT &= ~(1<<ST7735_RS);

    send_data(0x03);
    _delay_ms(120);
}

void st7735_send_command(uint8_t data) {

    ST7735_PORT &= ~(1<<ST7735_RS);

    send_data(data);    
}

void st7735_send_data(uint8_t *data) {

    ST7735_PORT |= (1<<ST7735_RS);

    send_data(data);

}

void display_on() {

    st7735_send_command(DISPLAY_ON);

}

void display_off() {

    st7735_send_command(DISPLAY_INVERSION_OFF);
}