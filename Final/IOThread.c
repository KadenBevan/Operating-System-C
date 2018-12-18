#include "IOThread.h"

// Thread Function: prunner is used in the CPU.c
// to run individual tasks or metadatas. This one
// is not used for preemption
void* prunner(void* time)
{
	int run_time = (int)time;
	
	// Use Mr. Professors timer
	runTimer(run_time);
	// return null because threads
	return NULL;
}

// Thread Function: prunner is used in the CPU.c
// to run individual tasks or metadatas. This one
// is used for preemption
void* prunner_interrupt(void* data_in)
{
	// Cast the time.
	//int run_time = (int)time;
	
	ThreadData *data = (ThreadData*)data_in;
	
	// Use Mr. Professors timer
	runTimer(data->run_time);
	
	// Done running the timer?
	// Flag the interrupt handler in the CPU.c
	interrupt(data->process, 1);
	
	// return null because threads
	return NULL;
}