/**
 * @file   error_code.h
 *
 * 
 *        
 *        
 *
 * CSC 460/560 Real Time Operating Systems
 *
 * @author Rayhan Rahman
 *
 */
#ifndef __ERROR_CODE_H__
#define __ERROR_CODE_H__

typedef enum {
	
	/** Number of events exceeded MAXEVENT */
	EXCEEDED_MAX_EVENTS,
	
	/** Number of task exceeded MAXPROCESS */
	EXCEEDED_MAX_PROCESS,
	
	/** A periodic task is waiting on an event */
	PERIODIC_TASK_WAITING,
	
	/** Periodic tasks overlap */
	PERIODIC_TASKS_OVERLAP,
	
	/** A periodic task exceeded its WCET */
	EXCEEDED_WCET,
	
	/** Multiple tasks (system, rr) is waiting on the same event */
	MULTIPLE_TASKS_WAITING,
	
	/** WCET of a periodic task is greater than its period*/
	WCET_GREATER,
	
	/** Stack overflow occurred */
	STACK_OVERFLOW,
	
	/** Stack underflow occurred */
	STACK_UNDERFLOW,
	
	/** Unknown error occurred */
	UNKNOWN_ERROR

} ERROR;

#endif
