#include <avr/io.h>
#include <util/delay.h>
#include "print_uart.h"
#include "dht22.h"

int main(void)
{
	struct dht22_ctx dev;
	int ret;

	print_init();

	if (dht22_init(&dev, &DDRB, &PINB, &PORTB, 0) < 0) {
		printf("Error: dht22_init\n");
		return -1;
	}

	while (1) {
		ret = dht22_getdata(&dev);
		if (0 == ret)
			printf("T=%.1f°C rH=%.1f%%\n", dev.temp, dev.rh);
		else
			printf("Error! Code=%d\n", ret);

		/* Collecting period needs to be >2 seconds */
		_delay_ms(10000);
	}

	return 0;
}
