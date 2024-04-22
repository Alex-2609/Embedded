#define F_CPU 16000000UL

#include <avr/io.h>
#include <stdint.h>
#include <string.h>

#include "SSD1306.h"
#include "fonts.h"

#include "i2c.h"

static uint8_t counter = 0;

/*
Basic OLED Parameters
*/
#define LCD_WIDTH              128
#define LCD_HEIGTH             64
#define CHAR_WIDTH             5
#define PAGE_CHARACTER_LENGTH  25
#define NUMBER_OF_PAGES        8


/**
 * Display Setup
 * @param IC2_ADDRESS address of the slave
 * @param CONTROL_BYTE following byte(s) is interpreted as command
 * @param DATA_BYTE following bytes(s) are interpreted as data
*/
#define I2C_ADDRESS         0x3C
#define CONTROL_BYTE        0x00
#define DATA_BYTE           0x40


#define SPACE                0x20
#define SPACE_FONT           0x00

#define ENTER_CONTRAST_MODE  0x81
#define RESET_CONTRAST       0x7F


/**
 * Fundamental commands 
*/

#define DISPLAY_RESET             0xA4
#define ENTIRE_DISPLAY_ON         0xA5
#define NORMAL_DISPLAY            0xA6
#define INVERSE_DISPLAY           0xA7
#define DISPLAY_OFF               0xAE
#define DISPLAY_ON                0xAF

/**
 * Scrolling commands
*/

#define RIGHT_HORIZONTAL_SCROLL   0x26
#define LEFT_HORIZONTAL_SCROLL    0x27
#define START_PAGE_ADDRESS (page)  ((page) & 0x07)
#define END_PAGE_ADDRESS (page)    ((page) & 0x07)



/**
 * Addressing commands
*/

#define SET_MEMORY                0x20
#define HORIZONTAL_MODE           0x00
#define VERTICAL_MODE             0x01
#define PAGE_ADDRESSING_MODE      0x02

/**
 * Hardware configuration
*/

#define SET_SEGMENT_REMAP0        0xA0
#define SET_SEGMENT_REMAP1        0xA1
#define SET_OUTPUT_SCAN_DIRECTION 0xC0

#define CHARGE_PUMP_SETTING       0x8D
#define ENABLE_CHARGE_PUMP        0x14

#define SET_COLUMN_ADDRESS        0x21
#define COLUMN_START_ADDRESS      0x00
#define COLUMN_END_ADDRESS        0x7F

#define SET_PAGE_ADDRESS          0x22
#define PAGE_START_ADDRESS        0x00
#define PAGE_END_ADDRESS          0x07

static uint8_t character_counter = 0;
static uint8_t column_cursor = 0;
static uint8_t page_cursor = 0;

void init_ssd1306() {

      uint8_t commands [] = {

        DISPLAY_OFF,
        SET_MEMORY,
        HORIZONTAL_MODE,
        SET_SEGMENT_REMAP0,
        SET_OUTPUT_SCAN_DIRECTION,
        CHARGE_PUMP_SETTING,
        ENABLE_CHARGE_PUMP,
        NORMAL_DISPLAY,
        DISPLAY_ON,
    };
    i2c_write(I2C_ADDRESS, CONTROL_BYTE, sizeof(commands), commands);
 
}

void print_character(unsigned char character) {

  uint8_t data[5]; 

  if(character < 32 || character > 127 ) {
    return;
  }

  for(int i=0; i < CHAR_WIDTH; ++i){
    data[i] = font[character][i];
  }
  
  i2c_write(I2C_ADDRESS, DATA_BYTE, CHAR_WIDTH, data);
  character_counter++;
  
}

void clear_display() {

  uint8_t clear [5] = {0x00, 0x00, 0x00, 0x00, 0x00};

  reset_cursor();

  for(int i=0; i < 205; ++i){
    i2c_write(I2C_ADDRESS, DATA_BYTE,CHAR_WIDTH, clear);
  }
   reset_cursor();
}

void print_text(char *text) {

  for(int i=0; i < strlen(text); ++i){
      if(text[i] == SPACE) { /*check for space in text*/
        print_character(SPACE_FONT);
      }
      print_character(text[i]);
  }
}
    

void reset_cursor() {

  uint8_t commands[] = {
      SET_COLUMN_ADDRESS,
      COLUMN_START_ADDRESS,
      COLUMN_END_ADDRESS,
      SET_PAGE_ADDRESS,
      PAGE_START_ADDRESS,
      PAGE_END_ADDRESS
  };
  i2c_write(I2C_ADDRESS, CONTROL_BYTE, sizeof(commands), commands);
}

void set_cursor(uint8_t page, uint8_t column) {

  page_cursor = page;
  column_cursor = column;

  uint8_t commands [6] = {
      SET_COLUMN_ADDRESS,
      column_cursor,
      COLUMN_END_ADDRESS,
      SET_PAGE_ADDRESS,
      page_cursor,
      PAGE_END_ADDRESS
  };
  i2c_write(I2C_ADDRESS, CONTROL_BYTE, sizeof(commands), commands);
}

void rearrangement_check() {

  if(character_counter == PAGE_CHARACTER_LENGTH) {
    character_counter = 0;
    column_cursor = 0;
    page_cursor++;

    if(page_cursor == NUMBER_OF_PAGES) {
      page_cursor = 0;
    }
    set_cursor(page_cursor, column_cursor);
  }
}


