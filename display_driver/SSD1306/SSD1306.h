#include <stdint.h>

void init_ssd1306();
void clear_display();
void print_character(unsigned char character);
void print_text(char *text);
void reset_cursor();
void set_cursor(uint8_t page, uint8_t column);
void rearrangement_check();