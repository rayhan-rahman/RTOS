/**
 * @file os.c
 *
 * Created: 7/24/2013 1:00:11 AM
 *  Author: rayhan
 */ 


#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include "os.h"
#include "error_code.h"
#include "trace.h"

//#define INSTRUMENTING
//#define MDEBUG
//#define F_CPU 11059200UL
#define DISABLE_INTERRUPT() asm volatile("cli"::)
#define ENABLE_INTERRUPT()  asm volatile("sei"::)

#define TRACING_WITH_LOGIC_ANALYZER

#ifdef TRACING_WITH_LOGIC_ANALYZER
#define LOGIC_ANALYZER_PORT PORTB
#define INIT_LOGIC_ANALYZER DDRB |= 0xFF;
#define LOGIC_ANALYZER_ERROR_CHANNEL PB7
#endif
	
typedef enum {
	NONE,
	EVENT_INIT,
	EVENT_WAIT,
	EVENT_WAIT_NEXT,
	EVENT_SIGNAL,
	EVENT_ASYNC_SIGNAL,
	TASK_CREATE_SYSTEM,
	TASK_CREATE_PERIODIC,
	TASK_CREATE_RR,
	TASK_TERMINATE,
	TASK_NEXT,
	TASK_GET_ARG,
	RTOS_TICK,
}kernel_request_t;

typedef enum {

	EVENT_INIT_ID = RTOS_TICK + 1,
	EVENT_WAIT_ID,
	EVENT_WAIT_NEXT_ID,
	EVENT_SIGNAL_ID,
	TASK_CREATE_SYSTEM_ID,
	TASK_CREATE_PERIODIC_ID,
	TASK_CREATE_RR_ID,
	TASK_NEXT_ID,
	TASK_TERMINATE_ID,
	TASK_GET_ARG_ID,
	PUSH_FRONT_ID,
	PUSH_BACK_ID,
	POP_FRONT_ID,
	POP_BACK_ID,
	TIMER2_COMPA_vect_ID,
	ENTER_KERNEL_ID,
	EXIT_KERNEL_ID,
	KERNEL_TERMINATE_TASK_ID,
	KERNEL_HANDLE_REQUEST_ID,
	KERNEL_EVENT_WAIT_ID,
	KERNEL_EVENT_WAIT_NEXT_ID,
	KERNEL_EVENT_SIGNAL_ID,
	KERNEL_DISPATCH_ID,
	KERNEL_ARRIVED_PERIODIC_TASK_ID,
	KERNEL_CREATE_IDLE_TASK_ID,
	KERNEL_CREATE_RR_TASK_ID,
	KERNEL_CREATE_SYSTEM_TASK_ID,
	KERNEL_CREATE_PERIODIC_TASK_ID,
	KERNEL_SCHEDULABLE_ID,
	KERNEL_CREATE_TASK_CONTEXT_ID,
	OS_ABORT_ID,
	
} function_id;

enum {
	ENTER = 0,
	EXIT
};

//#define ADD_TO_TRACE

static volatile kernel_request_t kernel_request;
static void enter_kernel() __attribute( (noinline, naked) );
static void exit_kernel() __attribute( (noinline, naked) );

static void idle(void) __attribute__((noinline));
static void OS_Abort();

static volatile ERROR kernel_error;


/*****************************************
Event Management Code
*****************************************/

static volatile uint8_t num_events_created;
static EVENT * kernel_request_event_ptr;

EVENT  *Event_Init(void)
{
#ifdef ADD_TO_TRACE
	add_trace(EVENT_INIT_ID, ENTER, Now());
#endif
	
	EVENT* event_ptr;
	volatile uint8_t sreg;
	
	sreg = SREG;
	DISABLE_INTERRUPT();
	
	kernel_request = EVENT_INIT;
	enter_kernel();
	
	event_ptr = kernel_request_event_ptr;
	
	SREG = sreg;
	
#ifdef ADD_TO_TRACE
	add_trace(EVENT_INIT_ID, EXIT, Now());
#endif
	
	return event_ptr;
}

void Event_Wait(EVENT* e)
{
#ifdef ADD_TO_TRACE
	add_trace(EVENT_WAIT_ID, ENTER, Now());
#endif	

	PORTB |= _BV(PB6);
	volatile uint8_t sreg = SREG;
	
	DISABLE_INTERRUPT();
	
	kernel_request = EVENT_WAIT;
	kernel_request_event_ptr = e;
	
	PORTB &= ~_BV(PB6);
	enter_kernel();
	
	SREG = sreg;
	
#ifdef ADD_TO_TRACE
	add_trace(EVENT_WAIT_ID, EXIT, Now());
#endif	
}

void Event_Wait_Next(EVENT* e)
{
#ifdef ADD_TO_TRACE
	add_trace(EVENT_WAIT_NEXT_ID, ENTER, Now());
#endif	
	volatile uint8_t sreg = SREG;
	
	DISABLE_INTERRUPT();
	
	kernel_request = EVENT_WAIT_NEXT;
	kernel_request_event_ptr = e;
	enter_kernel();
	
	SREG = sreg;
#ifdef ADD_TO_TRACE
	add_trace(EVENT_WAIT_NEXT_ID, EXIT, Now());
#endif	
}

