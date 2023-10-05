#ifndef _DHT22_H
#define _DHT22_H

#include <stdint.h>
#include <avr/io.h>

struct dht22_ctx {
	uint8_t bit;
	volatile uint8_t *ddr;
	volatile uint8_t *pin;
	volatile uint8_t *port;
	float rh;
	float temp;
#ifdef _DHT_DEBUG
	uint8_t raw[5];
#endif
};

/**
 * Initialize DHT22 device.
 *
 * @dev: struct dht22_ctx to hold the DHT22 device's settings and data
 * @ddr: specify DDR the DHT22 uses
 * @pin: specify PIN the DHT22 uses
 * @port: specify PORT the DHT22 uses
 * @bit: specify the bit on PIN and PORT which the DHT22 uses
 *
 * Return 0: Success, -1: Failure
 */
int dht22_init(struct dht22_ctx *dev, volatile uint8_t *ddr,
			   volatile uint8_t *pin, volatile uint8_t *port, uint8_t bit);

/**
 * Send request to DHT22 and get the data from it.
 *
 * @dev: specified which DHT22 device
 *
 * Return 0: Success, <0: Failure
 *
 * Once success, the temperature and relative humidity will be written into
 * dev->temp and dev->rh.
 */
int dht22_getdata(struct dht22_ctx *dev);

#endif /* _DHT22_H */
