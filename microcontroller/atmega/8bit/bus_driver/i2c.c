/* i2c - Communication using the I²C interface
 *
 * Written in 2013 by Sven Hesse <drmccoy@drmccoy.de>.
 *
 * To the extent possible under law, the author(s) have dedicated all copyright and related and
 * neighboring rights to this software to the public domain worldwide.
 * This software is distributed without any warranty.
 *
 * You should have received a copy of the CC0 Public Domain Dedication along with this software.
 * If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
 */

#include <avr/io.h>
#define F_CPU 16000000UL
#include <util/delay.h>

#include "i2c.h"

// -- Internal I²C functions --

#define SLAVE_ADDRESS_WRITE(x)  ((x) << 1)
#define SLAVE_ADDRESS_READ(x)  (((x) << 1) | 1)

#define I2C_STATUS_BUS_ERROR      0x00

#define I2C_STATUS_START          0x08
#define I2C_STATUS_RESTART        0x10

#define I2C_STATUS_MT_SLA_ACK     0x18
#define I2C_STATUS_MT_SLA_NACK    0x20
#define I2C_STATUS_MT_DATA_ACK    0x28
#define I2C_STATUS_MT_DATA_NACK   0x30
#define I2C_STATUS_MT_ARB_LOST    0x38

#define I2C_STATUS_MR_SLA_ACK     0x40
#define I2C_STATUS_MR_SLA_NACK    0x48
#define I2C_STATUS_MR_DATA_ACK    0x50
#define I2C_STATUS_MR_DATA_NACK   0x58
#define I2C_STATUS_MR_ARB_LOST    0x38

// Lowest-level register fiddling

// Signal transmission start
static void i2c_set_start() {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
}

// Signal transmission end
static void i2c_set_stop() {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
}

// Completely reset the I²C interface
static void i2c_set_reset() {
	TWCR = (1 << TWINT);
}

// This byte will be sent
static void i2c_set_byte(uint8_t data) {
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
}

// Ready to receive a byte and ACK it
static void i2c_get_byte_ack() {
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA);
}

// Ready to receive a byte and NACK it
static void i2c_get_byte_nack() {
	TWCR = (1 << TWINT) | (1 << TWEN);
}

// Did we get a (N)ACK?
static bool_t i2c_has_status() {
	return TWCR & (1 << TWINT);
}

// Return the (N)ACK state
static uint8_t i2c_get_status() {
	return TWSR & 0xF8;
}

// Higher-level sending

// Signal a transmission start and check if it went through
static bool_t i2c_send_start() {
	i2c_set_start();
	while (!i2c_has_status());

	return (i2c_get_status() == I2C_STATUS_START);
}

// Signal a transmission restart and check if it went through
static bool_t i2c_send_restart() {
	i2c_set_start();
	while (!i2c_has_status());

	return (i2c_get_status() == I2C_STATUS_RESTART);
}

// Signal a transmission stop
static bool_t i2c_send_stop() {
	i2c_set_stop();

	/* Reset the I²C hardware too, for good measure.
	   Yes, that's not really the nicest and cleanest way to go about it,
	   but it makes sure the bus is always in a well-defined state after we
	   stopped transmitting.
	*/
	_delay_us(10);
	i2c_set_reset();

	// We're the master, this can't fail.
	return TRUE;
}

// Send a 8-bit slave address for writing
static bool_t i2c_send_write_address8(uint8_t address) {
	i2c_set_byte(address);
	while (!i2c_has_status());

	return (i2c_get_status() == I2C_STATUS_MT_SLA_ACK);
}

// Send a 8-bit slave address for reading
static bool_t i2c_send_read_address8(uint8_t address) {
	i2c_set_byte(address);
	while (!i2c_has_status());

	return (i2c_get_status() == I2C_STATUS_MR_SLA_ACK);
}

// Send a 16-bit slave address for writing
static bool_t i2c_send_write_address16(uint16_t address) {
	if ((address & 0xF000) == 0xF000)
		if (!i2c_send_write_address8(address >> 8))
			return FALSE;

	return i2c_send_write_address8(address & 0x00FF);
}

// Send a 16-bit slave address for reading
static bool_t i2c_send_read_address16(uint16_t address) {
	if ((address & 0xF000) == 0xF000)
		if (!i2c_send_read_address8(address >> 8))
			return FALSE;

	return i2c_send_read_address8(address & 0x00FF);
}