void Event_Signal( EVENT *e )
{
#ifdef ADD_TO_TRACE
	add_trace(EVENT_SIGNAL_ID, ENTER, Now());
#endif	
	volatile uint8_t sreg = SREG;
	
	DISABLE_INTERRUPT();
	
	kernel_request = EVENT_SIGNAL;
	kernel_request_event_ptr = e;
	enter_kernel();
	
	SREG = sreg;	

#ifdef ADD_TO_TRACE
	add_trace(EVENT_SIGNAL_ID, EXIT, Now());
#endif	
}

void Event_Async_Signal( EVENT *e )
{
	
}

/***********************************************
Task Management 
*************************************************/

typedef void (*voidfuncvoid_ptr)(void); /* pointer to f(void) types */
typedef struct {
	voidfuncvoid_ptr f;
	uint8_t task_level;
	int arg;
	unsigned int period;
	unsigned int wcet;
	unsigned int start;
} task_create_arg_t;


static int kernel_task_create_result;
static task_create_arg_t kernel_task_create_args;

int Task_Create_System(void	(*f)(void), int arg)
{
#ifdef ADD_TO_TRACE
	add_trace(TASK_CREATE_SYSTEM_ID, ENTER, Now());
#endif
	volatile uint8_t sreg = SREG;
	DISABLE_INTERRUPT();
	
	kernel_task_create_args.f = f;
	kernel_task_create_args.arg = arg;
	kernel_task_create_args.task_level = SYSTEM;
	kernel_request = TASK_CREATE_SYSTEM;
	enter_kernel();

	SREG = sreg;
#ifdef ADD_TO_TRACE
	add_trace(TASK_CREATE_SYSTEM_ID, EXIT, Now());
#endif	
	return kernel_task_create_result;
}

int Task_Create_RR( void (*f)(void), int arg)
{
#ifdef ADD_TO_TRACE
	add_trace(TASK_CREATE_RR_ID, ENTER, Now());
#endif	

	
	volatile uint8_t sreg = SREG;
	DISABLE_INTERRUPT();
	
	kernel_task_create_args.f = f;
	kernel_task_create_args.arg = arg;
	kernel_task_create_args.task_level = RR;
	kernel_request = TASK_CREATE_RR;
	enter_kernel();

	SREG = sreg;
	
#ifdef ADD_TO_TRACE
	add_trace(TASK_CREATE_RR_ID, EXIT, Now());
#endif	

	return kernel_task_create_result;	
}

int Task_Create_Period( void (*f)(void), int arg, unsigned int period,
                          unsigned int wcet, unsigned int start)
{
#ifdef ADD_TO_TRACE
	add_trace(TASK_CREATE_PERIODIC_ID, ENTER, Now());
#endif	

	if (period < wcet){
		kernel_error = WCET_GREATER;
		OS_Abort();
	}
	
	volatile uint8_t sreg = SREG;
	DISABLE_INTERRUPT();

	kernel_task_create_args.f = f;
	
	kernel_task_create_args.arg = arg;
	kernel_task_create_args.task_level = PERIODIC;
	kernel_task_create_args.wcet = wcet;
	kernel_task_create_args.period = period;
	kernel_task_create_args.start = start;
	kernel_request = TASK_CREATE_PERIODIC;

	enter_kernel();

	SREG = sreg;
	
	
#ifdef ADD_TO_TRACE
	add_trace(TASK_CREATE_PERIODIC_ID, EXIT, Now());
#endif	

	return kernel_task_create_result;							  
}

void Task_Terminate(void)
{
	
#ifdef ADD_TO_TRACE
	add_trace(TASK_TERMINATE_ID, ENTER, Now());
#endif	
	volatile uint8_t sreg = SREG;
	DISABLE_INTERRUPT();
	
	kernel_request = TASK_TERMINATE;
	enter_kernel();
	
	SREG = sreg;
	
#ifdef ADD_TO_TRACE
	add_trace(TASK_TERMINATE, EXIT, Now());
#endif	
}

typedef enum
{
	DEAD = 0,
	READY,
	RUNNING,
	WAITING
} task_state_t;

typedef struct td_
{
	uint8_t stack[MAXSTACK];
	/* Xplained Stack pointer is 16 bit long.
	sp will store SP when this task is suspended
	*/
	uint16_t* volatile sp;  
	voidfuncvoid_ptr f;
	task_state_t task_state;
	uint8_t task_level;
	int arg;
	struct td_  * next_task;
	int allowed_ticks;
	signed int remaining_ticks;
	signed int time_to_arrive_ticks;
	int period;
	unsigned int interrupted_time;
	unsigned int preemption_begin_time;
	uint8_t by_task_next;
} task_descriptor_t;
static task_descriptor_t* cur_task;


void Task_Next(void)
{
	
	#ifdef ADD_TO_TRACE
	add_trace(TASK_NEXT_ID, ENTER, Now());
	#endif

	cur_task->by_task_next = 1;
	
	volatile uint8_t sreg = SREG;
	DISABLE_INTERRUPT();
	
	kernel_request = TASK_NEXT;
	enter_kernel();
	
	SREG = sreg;
	
	#ifdef ADD_TO_TRACE
	add_trace(TASK_NEXT, EXIT, Now());
	#endif
}

