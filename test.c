/*
 * test.c
 *
 * Created: 8/4/2013 5:45:15 AM
 *  Author: rayhan
 */ 

#include <avr/io.h>
#include <avr/delay.h>
#include <inttypes.h>
#include "os.h"
#include "trace.h"

enum {
	
	RR_TASK_1_ID = 100,
	RR_TASK_2_ID,
	RR_TASK_3_ID,
	RR_TASK_4_ID,
	
	PERIODIC_TASK_1_ID,
	PERIODIC_TASK_2_ID,
	PERIODIC_TASK_3_ID,
	PERIODIC_TASK_4_ID,
	PERIODIC_TASK_5_ID,
	
	SYSTEM_TASK_1_ID,
	SYSTEM_TASK_2_ID,
	SYSTEM_TASK_3_ID,
	SYSTEM_TASK_4_ID,
	SYSTEM_TASK_5_ID,

};


EVENT* e;
#define USER_ADD_TRACE

void rr_task_1(void)
{
	while(1){
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB0);
#endif		
		_delay_ms(1);
		Task_Next();
		
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB0);
#endif		
	}

}

void rr_task_2(void)
{
	
	while(1){	
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB1);
#endif

	_delay_ms(5);
	Task_Next();
	
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB1);
#endif
	}
	
}

void rr_task_3(void)
{
	while(1){
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB2);
#endif		

	_delay_ms(10);
	Task_Next();
	
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB3);
#endif		
	}
	
}

void rr_task_4(void)
{
	while(1){
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB4);
#endif	
	
	_delay_ms(15);
	Task_Next();
	
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB4);
#endif	
	}
	
}

void periodic_task_1(void)
{
	while(1){
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB0);
#endif
		
		_delay_ms(5);
		Task_Next();

#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB0);
#endif		
	}
}

void periodic_task_2(void)
{
	while(1){
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB1);
#endif
		
		_delay_ms(10);
		Task_Next();

#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB1);
#endif		
	}
}

void periodic_task_3(void)
{
	while(1){
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB2);
#endif		
		
		_delay_ms(15);
		Task_Next();
		
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB2);
#endif		
	}
}

void periodic_task_4(void)
{
	EVENT* e = Event_Init();
	while(1){
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB3);
#endif
		
		Event_Wait(e);
		Task_Next();
		
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB3);
#endif		
	}
}

void periodic_task_5(void)
{
	EVENT* e = Event_Init();
	while(1){
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB4);
#endif		
		Event_Wait_Next(e);
		Task_Next();
		
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB4);
#endif		
	}
}


void system_task_1(void)
{
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB1);
#endif

	_delay_ms(5);
	
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB1);
#endif	
}

void periodic_task_6(void)
{
	while(1){
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB5);
#endif		
		Task_Create_System(system_task_1, 0);
		_delay_ms(10);
		Task_Next();
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB5);
#endif		
	}
}


void system_task_2(void)
{
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB2);
#endif

	_delay_ms(10);
	
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB2);
#endif
}

void system_task_3(void)
{
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB3);
#endif
	
	_delay_ms(15);
	
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB3);
#endif	
}

void system_task_4(void)
{
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB4);
#endif
	
	Event_Wait(e);
	
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB4);
#endif	
}

void system_task_5(void)
{
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB5);
#endif
	
	Event_Wait(e);
	
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB5);
#endif	
}

/****

MAXEVENT exceeded

*/
//#define TEST_MAXEVENT
#ifdef TEST_MAXEVENT

int r_main(void)
{
	uint8_t i;
	EVENT* e;
	for (i = 0; i < MAXEVENT + 1; ++i){
		e = Event_Init();
	}	
	
	return 0;
}

#endif

/***

MAXPROCESS exceeded

*/
//#define TEST_MAXPROCESS
#ifdef TEST_MAXPROCESS

int r_main(void)
{
	uint8_t i;
	Task_Create_RR(rr_task_1, 0);
	Task_Create_Period(periodic_task_1, 0, 100, 4, 5);
	Task_Create_System(system_task_1, 0);
	Task_Create_RR(rr_task_2, 0);
	Task_Create_Period(periodic_task_2, 0, 100, 4, 15);
	Task_Create_System(system_task_2, 0);
	Task_Create_RR(rr_task_3, 0);
	Task_Create_Period(periodic_task_3, 0, 100, 4, 25);
	Task_Create_System(system_task_3, 0);
	Task_Create_RR(rr_task_4, 0);
	
	return 0;
}

#endif



/*

Periodic task called Event_Wait or Event_Wait_Next

*/
//#define TEST_EVENT_WAIT
#ifdef TEST_EVENT_WAIT


int r_main(void)
{

	Task_Create_Period(periodic_task_4, 0, 100, 5, 5);
	return 0;	
}

#endif

/*

Periodic task called Event_Wait_Next

*/
//#define TEST_EVENT_WAIT_NEXT
#ifdef TEST_EVENT_WAIT_NEXT


int r_main(void)
{

	Task_Create_Period(periodic_task_5, 0, 100, 5, 5);
	return 0;
}

#endif

/****

WCET is greater than the period

*/
//#define TEST_WCET_GREATER_THAN_PERIOD
#ifdef TEST_WCET_GREATER_THAN_PERIOD

