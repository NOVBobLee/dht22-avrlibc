#include <avr/io.h>
#include <util/setbaud.h>
#include "print_uart.h"

static void uart_init(void);
static int uart_putc(char c, FILE *stream);

static FILE my_stdout = FDEV_SETUP_STREAM(uart_putc, NULL, _FDEV_SETUP_WRITE);

void print_init(void)
{
	/* define stdout */
	stdout = &my_stdout;
	uart_init();
}

static void uart_init(void)
{
	/* set uart baud rate register (u2x mode if supply) */
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
#if USE_2X
#pragma message "Use 2x"
	UCSR0A |= _BV(U2X0);
#else
	UCSR0A &= ~_BV(U2X0);
#endif	/* USE_2X */

	/* character size: 8 bits */
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);

	/* enable tx */
	UCSR0B |= _BV(TXEN0);
}

static int uart_putc(char c, FILE *stream)
{
	/* check the transmit buffer is empty (ready to receive new data) */
	loop_until_bit_is_set(UCSR0A, UDRE0);
	/* write in char */
	UDR0 = c;
	return 0;
}