int Task_GetArg( void )
{
	
#ifdef ADD_TO_TRACE
	add_trace(TASK_GET_ARG_ID, ENTER, Now());
#endif	

	int arg;
	volatile uint8_t sreg = SREG;
	DISABLE_INTERRUPT();
	
	arg = cur_task->arg;
	
	SREG = sreg;
	
#ifdef ADD_TO_TRACE
	add_trace(TASK_GET_ARG_ID, EXIT, Now());
#endif
		
	return arg;
}


/*****************************************************
OS Code
******************************************************/


typedef struct {
	task_descriptor_t * head;
	task_descriptor_t * tail;
} queue_t;

static void push_front(queue_t* queue, task_descriptor_t * task_descriptor)
{
	
#ifdef ADD_TO_TRACE
	add_trace(PUSH_FRONT_ID, ENTER, Now());
#endif	
	/* if the queue is empty */
	if ( queue->head == NULL){
		queue->head = task_descriptor;
		/* tail should also point to this task */
		queue->tail = task_descriptor;
		task_descriptor->next_task = NULL;
	}else{
		task_descriptor->next_task = queue->head;
		queue->head = task_descriptor;
	}
#ifdef ADD_TO_TRACE
	add_trace(PUSH_FRONT_ID, EXIT, Now());
#endif	

}

static void push_back(queue_t* queue, task_descriptor_t * task_descriptor) 
{
	
#ifdef ADD_TO_TRACE
	add_trace(PUSH_BACK_ID, ENTER, Now());
#endif	
	/* if the queue is empty */
	if ( queue->tail == NULL ){
		queue->tail = task_descriptor;
		/* head should also point to this task */
		queue->head = task_descriptor;
		task_descriptor->next_task = NULL;
	}else{
		queue->tail->next_task = task_descriptor;
		task_descriptor->next_task = NULL;
		queue->tail = task_descriptor;
	}
#ifdef ADD_TO_TRACE
	add_trace(PUSH_BACK_ID, EXIT, Now());
#endif	

}


void pop_front(queue_t* queue)
{
	
#ifdef ADD_TO_TRACE
	add_trace(POP_FRONT_ID, ENTER, Now());
#endif	
	if ( queue->head ){
		queue->head = queue->head->next_task;	
	}
	if ( queue->head == NULL ){
		queue->tail = NULL;
	}
#ifdef ADD_TO_TRACE
	add_trace(POP_FRONT_ID, EXIT, Now());
#endif

}

void pop_back(queue_t* queue)
{
	
#ifdef ADD_TO_TRACE
	add_trace(POP_BACK_ID, ENTER, Now());
#endif	
	/* WARNING: Linear time */
	if (queue->tail){
		task_descriptor_t  * t = queue->head;
		while(t->next_task != queue->tail){
			t = t->next_task;
		}
		queue->tail = t;
	}
	if (queue->tail == NULL){
		queue->head = NULL;
	}
#ifdef ADD_TO_TRACE
	add_trace(POP_BACK_ID, EXIT, Now());
#endif	

}

static task_descriptor_t TASK_DESCRIPTORS[MAXPROCESS+1];
static queue_t dead_pool_queue;
static queue_t system_queue;
static queue_t periodic_queue;
static queue_t rr_queue;
static task_descriptor_t*  event_queue[MAXEVENT];

extern int rmain();

static void idle(void)
{
	while(1){}
}

extern void r_main(void);


#define SAVE_CONTEXT_TOP()       asm volatile (\
"push      r31                    \n\t"\
"in        r31,      __SREG__     \n\t"\
"cli                             \n\t"::);

#define SET_I_BIT()              asm volatile(\
"ori       r31,      0x80         \n\t"::);

#define SAVE_CONTEXT_BOTTOM()    asm volatile(\
"push      r31                    \n\t"\
"push      r30                    \n\t"\
"push      r29                    \n\t"\
"push      r28                    \n\t"\
"push      r27                    \n\t"\
"push      r26                   \n\t"\
"push      r25                    \n\t"\
"push      r24                    \n\t"\
"push      r23                    \n\t"\
"push      r22                    \n\t"\
"push      r21                   \n\t"\
"push      r20                   \n\t"\
"push      r19                   \n\t"\
"push      r18                   \n\t"\
"push      r17                   \n\t"\
"push      r16                   \n\t"\
"push      r15                   \n\t"\
"push      r14                   \n\t"\
"push      r13                   \n\t"\
"push      r12                   \n\t"\
"push      r11                   \n\t"\
"push      r10                   \n\t"\
"push      r9                   \n\t"\
"push      r8                   \n\t"\
"push      r7                   \n\t"\
"push      r6                   \n\t"\
"push      r5                   \n\t"\
"push      r4                   \n\t"\
"push      r3                   \n\t"\
"push      r2                   \n\t"\
"push      r1                   \n\t"\
"clr       r1                   \n\t"\
"push      r0                   \n\t"::);

#define SAVE_CONTEXT() SAVE_CONTEXT_TOP(); SAVE_CONTEXT_BOTTOM();

