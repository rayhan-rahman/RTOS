/*
 * trace.c
 *
 * Created: 8/4/2013 6:58:11 AM
 *  Author: rayhan
 */ 

#include <stdlib.h>
#include <string.h>
#include "trace.h"
#include "uart.h"

uint8_t trace_data_index;

void init_trace()
{
	trace_data_index = 0;
	uart1_init(UART_57600);
}

void add_trace(uint8_t process_number, uint8_t enter, uint16_t time)
{
	TRACE_BUFFER[trace_data_index].process_no = process_number;
	TRACE_BUFFER[trace_data_index].enter = enter;
	TRACE_BUFFER[trace_data_index].time = time;
	
	trace_data_index = (trace_data_index + 1) % TRACE_BUFFER_SIZE;
}

char buf[10];

void print_trace()
{
	uint16_t i;
	for (i = 0; i < TRACE_BUFFER_SIZE; ++i){
		write_string("[ ", 2);
		uart1_write_byte( TRACE_BUFFER[i].process_no );
		write_string(", ", 2);
		uart1_write_byte( TRACE_BUFFER[i].enter );
		write_string(", ", 2);
		itoa(TRACE_BUFFER[i].time, buf, 10);
		write_string(buf, strlen(buf));
		write_string(" ]\n\r", 4);
	}
}
