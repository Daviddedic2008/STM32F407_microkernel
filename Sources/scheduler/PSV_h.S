/*
 * pendSVHandler.S
 *
 *  Created on: Nov 20, 2025
 *      Author: david
 */

 // make sure to use unified syntax and allow thumb2 ops
.syntax unified
.thumb

.extern current_task_pointer

/*
	this handler system lets you cycle tasks via psp
*/

.global begin_pendSV
begin_pendSV:
	cpsid i // disable interrupts
	ldr r0, =current_task_pointer // load addr of current task pointer
	ldr r0, [r0] // deref
	msr psp, r0 // load current task pointer into process stack pointer
	mov r0, #2 // load 2 into register to write to CONTROL register
	msr CONTROL, r0 // set mcu to psp mode
	isb // flush pipeline to enforce new mode(not required i think?)
	cpsie i // enable interrupts
	bx lr // return, doesnt automatically default to psp

.global pendSV_Handler
pendSV_Handler:
	cpsid i // disable interrupts
	mrs r0, psp // move from special register psp(stack pointer when in thread mode)
	ldr r1, =current_task_pointer // load address of current_task_pointer
	ldr r2, [r1] // dereference and obtain actual current_task_pointer
	stmdb r0!, {r4-r11} // push callee registers onto the stack, increment r0 by 8 as well
	str r0, [r2] // store the current stack address into current_task_pointer->sp (stack pointer of current task)
	ldr r2, [r2, #4] // this loads the next task pointer into r2
	str r2, [r1] // update current task pointer to point to the next task pointer
	ldr r0, [r2] // load new tasks stack pointer into r0
	ldmia r0!, {r4-r11} // pop callee registers for the next task thats being called
	msr psp, r0 // set the program stack pointer to the beginning of next task
	cpsie i // enable interrupts
	bx lr // return from exception (tell it to return in thread mode and use psp, not msp)