#define RESTORE_CONTEXT()        asm volatile (\
"pop       r0                   \n\t"\
"pop       r1                   \n\t"\
"pop       r2                   \n\t"\
"pop       r3                   \n\t"\
"pop       r4                   \n\t"\
"pop       r5                   \n\t"\
"pop       r6                   \n\t"\
"pop       r7                   \n\t"\
"pop       r8                   \n\t"\
"pop       r9                   \n\t"\
"pop       r10                   \n\t"\
"pop       r11                  \n\t"\
"pop       r12                   \n\t"\
"pop       r13                   \n\t"\
"pop       r14                   \n\t"\
"pop       r15                   \n\t"\
"pop       r16                   \n\t"\
"pop       r17                   \n\t"\
"pop       r18                   \n\t"\
"pop       r19                   \n\t"\
"pop       r20                   \n\t"\
"pop       r21                   \n\t"\
"pop       r22                   \n\t"\
"pop       r23                   \n\t"\
"pop       r24                   \n\t"\
"pop       r25                   \n\t"\
"pop       r26                    \n\t"\
"pop       r27                    \n\t"\
"pop       r28                    \n\t"\
"pop       r29                    \n\t"\
"pop       r30                    \n\t"\
"pop       r31                    \n\t"\
"out       __SREG__, r31          \n\t"\
"pop       r31                    \n\t"::);

static volatile uint16_t kernel_sp;

/**************************
System Clock
*****************************/
#define TICK_CYCLES (F_CPU * TICK / 1000)


#define USING_TIMER_2
#ifdef USING_TIMER_2
#define PRESCALE 1024
#define ENABLE_CLOCK() TIMSK2 |= _BV(OCIE2A)
#define DISABLE_CLOCK() TIMSK2 &= ~_BV(OCIE2A)
static volatile unsigned int current_ticks;
void TIMER2_COMPA_vect(void) __attribute__( (signal, naked));
void init_clock()
{

	/* prescaling */
	TCCR2B |= (_BV(CS22) | _BV(CS21) | _BV(CS20));
	OCR2A = TICK_CYCLES / PRESCALE;
	
	/* Clock is not enabled here */
	//TIMSK2 |= _BV(OCIE2A);
}


void TIMER2_COMPA_vect(void)
{
	
#ifdef ADD_TO_TRACE
	add_trace(TIMER2_COMPA_vect_ID, ENTER, Now());
#endif	
#ifdef INSTRUMENTING
		PORTB |= _BV(PB5);
#endif
	SAVE_CONTEXT_TOP();
	SET_I_BIT();
	SAVE_CONTEXT_BOTTOM();
	cur_task->sp = (uint16_t*) SP;
	
	SP =  kernel_sp;
	RESTORE_CONTEXT();
	kernel_request = RTOS_TICK;
	
	OCR2A += TICK_CYCLES / PRESCALE;
	
#ifdef INSTRUMENTING
	PORTB &= ~_BV(PB5);
#endif

#ifdef ADD_TO_TRACE
	add_trace(TIMER2_COMPA_vect_ID, EXIT, Now());
#endif
	
	asm volatile ("ret\n"::);	
}

unsigned int Now()
{
	return (unsigned int) ((TCNT2 * TICK / OCR2A) + 5*current_ticks);
}

#endif

//#define USING_TIMER_1
#ifdef USING_TIMER_1
#define PRESCALE 64
#define ENABLE_CLOCK() TIMSK1 |= _BV(OCIE1A)
#define DISABLE_CLOCK() TIMSK1 &= ~_BV(OCIE1A)
static volatile unsigned int current_ticks;
void TIMER1_COMPA_vect(void) __attribute__( (signal, naked));
void init_clock()
{
	/* Clear on compare match, see page 156 of ATmega128p datasheet  */
	//TCCR1A |= _BV(WGM12);
	/* No Prescaling */
	TCCR1B |= (_BV(CS10));
	OCR1A = TICK_CYCLES;
	
	/* Clock is not enabled here */
	//TIMSK2 |= _BV(OCIE2A);
}

void TIMER1_COMPA_vect(void)
{
	#ifdef INSTRUMENTING
	//PORTB ^= _BV(PB2);
	#endif
	SAVE_CONTEXT_TOP();
	SET_I_BIT();
	SAVE_CONTEXT_BOTTOM();
	cur_task->sp = (uint16_t*) SP;
	
	SP =  kernel_sp;
	RESTORE_CONTEXT();
	kernel_request = RTOS_TICK;
	OCR1A += TICK_CYCLES;
	
	asm volatile ("ret\n"::);
}

unsigned int Now()
{
	return (unsigned int) ((TCNT1 * TICK / OCR2A) + current_ticks);
}

#endif



static void exit_kernel() 
{
	
#ifdef ADD_TO_TRACE
	add_trace(EXIT_KERNEL_ID, ENTER, Now());
#endif	
	/* saving kernel context */
	
	SAVE_CONTEXT()
	/* check overflow */
	/* not checking kernel stack overflow */
	
	/* WARNING: Potential source of error. */
	kernel_sp = SP;
	SP = (uint16_t) cur_task->sp;
	
	/* restoring task context */
	RESTORE_CONTEXT();
	/* check underflow */
	if ( (cur_task->stack[254] != (uint8_t) ( (uint16_t) Task_Terminate >> 8))
	|| (cur_task->stack[255] != (uint8_t) (uint16_t) Task_Terminate)
	)
	{
		kernel_error = STACK_UNDERFLOW;
		OS_Abort();
	}
	
	if (cur_task->by_task_next == 1){
		cur_task->interrupted_time = 0;
		cur_task->by_task_next =  0;
	}else if (cur_task->task_level == PERIODIC || cur_task->task_level == RR){
		cur_task->interrupted_time += (Now() - cur_task->preemption_begin_time); 
		if (cur_task->interrupted_time / 5 > 0){
			cur_task->remaining_ticks += (cur_task->interrupted_time / 5);
			cur_task->interrupted_time = (cur_task->interrupted_time % 5);
		}
	}
		
#ifdef ADD_TO_TRACE
	add_trace(EXIT_KERNEL_ID, EXIT, Now());
#endif
		
	asm volatile ("ret\n"::);	
}