int r_main(void)
{
	Task_Create_Period(periodic_task_1, 0, 1, 5, 10);
	return 0;
}

#endif

/***

Multiple Tasks waiting on the same event

*/
//#define TEST_MULTIPLE_TASK_WAITING
#ifdef TEST_MULTIPLE_TASK_WAITING




int r_main(void)
{
	e = Event_Init();
	Task_Create_System(system_task_4, 0);
	Task_Create_System(system_task_5, 0);
	
	return 0;
}

#endif



/*

Periodic task exceeded WCET 

*/
//#define TEST_EXCEEDED_WCET
#ifdef TEST_EXCEEDED_WCET

int r_main(void)
{
	Task_Create_Period(periodic_task_3, 0, 100, 2, 5);
	return 0;
}

#endif

/*

Periodic Task overlaps

*/
//#define TEST_PERIODIC_TASKS_OVERLAPS
#ifdef TEST_PERIODIC_TASKS_OVERLAPS

int r_main(void)
{
	Task_Create_Period(periodic_task_1, 0, 100, 5, 5);
	Task_Create_Period(periodic_task_2, 0, 100, 5, 5);
	return 0;
}

#endif

/***

Stack overflow

*/


/***

Stack underflow

*/



/*

Unknown error

*/

/*

System tasks are first come first serve

*/
//#define TEST_SYSTEM_FCFS
#ifdef TEST_SYSTEM_FCFS

int r_main(void)
{
	Task_Create_System(system_task_1, 0);
	Task_Create_System(system_task_2, 0);
	Task_Create_System(system_task_3, 0);
	Task_Create_System(system_task_4, 0);
	Task_Create_System(system_task_5, 0);
	
	return 0;
}

#endif

/*

RR tasks are First come first serve

*/

//#define TEST_RR_FCFS
#ifdef TEST_RR_FCFS

int r_main(void)
{
	
	Task_Create_RR(rr_task_1, 0);
	Task_Create_RR(rr_task_2, 0);
	Task_Create_RR(rr_task_3, 0);
		
	return 0;
}

#endif

/*

System Tasks preempt periodic tasks

*/
//#define TEST_SYSTEM_PREEMPT_PERIODIC
#ifdef TEST_SYSTEM_PREEMPT_PERIODIC

int r_main(void)
{
	Task_Create_Period(periodic_task_6, 0, 100, 10, 0);
	
	return 0;
} 

#endif

/*

System Tasks preempt RR task

*/
//#define TEST_SYSTEM_PREEMPT_RR
#ifdef TEST_SYSTEM_PREEMPT_RR
	
int r_main(void)
{
	Task_Create_RR(rr_task_2, 0);
	_delay_ms(5);
	Task_Create_System(system_task_2, 0);
	
	return 0;
}

#endif


/*

Periodic tasks preempt rr tasks

*/
//#define TEST_PERIOD_PREEMPT_RR
#ifdef TEST_PERIOD_PREEMPT_RR

int r_main(void)
{
	Task_Create_RR(rr_task_3, 0);
	_delay_ms(5);
	Task_Create_Period(periodic_task_2, 0, 100, 15, 5);
	
	return 0;
}

#endif


/*

Round robin tasks go to the end of the queue

*/
//#define TEST_RR_GO_TO_END_OF_QUEUE
#ifdef TEST_RR_GOES_TO_END_OF_QUEUE


int r_main(void)
{
	Task_Create_RR(rr_task_1, 0);
	Task_Create_RR(rr_task_2, 0);
	Task_Create_RR(rr_task_3, 0);
	Task_Create_RR(rr_task_4, 0);
	
	return 0;
}

#endif

/*

Round robin tasks go to the front of the queue
if preempted

*/
//#define TEST_RR_GO_TO_FRONT_OF_QUEUE
#ifdef TEST_RR_GO_TO_FRONT_OF_QUEUE

int r_main(void)
{
	Task_Create_RR(rr_task_4, 0);
	Task_Create_RR(rr_task_3, 0);
	_delay_ms(5);
	Task_Create_System(system_task_2, 0);
	
	return 0;
}

#endif

/*

System tasks are never preempted
shown by System FCFS test

*/


/*

Waiting tasks are signaled properly

*/
//#define TEST_TASKS_SIGNALED
#ifdef TEST_TASKS_SIGNALED

int r_main(void)
{
	
}

#endif


/*

Periodic and RR tasks are stretched properly

*/
//#define TEST_RR_PERIODIC_STRETCHED
#ifdef TEST_RR_PERIODIC_STRETCHED

int r_main(void)
{
	
}

#endif

/********************************************************************

INSTRUMENTING

********************************************************************/

#define INSTRUMENTING
#ifdef INSTRUMENTING
void dummy_period()
{
	while(1){
		_delay_ms(5);
		Task_Next();
	}	
}

int r_main(void)
{
	DDRB |= 0xFF;
	PORTB |= 0x00;

	PORTB |= _BV(PB0);
	Task_Create_Period(dummy_period, 0, 100, 10, 5);
	PORTB &= ~_BV(PB0);
	
	return 0;	
}

#endif
