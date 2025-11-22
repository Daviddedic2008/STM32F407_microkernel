/*
 * kernels.c
 *
 *  Created on: Nov 21, 2025
 *      Author: david
 */

#include "task.h"

/*
 * example kernel here:
 * notice the use of yeild() and ret()


uint32_t arr1[5] = {1, 2, 3, 4, 5};
uint32_t arr2[5] = {5, 4, 3, 2, 1};

// yielding in "long" loop to let other loops execute concurrently
void exampleKernel(){
	for(uint8_t idx = 0; idx < 5; idx++){
		arr1[idx] += arr2[idx];
		yield(); // yield control to next task in order
	}
	ret(); // de allocate current task (this one!)
}

*/