static void enter_kernel() 
{
	
#ifdef ADD_TO_TRACE
	add_trace(ENTER_KERNEL_ID, ENTER, Now());
#endif	
	/* if a periodic or an RR task is preempted
	keep count of the preemption time */
	if (cur_task->task_level == PERIODIC || cur_task->task_level == RR){
		cur_task->preemption_begin_time = Now(); 
	}


	/* save task context */
	SAVE_CONTEXT()
	/* check overflow */
	if ( (cur_task->stack[0] != 0b10101010)
	|| (cur_task->stack[1] != 0b00110011)
	|| (cur_task->stack[2] != 0b11001100)
	|| (cur_task->stack[3] != 0b01010101)
	)
	{
		kernel_error = STACK_OVERFLOW;
		OS_Abort();
	}
	/* WARNING: Potential source of error. */
	cur_task->sp = (uint16_t*) SP;

	SP = kernel_sp;
	
	/* restoring kernel context */
	RESTORE_CONTEXT()
	/* check underflow */
    /* not checking kernel stack underflow */
	//PORTB &= ~_BV(PB0);
#ifdef ADD_TO_TRACE
	add_trace(ENTER_KERNEL_ID, EXIT, Now());
#endif
	
	asm volatile ("ret\n"::);
}


static void kernel_terminate_task()
{

#ifdef ADD_TO_TRACE
	add_trace(KERNEL_TERMINATE_TASK_ID, ENTER, Now());
#endif	

	cur_task->task_state = DEAD;
	push_back(&dead_pool_queue, cur_task);
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_TERMINATE_TASK_ID, EXIT, Now());
#endif	

}


#ifdef KEEP
static void kernel_update_tick()
{
	++current_ticks;
	
	if (cur_task->task_level == PERIODIC 
			|| cur_task->task_level == RR)
	{
		cur_task->remaining_ticks--;
		if (cur_task->remaining_ticks == 0){
			/* TODO emit error message */
			
			/* abort os */
			OS_Abort();
		}
	}
}
#endif


static task_descriptor_t* new_task;
static void kernel_create_task_context()
{
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_CREATE_TASK_CONTEXT_ID, ENTER, Now());
#endif	
	/* Task terminate after */
	new_task->stack[MAXSTACK-1] = (uint8_t) (uint16_t) Task_Terminate;
	new_task->stack[MAXSTACK-2] = (uint8_t) ( (uint16_t) Task_Terminate >> 8);
	/* address of the task */
	new_task->stack[MAXSTACK-3] = (uint8_t) (uint16_t) kernel_task_create_args.f;
	new_task->stack[MAXSTACK-4] = (uint8_t) ( (uint16_t) kernel_task_create_args.f >> 8);
	/* r31 is saved first in stack[MAXSTACK-5] */
	/* SREG is saved in stack[MAXSTACK-6] */
	new_task->stack[MAXSTACK-6] = _BV(SREG_I);
	/* r1 is the zero register */
	new_task->stack[MAXSTACK-37] = (uint8_t) 0;
	/* r0 is at new_task->stack[MAXSTACK-37] */
	/* stack pointer now should point to new_task->stack[MAXSTACK-38] */
	new_task->sp = &(new_task->stack[MAXSTACK-38]);
	
	/* for memory protection 
	four bytes will be written with special bit patterns.
	If these patterns are over written then stack
	overflow has occurred. Not a full-proof method.
	*/
	new_task->stack[3] = 0b01010101;
	new_task->stack[2] = 0b11001100;
	new_task->stack[1] = 0b00110011;
	new_task->stack[0] = 0b10101010;
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_CREATE_TASK_CONTEXT_ID, EXIT, Now());
#endif
	
}


static uint8_t kernel_create_system_task()
{
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_CREATE_SYSTEM_TASK_ID, ENTER, Now());
#endif

	if (dead_pool_queue.head == NULL){
		kernel_error = EXCEEDED_MAX_PROCESS;
		OS_Abort();
	}
	
	new_task = dead_pool_queue.head;
	pop_front(&dead_pool_queue);
	kernel_create_task_context();
	new_task->task_state = READY;
	new_task->task_level = SYSTEM;
	new_task->arg = kernel_task_create_args.arg;
	new_task->f = kernel_task_create_args.f;
	
	push_back(&system_queue, new_task);
	/* task creation was successful */
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_CREATE_SYSTEM_TASK_ID, EXIT, Now());
#endif
	
	return 1;
}

