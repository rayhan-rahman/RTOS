/*
 * uart.h
 *
 * Created: 8/4/2013 6:46:34 AM
 *  Author: rayhan
 */ 


#ifndef UART_H_
#define UART_H_

#include <inttypes.h>

typedef enum _uart_bps
{
	UART_19200,
	UART_38400,
	UART_57600,
}  UART_BPS;

void uart0_init(UART_BPS bitrate);
void uart0_write_byte(uint8_t byte);
uint8_t uart0_read_byte(uint8_t index);
uint8_t uart0_bytes_received(void);
void uart0_reset_receive(void);

void uart1_init(UART_BPS bitrate);
void uart1_write_byte(uint8_t byte);
uint8_t uart1_read_byte(uint8_t index);
uint8_t uart1_bytes_received(void);
void uart1_reset_receive(void);


void write_string(char* str, int len);

#endif /* UART_H_ */
