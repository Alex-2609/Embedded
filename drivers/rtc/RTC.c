#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

#include "RTC.h"
#include "types.h"
#include "i2c.h"

#define ADDRESS         0x51
#define RTC_WRITE       0xA2
#define RTC_READ        0xA3


/**
 * Addresses of the different register of the PCF8563 
 */

#define CONTROL_STATUS1 0x00
#define CONTROL_STATUS2 0x01

#define TIME_REGISTER      7
#define VL_SECONDS      0x02
#define MINUTES         0x03
#define HOURS           0x04
#define DAYS            0x05
#define WEEKDAYS        0x06
#define MONTHS          0x07
#define YEARS           0x08

#define ALARM_REGISTER     6
#define ENABLE_ALARM    0x7F
#define DISABLE_ALARM   0x80
#define MINUTES_ALARM   0x09
#define HOUR_ALARM      0x0A
#define DAY_ALARM       0x0B
#define WEEKDAY_ALARM   0x0C

#define CLKOUT_CONTROL  0x0D
#define TIMER_CONTROL   0x0E
#define TIMER           0x0F

#define TEN_PLACE         10

/**
 * 
 * @param data is converted to values ten place and unit place
 * @return the data that are written in the register of the PCF8563
 * 
 */
static uint8_t data_to_rtc(uint8_t data) {
    
    uint8_t ten_place = data / 10;
    uint8_t unit_place = data % 10;
    return (ten_place<<4) | (unit_place);
   
}



static uint8_t convert_hours_n_days(uint8_t *data) {
    
    for(int i= 2; i < 4; ++i){
        
        uint8_t ten_place = ((data[i]>>4) & 0x03) * TEN_PLACE;
        uint8_t unit_place = data[i] & 0x0F;
        data[i] = ten_place + unit_place;
       
    }  
    
     return data;
    
    
}
 static uint8_t convert_seconds_n_minutes(uint8_t *data) {
    
     for(int  i = 0; i < 2; ++i){
         
         uint8_t ten_place = (data[i]>>4) * TEN_PLACE;
         uint8_t unit_place = data[i] & 0x0F;
         data[i] = ten_place + unit_place; 
     }
     
     return data;
}
 
 static uint8_t get_month(uint8_t *data) { 
     
     
     //workaround needed to be fixed. Sometimes it gaves the value 35 for Month 3 (March) back
     //Logic Analyzer is needed for debugging
    data[5] &= 0x1F; 
    
    uint8_t ten_place = (data[5]>>4) * TEN_PLACE;
    uint8_t unit_place = data[5] & 0x0F;
    return  ten_place + unit_place;
     
 }


void get_time(uint8_t *data) {
    i2c_read(ADDRESS, VL_SECONDS, 7, data);
    convert_seconds_n_minutes(data);
    convert_hours_n_days(data);
      
}

void set_time(uint8_t hours, uint8_t minute, uint8_t seconds) {  
    
    /**
     * 
     * @param hours
     * @param minute
     * @param seconds if >= 0 this value is written to the register
     * 
     */
    if(seconds >= 0) {
        i2c_write_byte(ADDRESS, VL_SECONDS, data_to_rtc(seconds));
        i2c_write_byte(ADDRESS, HOURS, data_to_rtc(hours));
        i2c_write_byte(ADDRESS, MINUTES, data_to_rtc(minute));
        
        
    } else { //otherwise the register left untouched to avoid problems                                    
        i2c_write_byte(ADDRESS, HOURS, data_to_rtc(hours));
        i2c_write_byte(ADDRESS, MINUTES, data_to_rtc(minute));
    }    
}

/**
 * 
 * @param day
 * @param month
 * @param year
 * values are written to the register of the pfc8563
 */
void set_date(uint8_t day, uint8_t month, uint8_t year) {
    
    i2c_write_byte(ADDRESS, DAYS, data_to_rtc(day));
    i2c_write_byte(ADDRESS, MONTHS, data_to_rtc(month));
    i2c_write_byte(ADDRESS, YEARS, year);
}

void set_alarm(uint8_t hour, uint8_t minute) {
   
    i2c_write_byte(ADDRESS, MINUTES_ALARM, data_to_rtc(hour));
    i2c_write_byte(ADDRESS, HOUR_ALARM, data_to_rtc(minute));
}

void alarm_off() {
    
    i2c_write(ADDRESS, MINUTES_ALARM, 4, DISABLE_ALARM);
    
}

bool_t alarm_interrupt() {
    
    
    uint8_t alarm_state = 0;
    i2c_read(ADDRESS, CONTROL_STATUS2, 1, &alarm_state);
    
    printf("%d", alarm_state);
    
    if((alarm_state &= 0x08) == 0x08) {
        return TRUE;
    } else {
        return FALSE;
    }     
}