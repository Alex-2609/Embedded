#include <stdint.h>

/**
 * @implements the master spi mode
*/
void spi_master_init();

/**
 * @implements the slave spi mode
*/
void spi_slave_init();

/**
 * @param data will be send to the slave
 * @param data_length size of data 
*/
void master_transmit(uint8_t *data, uint8_t data_length);

void send_data(uint8_t data);
