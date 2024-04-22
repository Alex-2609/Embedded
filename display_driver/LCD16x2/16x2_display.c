#define F_CPU 16000000UL

#include <avr/io.h>

#include "LCD_DISPLAY.h"
#include "i2c.h"


/*
    PCF8574 to LCD Pinout

    P7 -> D7
    P6 -> D6
    P5 -> D5
    P4 -> D4
    P3 -> Backlight control via transistor
    P2 -> Enable
    P1 -> Read/Write
    P0 -> Register select

    The PCF8574 sends the data from MSB to LSB
*/

// I2C of the LCD display
#define LCD_ADDRESS       0x27

// values for the pins
#define PIN_D7            0x80
#define PIN_D6            0x40
#define PIN_D5            0x20
#define PIN_D4            0x10

/**P3 on PCF8574
 * - a logic 0 leaves the NPN transistor base pin open
 * - a logic 1 triggers the base pin of
 *   the NPN transistor an close the circuit
 * 
 * - for more details see shematic
*/
#define BACKLIGHT_ON      0x08 
#define BACKLIGHT_OFF     0x00

#define ENABLE            0x04

/**
 * with write we send commands to the
 * 
 * 
*/
#define WRITE             0x00
#define READ              0x01

#define REGISTER_SELECT   0x01

#define ROWS                2
#define COLUMS             16

#define CURSOR_DEINCREMENT  0
#define CURSOR_INCREMENT    1

#define CURSOR_OFF          0
#define CURSOR_ON           1

#define CURSOR_NO_BLINK     0
#define CURSOR_BLINK        1

#define SHIFT_LEFT          0
#define SHIFT_RIGHT         1

#define NIBBLE_MODE         0
#define BYTE_MODE           1

#define FONT_5x7            0
#define FONT_5x10           1




void init_lcd() {

    uint8_t commands [2] = {
        
        0xFF

    };

    i2c_send_command(LCD_ADDRESS, 0xFF);
   

  
    
}
void turn_on_lcd() {

    i2c_send_command(LCD_ADDRESS, (0xFF & BACKLIGHT_ON));
}

void turn_off_lcd() {
    i2c_send_command(LCD_ADDRESS, (0xFF & BACKLIGHT_OFF));

}





