#ifndef I2C_H_
#define I2C_H_

#include "types.h"

/** An operation to poll for. */
typedef enum {
	kI2CPollRead,
	kI2CPollWrite,
	kI2CPollReadWrite
} i2c_poll_t;


/** Initialize the I²C in master mode. */
void i2c_init(void);

/** Poll a slave.
 *
 *  @param slave_address The address of the slave to poll.
 *  @param type          The type of the operation to poll for.
 *
 *  @return TRUE if the slave acknowledged the request.
 */
bool_t i2c_poll(uint16_t slave_address, i2c_poll_t type);

/** Send a command to a slave.
 *
 *  @param slave_address The address of the slave to send the command to.
 *  @param command       The command to send.
 *
 *  @return TRUE if the command was successfully sent.
 */
bool_t i2c_send_command(uint16_t slave_address, uint8_t command);

/** Write n bytes of data to a memory address of a slave.
 *
 *  @param slave_address The address of the slave to write to.
 *  @param data_address  The memory address within the slave to write to.
 *  @param n             The number of bytes to write.
 *  @param data          The data to write.
 *
 *  @return The number of bytes successfully written.
 */
uint16_t i2c_write(uint16_t slave_address, uint8_t data_address, uint16_t n, const uint8_t *data);

/** Read n bytes of data from a memory address of a slave.
 *
 *  @param slave_address The address of the slave to read from.
 *  @param data_address  The memory address within the slave to read from.
 *  @param n             The number of bytes to read.
 *  @param data          The read data will be stored here.
 *
 *  @return The number of bytes successfully read.
 */
uint16_t i2c_read(uint16_t slave_address, uint8_t data_address, uint16_t n, uint8_t *data);

/** Write one bytes of data to a memory address of a slave.
 *
 *  @param slave_address The address of the slave to write to.
 *  @param data_address  The memory address within the slave to write to.
 *  @param data          The data to write.
 *
 *  @return TRUE if the byte was successfully written.
 */
bool_t i2c_write_byte(uint16_t slave_address, uint8_t data_address, uint8_t data);

#endif /* I2C_H_ */

