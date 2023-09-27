#ifndef _DHT22_H
#define _DHT22_H

#include <stdint.h>
#include <avr/io.h>

struct dht22_ctx {
	uint8_t bit;
	volatile uint8_t *ddr;
	volatile uint8_t *pin;
	volatile uint8_t *port;
	uint16_t rh;
	uint16_t temp;
	uint32_t mru_ms;
};

int dht22_init(struct dht22_ctx *dev, volatile uint8_t *ddr,
			   volatile uint8_t *pin, volatile uint8_t *port, uint8_t bit);
int dht22_getdata(struct dht22_ctx *dev);

#endif /* _DHT22_H */