static uint8_t kernel_create_periodic_task()
{
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_CREATE_PERIODIC_TASK_ID, ENTER, Now());
#endif	

	if (dead_pool_queue.head == NULL)
	{
		kernel_error = EXCEEDED_MAX_PROCESS;
		OS_Abort();
	}
	
	new_task = dead_pool_queue.head;
	pop_front(&dead_pool_queue);
	kernel_create_task_context();
	new_task->task_level = PERIODIC;
	new_task->arg = kernel_task_create_args.arg;
	new_task->allowed_ticks = kernel_task_create_args.wcet;
	new_task->remaining_ticks = kernel_task_create_args.wcet;
	new_task->period = kernel_task_create_args.period;
	new_task->time_to_arrive_ticks = kernel_task_create_args.start;
	new_task->f = kernel_task_create_args.f;
	new_task->task_state = READY;
	new_task->interrupted_time = 0;
	new_task->by_task_next = 1;
	push_back(&periodic_queue, new_task);
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_CREATE_PERIODIC_TASK_ID, EXIT, Now());
#endif
	
	return 1;
}

static uint8_t kernel_create_rr_task()
{
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_CREATE_RR_TASK_ID, ENTER, Now());
#endif	
	if (dead_pool_queue.head == NULL){
		kernel_error = EXCEEDED_MAX_PROCESS;
		OS_Abort();
	}
	
	new_task = dead_pool_queue.head;
	pop_front(&dead_pool_queue);
	kernel_create_task_context();
	new_task->allowed_ticks = 2;
	new_task->remaining_ticks = 2;
	new_task->arg = kernel_task_create_args.arg;
	new_task->f = kernel_task_create_args.f;
	new_task->task_level = RR;
	new_task->task_state = READY;
	new_task->interrupted_time = 0;
	new_task->by_task_next = 1; //as if Task_Next() was called
	push_back(&rr_queue, new_task);
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_CREATE_RR_TASK_ID, EXIT, Now());
#endif
	
	return 1;
}

static uint8_t kernel_create_idle_task()
{
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_CREATE_IDLE_TASK_ID, ENTER, Now());
#endif	
	
	new_task = &TASK_DESCRIPTORS[MAXPROCESS];
	kernel_create_task_context();
	new_task->f = idle;
	new_task->task_level = NULL;
	/* task state should always be READY */
	new_task->task_state = READY;
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_CREATE_IDLE_TASK_ID, EXIT, Now());
#endif
	
	return 1;
}

static volatile uint16_t events;

static void kernel_event_wait()
{

#ifdef ADD_TO_TRACE
	add_trace(KERNEL_EVENT_WAIT_ID, ENTER, Now());
#endif

	volatile uint8_t handle = (uint8_t) ((uint16_t) (kernel_request_event_ptr) - 1);
	if (handle >= num_events_created){
		kernel_error = EXCEEDED_MAX_EVENTS;
		OS_Abort();
	} else if (cur_task->task_level == PERIODIC){
		kernel_error = PERIODIC_TASK_WAITING;
		OS_Abort();
	} else {
		/* check if some other task is already waiting for this task */
		if (event_queue[handle] != NULL){
			/* A task is already waiting for this event */
			kernel_error = MULTIPLE_TASKS_WAITING;
			OS_Abort();
		} else if( events & (1 << handle)){
			/* event already took place */
			events &= ~(1 << handle);
		} else {
			cur_task->task_state = WAITING;
			event_queue[handle] = cur_task;
		}
	}
	
#ifdef ADD_TO_TRACE
	add_trace(TASK_GET_ARG_ID, EXIT, Now());
#endif	

}

static void kernel_event_wait_next()
{
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_EVENT_WAIT_NEXT_ID, ENTER, Now());
#endif
	
	volatile uint8_t handle = (uint8_t) ((uint16_t) (kernel_request_event_ptr) - 1);
	
	if ( handle >= num_events_created){
		kernel_error = EXCEEDED_MAX_EVENTS;
		OS_Abort();
	} else if (cur_task->task_level == PERIODIC){
		/* PERIODIC tasks can't wait on events */
		kernel_error = PERIODIC_TASK_WAITING;
		OS_Abort();		
	} else {
		/* same task should wait */
		if (cur_task != event_queue[handle]){
			/* Error different tasks are waiting */
			kernel_error = MULTIPLE_TASKS_WAITING;
			OS_Abort();
		}
		/* else do nothing */
	}
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_EVENT_WAIT_NEXT_ID, EXIT, Now());
#endif
	
}

static void kernel_event_signal()
{
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_EVENT_SIGNAL_ID, ENTER, Now());
#endif
	
	volatile uint8_t handle = (uint8_t) ((uint16_t) (kernel_request_event_ptr) - 1);
	
	if ( handle >= num_events_created){
		kernel_error = EXCEEDED_MAX_EVENTS;
		OS_Abort();
	} else {
		if ( event_queue[handle]->task_level == PERIODIC){
			/* Periodic tasks can't wait */
			kernel_error = PERIODIC_TASK_WAITING;
			OS_Abort();
		} else if ( event_queue[handle]->task_level == SYSTEM ){
			push_back(&system_queue, event_queue[handle]);
		} else if ( event_queue[handle]->task_level == RR ){
			push_back(&rr_queue, event_queue[handle]);
		} else if ( event_queue[handle] == NULL ){
			events |= (1 << handle);
		} else {
			event_queue[handle]->task_state = READY;
			event_queue[handle] = NULL;
		}
	}
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_EVENT_SIGNAL_ID, EXIT, Now());
#endif
	
}

static task_descriptor_t* idle_task;// = &TASK_DESCRIPTORS[MAXPROCESS];
static uint8_t kernel_request_retval;

