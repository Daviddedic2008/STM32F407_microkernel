
/*
 * task.c
 *
 *  Created on: Nov 20, 2025
 *      Author: david
 */
#define maxTasks 30
#define taskSz 128

static uint32_t stackMem[maxTasks][taskSz];

typedef struct{
	uint32_t* psp;
	uint32_t* next_psp;
}task;

task tasks[maxTasks];

task* current_task_pointer; // used in pendSV handler

uint32_t tasksAllocated;

#define CURIDX (current_task_pointer - tasks)

void initScheduler(){
	tasksAllocated = 0;
	for(task* t = tasks; t < tasks + maxTasks; t++){
		t->psp = (void*)0; // init to null(free)
	}
}

void freeTask(task* t){
	t->next_psp = (void*)0;
}

void initTask(void(*fn)(void), const uint32_t idx, const task* nextTask){
	// upon each task init, we must set up the "psp" in the stackmem arr
	// we are simulating an exception stack frame so when this pointer is passed to asm subroutine it works
	// this init code lets the asm routine know that it needs to run in psp mode
	uint32_t* stackPointer = &(stackMem[idx][taskSz]); // start at top of stack + 1
	*(--stackPointer) = 0x01000000; // some thing about a thumb bit having to be set in the first slot
	*(--stackPointer) = (uint32_t)fn; // pass function location;
	*(--stackPointer) = 0xFFFFFFFD; // LR — special EXC_RETURN value: return to Thread mode using PSP
	*(--stackPointer) = 0; // R12 — not used here
	*(--stackPointer) = 0; // R3
	*(--stackPointer) = 0; // R2
	*(--stackPointer) = 0; // R1
	*(--stackPointer) = 0; // R0 — argument to task function (none here)
	for(uint32_t i = 0; i < 8; i++, stackPointer--){*stackPointer = 0; /*init to zero ig*/} // reserve more of the stack for callee saved regs
	tasks[idx].psp = stackPointer; // save initial stack position after loading everything into stack
	tasks[idx].next_psp = nextTask;
}

task* findPrevActiveTask(task* t){
	uint32_t tasksChecked = 0;
	do{
		t--;
		tasksChecked++;
		if(t < tasks){t = tasks + maxTasks-1;}
	}while(t == (void*)0 && tasksChecked < maxTasks);
	return t;
}

task* findNextActiveTask(task* t){
	uint32_t tasksChecked = 0;
	do{
		t++;
		tasksChecked++;
		if(t >= tasks+maxTasks){t = tasks;}
	}while(t == (void*)0 && tasksChecked < maxTasks);
	return t;
}

void addTask(void(*fn)(void)){
	if(tasksAllocated == 0){
		initTask(fn, maxTasks-1, &tasks[maxTasks-1]);
		return; // base
	}
	tasksAllocated++;
	for(task* t = tasks + maxTasks-1; t > 0; t--){
		if(t->psp == (void*)0){
			task* n = findNextActiveTask(t);
			task* p = findPrevActiveTask(t);
			// found first open spot with a full task next
			initTask(fn, t - tasks, n); // if its only task, just set next task to itself
			p->next_psp = t;
			break;
		}
	}
}

void ret(){
	// must be called before regular return statement in all tasks
	task* p = findPrevActiveTask(current_task_pointer);
	task* n = findNextActiveTask(current_task_pointer);
	freeTask(current_task_pointer);
	current_task_pointer = n;
	p->next_psp = n;
}

void yield(){
	SCB_ISR = 1 << 28; // trigger pendSV exception, and therefore interrupt to switch tasks
	__asm("isb"); // flush pipeline to ensure pendSV is triggered(I read this can help but isnt nescessary)
}

