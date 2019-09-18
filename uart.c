/*
 * uart.c
 *
 * Created: 8/4/2013 6:46:55 AM
 *  Author: rayhan
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart.h"

/************************************************************************/
/* UART Code                                                            */
/************************************************************************/

#define  UART0_BUFFER_SIZE 32
static volatile uint8_t uart0_buffer[UART0_BUFFER_SIZE];
static volatile uint8_t uart0_buffer_index = 0;

ISR(USART0_RX_vect)
{
	uart0_buffer[uart0_buffer_index] = UDR0;
	uart0_buffer_index = (uart0_buffer_index + 1) % UART0_BUFFER_SIZE;
}

void uart0_init(UART_BPS bitrate)
{
	UCSR0B = _BV(RXEN0) | _BV(TXEN0) | _BV(RXCIE0);
	UCSR0C = _BV(UCSZ00) | _BV(UCSZ01);
	UBRR0H = 0;

	/* see page 199 of doc8272*/
	switch (bitrate)
	{
		case  UART_19200:
		UBRR0L = 35;
		break;
		case UART_38400:
		UBRR0L = 17;
		break;
		case UART_57600:
		UBRR0L = 11;
		break;
	}
	
	uart0_buffer_index = 0;
	
}

void uart0_write_byte(uint8_t byte)
{
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = byte;
}


uint8_t uart0_read_byte(uint8_t index)
{
	if (index < UART0_BUFFER_SIZE)
	{
		return uart0_buffer[index];
	}
	return 0;
}

uint8_t uart0_bytes_received(void)
{
	return uart0_buffer_index;
}

void uart0_reset_receive(void)
{
	uart0_buffer_index = 0;
}



#define  UART1_BUFFER_SIZE 32
static volatile uint8_t uart1_buffer[UART1_BUFFER_SIZE];
static volatile uint8_t uart1_buffer_index = 0;

ISR(USART1_RX_vect)
{
	uart1_buffer[uart1_buffer_index] = UDR1;
	uart1_buffer_index = (uart1_buffer_index + 1) % UART1_BUFFER_SIZE;
}

void uart1_init(UART_BPS bitrate)
{
	UCSR1B = _BV(RXEN1) | _BV(TXEN1) | _BV(RXCIE1);
	UCSR1C = _BV(UCSZ10) | _BV(UCSZ11);
	UBRR1H = 0;

	/* see page 199 of doc8272*/
	switch (bitrate)
	{
		case  UART_19200:
		UBRR1L = 35;
		break;
		case UART_38400:
		UBRR1L = 17;
		break;
		case UART_57600:
		UBRR1L = 11;
		break;
	}
	
	uart1_buffer_index = 0;
	
}

void uart1_write_byte(uint8_t byte)
{
	while (!(UCSR1A & (1 << UDRE1)));
	UDR1 = byte;
}


uint8_t uart1_read_byte(uint8_t index)
{
	if (index < UART1_BUFFER_SIZE)
	{
		return uart1_buffer[index];
	}
	return 0;
}

uint8_t uart1_bytes_received(void)
{
	return uart1_buffer_index;
}

void uart1_reset_receive(void)
{
	uart1_buffer_index = 0;
}

void write_string(char* str, int len)
{
	int i;
	for(i = 0; i < len; ++i)
	{
		uart1_write_byte(str[i]);
	}
}