static void kernel_handle_request()
{
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_HANDLE_REQUEST_ID, ENTER, Now());
#endif	
	
	volatile task_descriptor_t * temp;
	switch(kernel_request){
		
		case NONE:
		break;
		
		case RTOS_TICK:
			
			++current_ticks;
			if (cur_task->task_level == RR && cur_task->task_state == RUNNING){
				if (cur_task->remaining_ticks > 0){
					cur_task->remaining_ticks--;
				}
				
				if (cur_task->remaining_ticks == 0 && cur_task->interrupted_time > 0){
					cur_task->remaining_ticks = 1;
					cur_task->interrupted_time = 0;
				}else if (cur_task->remaining_ticks == 0){
					cur_task->task_state = READY;
					push_back(&rr_queue, cur_task);
				}
			} else if (cur_task->task_level == PERIODIC && cur_task->task_state == RUNNING){
				if (cur_task->remaining_ticks > 0){
					cur_task->remaining_ticks--;
				}
				if (cur_task->remaining_ticks == 0 && cur_task->interrupted_time > 0){
					cur_task->remaining_ticks = 1;
					cur_task->interrupted_time = 0;
				}else if (cur_task->remaining_ticks == 0){
					/* Error: Periodic task exceeded wcet */
					/* Hard REAL TIME */
					kernel_error = EXCEEDED_WCET;
					OS_Abort();
				}
			}
			temp = periodic_queue.head;
			while (temp != NULL){
				//if (temp->time_to_arrive_ticks >0){
					temp->time_to_arrive_ticks--;
				//}
				temp = temp->next_task;
			}
			
		break;
		
		case TASK_CREATE_SYSTEM:
			kernel_request_retval = kernel_create_system_task();
			if ( cur_task->task_level != SYSTEM){
				/* definitely preempt */
				cur_task->task_state = READY;
			}
		break;
		
		case TASK_CREATE_PERIODIC:
			kernel_request_retval = kernel_create_periodic_task();
			/* if current task is a round robin task, it will be pre-empted
			at the next RTOS tick (but not immediately now), as periodic
			tasks begin at integer ticks */
			
			
		break;
		
		case TASK_CREATE_RR:
			kernel_request_retval = kernel_create_rr_task();
		break;
		
		case TASK_TERMINATE:
			if (cur_task != idle_task){
				kernel_terminate_task();
			}
		break;
		
		case TASK_NEXT:
			switch (cur_task->task_level)
			{
				case SYSTEM:
					cur_task->task_state = READY;
					push_back(&system_queue, cur_task);
				break;
				
				case PERIODIC:
					cur_task->time_to_arrive_ticks = cur_task->period - (cur_task->allowed_ticks - cur_task->remaining_ticks);
					cur_task->remaining_ticks = cur_task->allowed_ticks;
					cur_task->task_state = READY;
					
				break;
				
				case RR:
					cur_task->task_state = READY;
					cur_task->remaining_ticks = cur_task->allowed_ticks;
					push_back(&rr_queue, cur_task);
				break;
				
				default: /* idle_task */
				break;
			}
		cur_task->task_state = READY;
		break;
		
		case TASK_GET_ARG:
		break;
		
		case EVENT_INIT:
			kernel_request_event_ptr = NULL;
			if (num_events_created < MAXEVENT){
				kernel_request_event_ptr = (EVENT  *)(uint16_t)(num_events_created + 1);
				++num_events_created;
			}else{
				//kernel_request_event_ptr = (EVENT  *)(uint16_t)0;
				kernel_error = EXCEEDED_MAX_EVENTS;
				OS_Abort();
			}
		break;
		
		case EVENT_WAIT:
			if (cur_task != idle_task){
				kernel_event_wait();
			}
		break;
		
		case EVENT_WAIT_NEXT:
			if (cur_task != idle_task){
				kernel_event_wait_next();
			}
		break;
		
		case EVENT_SIGNAL:
			if (cur_task != idle_task){
				kernel_event_signal();
			}
		break;
		
		case EVENT_ASYNC_SIGNAL:
		
		break;
		
		default:
			OS_Abort();
	}
	
	kernel_request = NONE;
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_HANDLE_REQUEST_ID, EXIT, Now());
#endif
	
}

static uint8_t kernel_schedulable()
{
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_SCHEDULABLE_ID, ENTER, Now());
#endif
	
	volatile uint8_t n_tasks_arrived = 0;
	volatile task_descriptor_t* temp = periodic_queue.head;
	while( temp != NULL ){
		if (temp->time_to_arrive_ticks <= 0 || temp->remaining_ticks > 0){
			++n_tasks_arrived;
		}
		temp = temp->next_task;
	}
	
	if (n_tasks_arrived > 1){
		kernel_error = PERIODIC_TASKS_OVERLAP;
		OS_Abort();
	}
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_SCHEDULABLE_ID, EXIT, Now());
#endif	
	
	return n_tasks_arrived > 1 ? 0 : 1;
}

static task_descriptor_t* kernel_arrived_periodic_task()
{

#ifdef ADD_TO_TRACE
	add_trace(KERNEL_ARRIVED_PERIODIC_TASK_ID, ENTER, Now());
#endif
	
	volatile uint8_t arrived = 0;
	volatile task_descriptor_t* temp = periodic_queue.head;
	while( temp != NULL ){
		if (temp->time_to_arrive_ticks <= 0 || temp->remaining_ticks > 0){
			arrived = 1;
			break;
		}
		temp = temp->next_task;
	}
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_ARRIVED_PERIODIC_TASK_ID, EXIT, Now());
#endif	
	return arrived == 1 ? temp : NULL;
}

