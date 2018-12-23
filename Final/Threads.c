#include "Threads.h"
#include "Driver.h"

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
void* io_thread(void* data_in)
{
	IOThreadData *data = (IOThreadData*)data_in;
	
	// Use Mr. Professors timer
	runTimer(data->run_time);
	
	// Done running the timer?
	// Flag the interrupt handler in the CPU.c
	interrupt(data->process, 1);
	
	// return null because threads
	return NULL;
}

void* interrupt_watcher(void* data_in)
{
	WatchThreadData *data = (WatchThreadData*)data_in;
	
	char output_buffer[MAX_STRING], timestr[SMALL_STRING];
	
	// Set a Process pointer to the value of interrupt() with the flag set to read
	Process *interrupter = interrupt(NULL, 2);
	
	// While out interrupter pointer is NULL and no process has been set to READY
	while(interrupter == NULL)
	{
		// Update the interrupter pointer.
		interrupter = interrupt(NULL, 2);
	}
	// If the interrupter gets set to something.
	if(interrupter != NULL)
	{
		pthread_mutex_lock(&INTERRUPT_MUTEX);
		// Immediatly reset the interrupt with the flag set to reset.
		interrupt(NULL, 3);
		pthread_mutex_unlock(&INTERRUPT_MUTEX);
		
		// Output what process interrupted.
		accessTimer(1, timestr);
		sprintf(output_buffer, "#\n#\n#\n#\nTime: %9s, OS: Interrupt by Process %i\n#\n#\n#\n#\n#", timestr, interrupter->PID);
		handle_output(data->config, output_buffer);
		
		// Output what task interrupted
		accessTimer(1, timestr);
		sprintf(output_buffer, "Time: %9s, OS: Process %i %s IO end\n", timestr, interrupter->PID, interrupter->current_operation->operation);
		handle_output(data->config, output_buffer);
		if(interrupter->total_cycle > 1)
		{
			// Ready the interrupter
			ready_process(interrupter, data->config);
		}
		else
		{
			exit_process(interrupter, data->config);
		}
		
		init_interrupt_watcher(data->config);
	}
	return NULL;
}

int init_interrupt_watcher(Config *config_data)
{
	pthread_t thread;
	WatchThreadData *data = malloc(sizeof(WatchThreadData));
	data->config = config_data;
	pthread_create(&thread, NULL, interrupt_watcher, (void*)data);
	return 0;
}