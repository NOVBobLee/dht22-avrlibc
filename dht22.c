#include <util/delay.h>
#include "dht22.h"

#define TIMEOUT_LOOPS 50

/* use loop for relaxing the delay us,ms */
static inline int wait_for_change(struct dht22_ctx *dev, int reps,
								  uint8_t expected)
{
	for (int i = 0; i < reps; ++i)
		if (expected == (*dev->pin & _BV(dev->bit)))
			return 0;	/* get expected */
	return -1;			/* timeout */
}

static int check_pullup(struct dht22_ctx *dev)
{
	*dev->ddr &= ~_BV(dev->bit);

	if (1 == (*dev->pin & _BV(dev->bit)))
		return 0;
	return -1;
}

static inline void send_request(struct dht22_ctx *dev)
{
	*dev->ddr |= _BV(dev->bit);

	/* set low(>1ms) */
	*dev->port &= ~_BV(dev->bit);
	_delay_ms(1.1);

	/* set high(20-40us) */
	*dev->port |= _BV(dev->bit);
	_delay_us(20);
}

static inline int confirm_ack(struct dht22_ctx *dev)
{
	*dev->ddr &= ~_BV(dev->bit);

	/* confirm low(80us) */
	if (wait_for_change(dev, TIMEOUT_LOOPS, 0) < 0)
		return -1;

	_delay_us(75);

	/* confirm: high(80us) */
	if (wait_for_change(dev, TIMEOUT_LOOPS, 1) < 0)
		return -1;

	_delay_us(75);
	return 0;
}

static inline void parse_data(struct dht22_ctx *dev, uint8_t *data)
{
	dev->rh = .1f * (data[0] << 8 | data[1]);
	dev->temp = .1f * ((data[2] & 0x7f) << 8 | data[3]);
	if (data[2] & 0x80)
		dev->temp = -dev->temp;
}

static inline int receive_data(struct dht22_ctx *dev)
{
	uint8_t data[5] = { 0 };

	*dev->ddr &= ~_BV(dev->bit);

	/* receive data, from MSB to LSB */
	for (int i = 0; i < 5; ++i) {
		for (int b = 7; b >= 0; --b) {
			/* confirm low(50us) */
			if (wait_for_change(dev, TIMEOUT_LOOPS, 0) < 0)
				return -3;
			_delay_us(50);

			/* currently should be high */
			if (0 == (*dev->pin & _BV(dev->bit)))
				return -4;
			_delay_us(20);

			/* get bit 0 if confirm high(26-28us)
			 *     bit 1 if confirm high(70us)
			 * 15 loops is about 16 us */
			if (wait_for_change(dev, 15, 0) < 0) {
				data[i] |= _BV(b);

				/* delay for remainder 30+ us */
				_delay_us(30);
				if (wait_for_change(dev, TIMEOUT_LOOPS, 0) < 0)
					return -5;
			}
		}
	}

#ifdef _DHT_DEBUG
	/* record the raw data */
	for (int i = 0; i < 5; ++i)
		dev->raw[i] = data[i];
#endif

	/* cksum */
	if ((data[0] + data[1] + data[2] + data[3]) != data[4])
		return -6;

	/* translate the data */
	parse_data(dev, data);

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
#ifdef _DHT_DEBUG
	for (int i = 0; i < 5; ++i)
		dev->raw[i] = 0;
#endif

	/* set high */
	*dev->ddr |= _BV(dev->bit);
	*dev->port |= _BV(dev->bit);

	/* confirm pin already pull-up */
	return check_pullup(dev);
}

int dht22_getdata(struct dht22_ctx *dev)
{
	/* check initial pin state */
	if (check_pullup(dev) < 0)
		return -1;
	_delay_ms(50);

	/* send request to DHT22 */
	send_request(dev);

	/* confirm ack responsed from DHT22 */
	if (confirm_ack(dev) < 0)
		return -2;

	/* receive DHT22 data */
	return receive_data(dev);
}