static void kernel_dispatch()
{
	
#ifdef ADD_TO_TRACE	
	add_trace(KERNEL_DISPATCH_ID, ENTER, Now());
#endif	

	if (cur_task->task_state != RUNNING || cur_task == idle_task){
		if (system_queue.head != NULL) {
			/* if current task is periodic then preempt */
			if (cur_task->task_level == PERIODIC){
				cur_task->task_state = READY;
				
			}
			/* if current task is rr then preempt */
			if (cur_task->task_level == RR){
				cur_task->task_state = READY;
				
				push_front(&rr_queue, cur_task);
			}
			cur_task = system_queue.head;
			pop_front(&system_queue);
		} else if (kernel_schedulable() &&  kernel_arrived_periodic_task()){
			cur_task = kernel_arrived_periodic_task();
			/* never pop it from the periodic_queue */
		} else if (rr_queue.head != NULL & rr_queue.head->task_state == READY){
			cur_task = rr_queue.head;
	
			pop_front( &rr_queue );
		} else {
			cur_task = idle_task;
		}
		cur_task->task_state = RUNNING;
	}
	
#ifdef ADD_TO_TRACE
	add_trace(KERNEL_DISPATCH_ID, EXIT, Now());
#endif
	
}

static void kernel_main_loop()
{
	while(1){
#ifdef MDEBUG
		if (SREG & 0b10000000){
			PORTB &= ~_BV(PB3);
		}
		
		volatile task_descriptor_t* temp = periodic_queue.head;
		volatile t = current_ticks % 160;
		PORTB |= 0b11111111;
		while(temp->next_task != NULL){
		
			if ( t < 19 && temp->task_state == RUNNING){
				PORTB &= ~(0b00000001);
			} else if ( t >= 20 && t <= 40 && temp->task_state == RUNNING) {
				PORTB &= ~(0b00000010);
			} else if ( t >= 80 && t <= 100 && temp->task_state == RUNNING){
				PORTB &= ~(0b00000100);
			} else if ( t >= 100 && t <= 120 && temp->task_state == RUNNING) {
				PORTB &= ~(0b00001000);
			}
			temp = temp->next_task;
		}
#endif


		kernel_dispatch();
		exit_kernel();
		
		kernel_handle_request();
	}
}


static void OS_Init( void )
{
	
#ifdef INSTRUMENTING
	DDRB |= _BV(PB5);
	PORTB &= ~_BV(PB5);
#endif
	uint8_t i;
	/* Initialize Memory */
	for ( i = 0; i < MAXPROCESS-1; ++i){
		TASK_DESCRIPTORS[i].f = NULL;
		TASK_DESCRIPTORS[i].task_state = DEAD;
		TASK_DESCRIPTORS[i].next_task = &TASK_DESCRIPTORS[i+1];
	}
	TASK_DESCRIPTORS[MAXPROCESS-1].f = NULL;
	TASK_DESCRIPTORS[MAXPROCESS-1].task_state = DEAD;
	TASK_DESCRIPTORS[MAXPROCESS-1].next_task = NULL;
	
	dead_pool_queue.head = &TASK_DESCRIPTORS[0];
	dead_pool_queue.tail = &TASK_DESCRIPTORS[MAXPROCESS-1];
	
	kernel_task_create_args.f = (voidfuncvoid_ptr) idle;
	kernel_task_create_args.task_level = NULL;
	kernel_create_idle_task();
	
	kernel_task_create_args.f = (voidfuncvoid_ptr) r_main;
	kernel_task_create_args.task_level = SYSTEM;
	kernel_task_create_args.arg = 0;
	kernel_create_system_task();
	
	cur_task = &TASK_DESCRIPTORS[0];
	idle_task = &TASK_DESCRIPTORS[MAXPROCESS];
	
	cur_task->task_state = RUNNING;
	pop_front(&system_queue);
	/* initialize these globals */
	num_events_created = 0;
	current_ticks = 0;
	
	init_clock();
	ENABLE_CLOCK();
	
	kernel_main_loop();
}

static void OS_Abort()
{
	uint8_t i;
	cli();
#ifdef ADD_TO_TRACE
	add_trace(OS_ABORT_ID, ENTER, Now());
#endif

#ifdef TRACING_WITH_LOGIC_ANALYZER
	INIT_LOGIC_ANALYZER 
	while(1){
		for (i = 0; i <= kernel_error; ++i){
			LOGIC_ANALYZER_PORT |= _BV(LOGIC_ANALYZER_ERROR_CHANNEL);
			_delay_us(500);
			LOGIC_ANALYZER_PORT &= ~_BV(LOGIC_ANALYZER_ERROR_CHANNEL);
		}
		_delay_ms(10);
	}
#endif

#ifdef INSTRUMENTING
	PORTB |= ~_BV(PB3);
#endif
	/* disable RTOS tick */
	DISABLE_CLOCK();
	
	/* Flash LEDs to denote error codes */
	/* Or print kernel trace when (some, reset?) button is pressed */
	/* after printing kernel trace, disable all interrupts */
	
	while(1){}
}

int main()
{
	OS_Init();
	while(1){
	/* Watchdog */	
	}
	return 0;
}