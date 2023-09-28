#include <avr/interrupt.h>
#include <util/delay.h>
#include "dht22.h"

#define MAX_LOOP 100

#define DECLARE_WAIT_FOR_CHANGE(number, min_us, max_loop)			\
static inline int wait_for_change ## number(struct dht22_ctx *dev,	\
											uint8_t expected)		\
{																	\
	*dev->ddr &= ~_BV(dev->bit);									\
	_delay_us(min_us);												\
	for (int i = 0; i < max_loop; ++i) {							\
		if (expected == (*dev->pin & _BV(dev->bit)))				\
			return 0;	/* get expected */							\
	}																\
	return -1;			/* timeout */								\
}

static int check_pullup(struct dht22_ctx *dev)
{
	int ret = -1;

	*dev->ddr &= ~_BV(dev->bit);
	if (1 == (*dev->pin & _BV(dev->bit)))
		ret = 0;

	return ret;
}

static int send_request(struct dht22_ctx *dev)
{
	if (check_pullup(dev) < 0)
		return -1;
	_delay_ms(50);

	/* set low(1ms) */
	*dev->ddr |= _BV(dev->bit);
	*dev->port &= ~_BV(dev->bit);
	_delay_ms(1.1);

	/* set high(20us) */
	*dev->port |= _BV(dev->bit);
	return 0;
}

int dht22_init(struct dht22_ctx *dev, volatile uint8_t *ddr,
			   volatile uint8_t *pin, volatile uint8_t *port, uint8_t bit)
{
	dev->rh = 0.f;
	dev->temp = 0.f;
	dev->ddr = ddr;
	dev->pin = pin;
	dev->port = port;
	dev->bit = bit;

	/* set high */
	*dev->ddr |= _BV(dev->bit);
	*dev->port |= _BV(dev->bit);

	/* confirm pin already pull-up */
	return check_pullup(dev);
}

DECLARE_WAIT_FOR_CHANGE(0, 20, MAX_LOOP);
DECLARE_WAIT_FOR_CHANGE(1, 70, MAX_LOOP);
DECLARE_WAIT_FOR_CHANGE(2, 40, MAX_LOOP);
DECLARE_WAIT_FOR_CHANGE(3, 30, 1);
DECLARE_WAIT_FOR_CHANGE(4, 10, MAX_LOOP);
int dht22_getdata(struct dht22_ctx *dev)
{
	int ret = 0;
	uint8_t data[5] = { 0 };

	/* send request: low(1ms) -> high(20-40us) */
	if ((ret = send_request(dev)) < 0)
		return ret;

	/* check dht22 ACK: low(80us) */
	if (wait_for_change0(dev, 0) < 0)
		return -2;

	/* check dht22 ACK: high(80us) */
	if (wait_for_change1(dev, 1) < 0)
		return -3;

	if (wait_for_change1(dev, 0) < 0)
		return -4;

	/* receive data, from MSB to LSB */
	for (int i = 0; i < 5; ++i) {
		for (int b = 7; b >= 0; --b) {
			/* low(50us) */
			if (wait_for_change2(dev, 1) < 0)
				return -5;

			/* 0 if high(26-28us)
			 * 1 if high(70us) */
			if (wait_for_change3(dev, 0) < 0) {
				data[i] |= _BV(b);
				if (wait_for_change4(dev, 0) < 0)
					return -6;
			}
		}
	}

	/* cksum */
	if ((data[0] + data[1] + data[2] + data[3]) != data[4])
		return -7;

	/* translate the data */
	dev->rh = .1f * (data[0] << 8 | data[1]);
	dev->temp = .1f * ((data[2] & 0x7f) << 8 | data[3]);
	if (data[2] & 0x80)
		dev->temp = -dev->temp;

	return 0;
}
