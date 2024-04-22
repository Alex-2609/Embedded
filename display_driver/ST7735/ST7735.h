#include <stdint.h>


void st7735_init();  
void st7735_reset();

void st7735_send_data(uint8_t *data);
void st7735_send_command(uint8_t data);
void display_on();
void display_off();