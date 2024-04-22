#include <avr/io.h>
#define F_CPU 16000000UL

#include "i2c.h"

#define HTU_ADDRESS 0x40

/**
 * TEMP_MHOLD for temperature measurement while holding master
 * HUM_MHOLd for humidity measurement while holding master
*/
#define TEMP_MHOLD  0xE3
#define HUM_MHOLD   0xE5


float get_temp(uint8_t *data) {

    i2c_read(HTU_ADDRESS, TEMP_MHOLD, 2, data);
    uint16_t temp = (data[0]<<8 | data[1]);
    temp &= 0xFFFC;
    float temperature = -47.85f + 172.72f * (temp/ (float) 65535);
    
    return temperature;   
}

float get_hum(uint8_t *data) {

    i2c_read(HTU_ADDRESS, HUM_MHOLD, 2, data);
    uint16_t hum = (data[0]<<8 | data[1]);
    hum &= 0xFFFC;
    float humidity = -6 + 125 * (hum / (float) 65535);

    return humidity;


}