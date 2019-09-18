/****
 * @file kernel.h
 * 
 * 
 */
 
#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "os.h"

typedef enum {
	DEAD = 0,
	READY,
	WAITING,
	RUNNING,
} task_state_t;

typedef struct {
	uint8_t task_stack[MAXSTACK];
	uint8_t* volatile task_sp;
	task_state_t task_state;
	
} task_descriptor_t;

#endif

