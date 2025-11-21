/*
 * task.h
 *
 *  Created on: Nov 20, 2025
 *      Author: david
 *
 *  round robin coop scheduler
 *  it is pure coop scheduler, tasks can only yield and return
 *  each task holds a pointer to the next one
 *  the scheduler uses the cortex's psp(process stack pointer) instead of main sp
 *  WHY PENDSV:
 *  	-the pendSV exception is lowest priority
 *  	-its an exception so it auto restores some regs
 *  	-software-only callable
 *  	-runs in handler mode, so it runs on the main stack:
 *  		this means it never interferes with tasks
 *  		most exceptions have this feature i think?
 *  	-priority access to special registers like psp register
 *  when pendSV is triggered, the asm handler automatically moves the psp to the next task's stack space
 *  tasks can be added, and are auto removed when the tasks function returns
 *  tasks cannot have arguments
 *  context switches save and restore all callee saved regs
 *  each task has a static max stack size, currently 128 32 bit words
 *  there is a static max num of tasks, currently set to 30
 */

#ifndef SOURCES_SCHEDULER_TASK_H_
#define SOURCES_SCHEDULER_TASK_H_



#endif /* SOURCES_GENERAL_SCHEDULER_TASK_H_ */