// Send a byte of data to a slave
static bool_t i2c_send_data(uint8_t data) {
	i2c_set_byte(data);
	while (!i2c_has_status());

	return (i2c_get_status() == I2C_STATUS_MT_DATA_ACK);
}

// Higher-level receiving

// Read a byte of data from the slave and ACK it
static bool_t i2c_receive_byte_ack(uint8_t *data) {
	i2c_get_byte_ack();
	while (!i2c_has_status());

	if (i2c_get_status() != I2C_STATUS_MR_DATA_ACK)
		return FALSE;

	*data = TWDR;
	return TRUE;
}

// Read a byte of data from the slave and NACK it
static bool_t i2c_receive_byte_nack(uint8_t *data) {
	i2c_get_byte_nack();
	while (!i2c_has_status());

	if (i2c_get_status() != I2C_STATUS_MR_DATA_NACK)
		return FALSE;

	*data = TWDR;
	return TRUE;
}

// Read a byte of data from a slave and (N)ACK it
static bool_t i2c_receive_byte(uint8_t *data, bool_t ack) {
	return ack ? i2c_receive_byte_ack(data) : i2c_receive_byte_nack(data);
}

// -- Public I²C functions --

void i2c_init(void) {
	/** Set the bit-rate and clock prescaler.
	 *
	 * Those don't have to be exact; ball-park is enough.
	 * If transmission is going too fast for a slave device, it will hold the clock line low and
	 * slow transmission down this way.
	 */
	TWBR  = 0x48;
	TWSR  &= 0xFC;

	i2c_set_reset();
}

bool_t i2c_poll(uint16_t slave_address, i2c_poll_t type) {
	if (type == kI2CPollReadWrite)
		return i2c_poll(slave_address, kI2CPollRead) && i2c_poll(slave_address, kI2CPollWrite);

	if (!i2c_send_start()) {
		i2c_send_stop();
		return FALSE;
	}

	bool_t result;
	if (type == kI2CPollWrite)
		result = i2c_send_write_address16(SLAVE_ADDRESS_WRITE(slave_address));
	else
		result = i2c_send_read_address16(SLAVE_ADDRESS_READ(slave_address));

	i2c_send_stop();
	return result;
}

bool_t i2c_send_command(uint16_t slave_address, uint8_t command) {
	if(!i2c_send_start()) {
		i2c_send_stop();
		return FALSE;
	}

	if(!i2c_send_write_address16(SLAVE_ADDRESS_WRITE(slave_address))) {
		i2c_send_stop();
		return FALSE;
	}

	if(!i2c_send_data(command)) {
		i2c_send_stop();
		return FALSE;
	}

	i2c_send_stop();
	return TRUE;
}

uint16_t i2c_write(uint16_t slave_address, uint8_t data_address, uint16_t n, const uint8_t *data) {
	if(!i2c_send_start()) {
		i2c_send_stop();
		return 0;
	}

	if(!i2c_send_write_address16(SLAVE_ADDRESS_WRITE(slave_address))) {
		i2c_send_stop();
		return 0;
	}

	if(!i2c_send_data(data_address)) {
		i2c_send_stop();
		return 0;
	}

	uint16_t processed = 0;
	while (n-- > 0) {
		if(!i2c_send_data(*data++))
			break;

		processed++;
	}

	i2c_send_stop();
	return processed;
}

bool_t i2c_write_byte(uint16_t slave_address, uint8_t data_address, uint8_t data) {
	return i2c_write(slave_address, data_address, 1, &data) == 1;
}

uint16_t i2c_read(uint16_t slave_address, uint8_t data_address, uint16_t n, uint8_t *data) {
	if (n == 0)
		return 0;

	if(!i2c_send_start()) {
		i2c_send_stop();
		return 0;
	}

	if(!i2c_send_write_address16(SLAVE_ADDRESS_WRITE(slave_address))) {
		i2c_send_stop();
		return 0;
	}

	if(!i2c_send_data(data_address)) {
		i2c_send_stop();
		return 0;
	}

	if(!i2c_send_restart()) {
		i2c_send_stop();
		return 0;
	}

	if(!i2c_send_read_address16(SLAVE_ADDRESS_READ(slave_address))) {
		i2c_send_stop();
		return 0;
	}

	uint16_t processed = 0;
	while (n-- > 0) {
		if(!i2c_receive_byte(data++, n != 0))
			break;

		processed++;
	}


	i2c_send_stop();
	return processed;
}

