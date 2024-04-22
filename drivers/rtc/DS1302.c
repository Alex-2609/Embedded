#include <avr/io.h>
#include "DS1302.h"
#include "SPI.h"

#define READ_REGISTERS 7
#define SECONDS 0x81
#define MINUTES 0x83
#define HOUR    0x85
#define DATE    0x87
#define MONTH   0x89
#define DAY     0x8B
#define YEAR    0x8D


uint8_t get_time(uint8_t * data) {


   

}