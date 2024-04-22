#include <stdint.h>

void init_ds1302();
uint8_t get_time(uint8_t* data);
uint8_t set_time(uint8_t hours, uint8_t minutes, uint8_t seconds);
