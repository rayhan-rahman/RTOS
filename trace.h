/*
 * trace.h
 *
 * Created: 8/4/2013 6:58:28 AM
 *  Author: rayhan
 */ 


#ifndef TRACE_H_
#define TRACE_H_

#include "uart.h"


typedef struct 
{
	/* process_no is used to identify the function */
	uint8_t process_no;
	/* function entered will be denoted with a 0
	and function exit will be denoted with a 1 */
	uint8_t enter;
	/* time of this function */
	uint16_t time;	
} trace_data;

#define TRACE_BUFFER_SIZE 256
trace_data TRACE_BUFFER[TRACE_BUFFER_SIZE];

void init_trace();
void add_trace(uint8_t process_number, uint8_t enter, uint16_t time);
void print_trace();

#endif /* TRACE_H_ */