/*
 * RTOS.c
 *
 * Created: 7/28/2013 11:06:07 AM
 *  Author: rayhan
 */ 


#include <util/delay.h>
#include <avr/io.h>
#include "os.h"


//#define INSTRUMENTING

enum { A=1, B, C, D, E, F, G };
/*
const unsigned char PPP[] = {A, 20, B, 20, IDLE, 40, C, 20, D, 20, IDLE, 40};
const unsigned int PT = sizeof(PPP)/2;
*/
EVENT* e;

//const unsigned char PPP[] = {};
//const unsigned int PT = 0;
void periodic_task(void)
{
	//uint8_t i;
	uint8_t arg = 0;
	arg = Task_GetArg();
	uint8_t v = 0b10000000;
	
	//if (arg == A) v = _BV(PB0);
	//else if (arg == B) v = _BV(PB1);
	//else if (arg == C) v = _BV(PB2);
	//else if (arg == D) v = _BV(PB3);
	//Event_Wait(e);	// periodic events can't wait!
	for(;;)
	{
		
		//PORTB &= ~v;
		if (arg == A) _delay_ms(25);
		else if (arg == B) _delay_ms(75);
		else if (arg == C) _delay_ms(25);
		else if (arg == D) _delay_ms(75);
		//PORTB |= v;
		
	
		Task_Next();
	}
}

#ifdef MINE
void a(void){
	
	for (;;){
		PORTB |= (_BV(PB0) | _BV(PB1) | _BV(PB2) |_BV(PB3));
		PORTB &= ~_BV(PB0);
		//PORTB ^= _BV(PB0);
		Task_Next();
	}
}

void b(void){
	for(;;){
		PORTB |= (_BV(PB0) | _BV(PB1) | _BV(PB2) |_BV(PB3));
		PORTB &= ~_BV(PB1);
		//PORTB ^= _BV(PB1);
		
		Task_Next();
	}
}

void c(void){
	for(;;){
		PORTB |= (_BV(PB0) | _BV(PB1) | _BV(PB2) |_BV(PB3));
		PORTB &= ~_BV(PB2);
		//PORTB ^= _BV(PB2);
		Task_Next();
	}
}

void d(void){
	for(;;){
		PORTB |= (_BV(PB0) | _BV(PB1) | _BV(PB2) |_BV(PB3));
		PORTB &= ~_BV(PB3);
		//PORTB ^= _BV(PB3);
		Task_Next();
	}
}

#endif

void rr_task(void)
{
	for (;;)
	{
		Task_Next();
	}
}

void test_system_task(void)
{
	_delay_ms(10);
}


//#define RTOS_R_MAIN
#ifdef RTOS_R_MAIN
int r_main(void)
{
	DDRB = 0xFF;
	PORTB  = 0xFF;
#ifdef INSTRUMENTING	
	PORTB |= _BV(PB2);
	e = Event_Init();
	PORTB &= ~_BV(PB2);
#endif	
	// Task_Create params: function, parameter, level, name
//#define MINE
#ifdef MINE
	Task_Create_Period(a, A, 160, 20, 0);
	Task_Create_Period(b, B, 160, 20, 20);
	Task_Create_Period(c, C, 160, 20, 80);
	Task_Create_Period(d, D, 160, 20, 100);
	
#endif

//#define NEILS
#ifdef NEILS
	Task_Create_Period(periodic_task, A, 160, 20, 0);
	Task_Create_Period(periodic_task, B, 160, 20, 20);
	Task_Create_Period(periodic_task, C, 160, 20, 80);
	Task_Create_Period(periodic_task, D, 160, 20, 100);	
	Task_Create_RR(rr_task, 0);

#endif
	return 0;
}
#endif

#define TESTING_AND_INSTRUMENTING
#ifdef TESTING_AND_INSTRUMENTING
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
	//PORTB |= _BV(PB0);
	PORTB &= ~_BV(PB0);
#endif		
		_delay_ms(1);
		Task_Next();
		
#ifdef USER_ADD_TRACE
	//PORTB &= ~_BV(PB0);
	PORTB |= _BV(PB0);
	
#endif		
	}

}

void rr_task_2(void)
{
	
	while(1){	
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB0);
	//PORTB &= ~_BV(PB1);
#endif

	_delay_ms(5);
	Task_Next();
	
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB0);
	//PORTB |= _BV(PB1);
#endif
	}
	
}

void rr_task_3(void)
{

	while(1){
#ifdef USER_ADD_TRACE
	//PORTB |= _BV(PB2);
	PORTB &= ~_BV(PB2);
#endif		

	_delay_ms(10);
	Task_Next();
	
#ifdef USER_ADD_TRACE
	//PORTB &= ~_BV(PB2);
	PORTB |= _BV(PB2);
#endif		
	}
	
}

