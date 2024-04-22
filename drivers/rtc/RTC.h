/* 
 * File:   RTC.h
 * Author: Alex
 * 
 * todo:
 * 
 *
 * Created on 29. Februar 2024, 22:06
 */

#ifndef RTC_H
#define	RTC_H

#include <stdint.h>
#include "types.h"


/**
 * @param data stores the data from the PFC8563
 */
void get_time(uint8_t *data);

/**
 * 
 * @param data same as above
 */
void get_date(uint8_t *data);

/**
 * 
 * @param hours 
 * @param minutes
 * @param seconds
 * 
 * are written to the register to the pcf8563
 */
void set_time(uint8_t hours, uint8_t minutes, uint8_t seconds);
/**
 * 
 * @param day
 * @param month
 * @param year
 * 
 * same as above
 */
void set_date(uint8_t day, uint8_t month, uint8_t year);

/**
 * 
 * @param hour
 * @param minute
 * 
 * values that are written in the register of the pcf8563
 */
void set_alarm(uint8_t hour, uint8_t minute);

/*
 * checks the associated register if the alarm flag is set
 */
bool_t alarm_interrupt();

/**
 * disable the alarm
 */
void alarm_off();

#endif	/* RTC_H */ 

