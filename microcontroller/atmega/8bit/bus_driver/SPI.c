#define F_CPU 16000000UL

#include <avr/io.h>
#include "SPI.h"

#define SPI_DDR   DDRB
#define SPI_PORT PORTB
#define CS       PINB2
#define MOSI     PINB3
#define MISO     PINB4
#define CLOCK    PINB5

void spi_master_init() {

    SPI_DDR = (1<<CS) | (1<<MOSI) | (1<<CLOCK);
    SPCR = (1<<SPE) | (1<<MSTR) | (1<<SPI2X);

}

void send_data(uint8_t data) {
    SPI_PORT &= ~(1<<CS);
    SPDR = data;

    while(!(SPSR & (1<<SPIF)));

    SPI_PORT |= (1<<CS);
}

void master_transmit(uint8_t *data, uint8_t data_length) {

    for(int i = 0; i < data_length; ++i) {
        send_data(data[i]);
    }
}