void rr_task_4(void)
{
	while(1){
#ifdef USER_ADD_TRACE
	//PORTB |= _BV(PB3);
	PORTB &= ~_BV(PB3);
#endif	
	
	_delay_ms(15);
	Task_Next();
	
#ifdef USER_ADD_TRACE
	//PORTB &= ~_BV(PB3);
	PORTB |= _BV(PB3);
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
		_delay_ms(8);
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
	//PORTB |= _BV(PB2);
#endif		
		
		_delay_ms(15);
		
#ifdef USER_ADD_TRACE
	//PORTB &= ~_BV(PB2);
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
	PORTB |= _BV(PB0);
#endif		
		Task_Create_System(system_task_1, 0);
		_delay_ms(10);
		Task_Next();
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB0);
#endif		
	}
}


void system_task_2(void)
{
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB1);
#endif

	_delay_ms(10);
	
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB1);
#endif
}

void system_task_3(void)
{
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB2);
#endif
	
	_delay_ms(15);
	
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB2);
#endif	
}

void system_task_4(void)
{
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB4);
#endif
	_delay_ms(15);
	//Event_Wait(e);
	
#ifdef USER_ADD_TRACE
	PORTB &= ~_BV(PB4);
#endif	
}

void system_task_5(void)
{
#ifdef USER_ADD_TRACE
	PORTB |= _BV(PB5);
#endif
	_delay_ms(15);
	//Event_Wait(e);
	
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
	DDRB |= 0xFF;
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
	DDRB |= 0xFF;
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
	DDRB |= 0xFF;
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
	DDRB |= 0xFF;
	Task_Create_Period(periodic_task_1, 0, 5, 10, 0);
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
	DDRB |= 0xFF;
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
	DDRB |= 0xFF;
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
	DDRB |= 0xFF;
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
	volatile uint8_t i;
	DDRB |= 0xFF;
	_delay_ms(20);
	Task_Create_System(system_task_1, 0);
	Task_Create_System(system_task_2, 0);
	Task_Create_System(system_task_3, 0);


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
	DDRB |= 0xFF;
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
	DDRB |= 0xFF;
	Task_Create_Period(periodic_task_6, 0, 100, 10, 0);
	
	return 0;
} 

#endif

/*

System Tasks preempt RR task

*/
#define TEST_SYSTEM_PREEMPT_RR
#ifdef TEST_SYSTEM_PREEMPT_RR
	
int r_main(void)
{
	uint8_t i;
	DDRB |= 0xFF;
	Task_Create_RR(rr_task_2, 0);
	_delay_ms(5);
	for (i = 0; i < 3; ++i){
		Task_Create_System(system_task_2, 0);
		_delay_ms(1);
		//Task_Create_System(system_task_2, 0);
	}
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
	DDRB |= 0xFF;
	Task_Create_RR(rr_task_3, 0);
	//_delay_ms(5);
	DDRB |= 0xFF;
	PORTB |= 0x00;
	Task_Create_Period(periodic_task_2, 0, 5, 2, 5);
	
	return 0;
}

#endif


/*

Round robin tasks go to the end of the queue

*/
//#define TEST_RR_GOES_TO_END_OF_QUEUE
#ifdef TEST_RR_GOES_TO_END_OF_QUEUE


int r_main(void)
{
	DDRB |= 0xFF;
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
	DDRB |= 0xFF;
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

//#define INSTRUMENTING
#ifdef INSTRUMENTING

void dummy_rr()
{
	while(1){
		Task_Next();
	}
}

void dummy_period()
{
	while(1){
		_delay_ms(5);
		Task_Next();
	}	
}

void dummy_system_1()
{
	uint8_t i;
	EVENT* f[10];
	for (i = 0; i < 10; ++i){
		//PORTB |= _BV(PB7);
		f[i] = Event_Init();
		_delay_ms(10);
		//PORTB &= ~_BV(PB7);
	}
}

void dummy_system_2()
{
	uint8_t i;
	for (i = 0; i < 10; ++i){
		//PORTB |= _BV(PB7);
		Task_Create_System(dummy_system_1, 0);
		//PORTB &= ~_BV(PB7);
	}
}
EVENT* f[10];

void dummy_system_wait()
{
	uint8_t i = Task_GetArg();
	//_delay_ms(5);
	Event_Wait(f[i]);
}

int r_main(void)
{
	uint8_t i;
	DDRB |= 0xFF;
	PORTB |= 0x00;

	//for (i = 0; i < 10; ++i){
		//PORTB |= _BV(PB7);
		//Task_Create_Period(dummy_period, 0, 100, 10, 5);
		//PORTB &= ~_BV(PB7);
	//}
	//Task_Create_System(dummy_system_2, 0);
	//Task_Create_System(dummy_system_1, 0);
	
	//for (i = 0; i < 10; ++i){
		//PORTB |= _BV(PB7);
		//Task_Create_RR(dummy_rr, 0);
		//PORTB &= ~_BV(PB7);
		//_delay_ms(10);
	//}
	
	for (i = 0; i < 10; ++i){
		//PORTB |= _BV(PB7);
		f[i] = Event_Init();
		//_delay_ms(10);
		//PORTB &= ~_BV(PB7);
	}
	
	for (i = 0; i < 5; ++i){
		Task_Create_System(dummy_system_wait, i);
		//_delay_ms(10);
	}
	
	

	
	return 0;	
}

#endif

#endif
