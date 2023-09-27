#include <avr/io.h>
#include <util/delay.h>
#include "print_uart.h"
#include "dht22.h"

#define REPEAT 5

int main(void)
{
	struct dht22_ctx dev;
	int ret;

	print_init();

	if (dht22_init(&dev, &DDRB, &PINB, &PORTB, 0) < 0) {
		printf("Error: dht22_init\n");
		return -1;
	}

	for (int t = 0; t < REPEAT; ++t) {
		ret = dht22_getdata(&dev);
		if (0 == ret)
			printf("Temperature=%hu RelativeHumidity=%hu\n", dev.temp, dev.rh);
		else
			printf("Error! Code=%d\n", ret);

		/* Collecting period needs to be >2 seconds */
		_delay_ms(2000);
	}

	return 0;
}
