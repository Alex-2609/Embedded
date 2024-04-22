#include <stdint.h>

/**
 * @param start_position sets the start position of the servo 
 * @param end_position sets the end position of the servo
*/
void init_servo(float start_position, float end_position);

/**
 * @param angle sets the servo to this angle
*/
void set_angle(float angle_to_set);

/**
 * sets the servo to the start position
*/
void set_start_position();

/**
 * sets the servo to the end position
*/
void set_end_position();

/**
 * sets the servo to the middle position (endposition/2)
*/
void set_mid_position();
