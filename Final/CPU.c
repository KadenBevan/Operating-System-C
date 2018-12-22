#include "CPU.h"

int run(Process *process_list, Config *config_data)
{
	char output_buffer[MAX_STRING], timestr[SMALL_STRING];
	
	Process *next_process = NULL;
	update_pcb_time(process_list, config_data);
	next_process = get_next_process(process_list, config_data);
	
	if(next_process == NULL)
	{
		// tell the output handler what process we are selecting
		accessTimer(1, timestr);
		sprintf(output_buffer, "Time: %9s, OS: System Idle\n", timestr);
		handle_output(config_data, output_buffer);
		while(next_process == NULL)
		{
			next_process = get_next_process(process_list, config_data);
		}
	}
	// tell the output handler what process we are selecting
	accessTimer(1, timestr);
	sprintf(output_buffer, "Time: %9s, OS: Process %d selected with %d ms remaining\n", timestr, next_process->PID, next_process->total_cycle);
	handle_output(config_data, output_buffer);
	
	if(next_process->state != RUNNING)
	{
		// set state to running
		run_process(next_process, config_data);
	}
	MetaData *operation = next_process->operation_list;
	while(operation->next != NULL)
	{
		next_process->current_operation = operation;
		if(operation->finished == 1)
		{
			operation = operation->next;
		}
		else
		{
			if(operation->command == 'I' || operation->command == 'O')
			{
				run_io(next_process, operation, config_data);
				operation->finished = 1;
				update_pcb_time(process_list, config_data);
				run(process_list, config_data);
			}
			else if(operation->command == 'P')
			{
				run_p(next_process, operation, config_data);
				update_pcb_time(process_list, config_data);
				run(process_list, config_data);
			}
			else
			{
				operation->finished = 1;
				operation = operation->next;
			}
		}
		if(operation->next == NULL && next_process->state != EXIT)
		{
			exit_process(next_process, config_data);
			run(process_list, config_data);
		}
	}
}

// Run a opertation with the command 'P'.
// This function is basically a spinner unitl the
// entire process is finished or there is an interrupt.
int run_p(Process *current_process, MetaData *current_operation, Config* config_data)
{
	char output_buffer[MAX_STRING], timestr[SMALL_STRING];
	
	if(current_operation->cycle_time > 0)
	{
		accessTimer(1, timestr);
		sprintf(output_buffer, "Time: %9s, OS: Process %i run operation start\n", timestr, current_process->PID);
		handle_output(config_data, output_buffer);
		
		runTimer(config_data->qTime);
		
		sub_cycle(current_operation, config_data->qTime);
		
		if(current_operation->cycle_time <= 0)
		{
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process %i run operation end\n", timestr, current_process->PID);
			handle_output(config_data, output_buffer);
			
			current_operation->finished = 1;
			
			if(current_process->total_cycle <= 0)
			{
				exit_process(current_process, config_data);
			}
			else
			{
				ready_process(current_process, config_data);
			}
			return 0;
		}
		else
		{
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process %i run cycle complete\n", timestr, current_process->PID);
			handle_output(config_data, output_buffer);
			
			ready_process(current_process, config_data);
			return 0;
		}
	}
}

// This function is for commands 'I' and 'O'.
// Spawns IOthreads to wait for IO to finish.
int run_io(Process *current_process, MetaData *current_operation, Config* config_data)
{
	char output_buffer[MAX_STRING], timestr[SMALL_STRING];
	
	if(current_operation->command == 'I')
	{
		// Output
		accessTimer(1, timestr);
		sprintf(output_buffer, "Time: %9s, OS: Process %i %s input start\n", 
				timestr, current_process->PID, current_operation->operation);
		handle_output(config_data, output_buffer);
		
		// Block Current Process
		block_process(current_process, config_data);
		
		// Spawn Threads
		IOThreadData *data = malloc(sizeof(IOThreadData));
		data->process = current_process;
		data->run_time = current_operation->cycle_time * config_data->IOCT;
		pthread_t thread;
		pthread_create(&thread, NULL, io_thread, (void*)data);
	}
	else if(current_operation->command == 'O')
	{
		// Output
		accessTimer(1, timestr);
		sprintf(output_buffer, "Time: %9s, OS: Process %i %s output start\n", 
				timestr, current_process->PID, current_operation->operation);
		handle_output(config_data, output_buffer);
		
		// Block Current Process
		block_process(current_process, config_data);
		
		// Spawn Threads
		IOThreadData *data = malloc(sizeof(IOThreadData));
		data->process = current_process;
		data->run_time = current_operation->cycle_time * config_data->IOCT;
		pthread_t thread;		
		pthread_create(&thread, NULL, io_thread, (void*)data);
	}
	return 0;
}

// Function: interrupt() returns the interrupter if there is one.
//			 not safe to be called with mode set to write from outside
//			 IO Thread.
// Interrupt logic is as follows:
// mode == 1 (write) set the interrupter. Called by IO thread
// mode == 2 (read) return the current interrupter
// mode == 3 (reset) reset the interrupter
Process* interrupt(Process *interrupter, int mode)
{
	static Process *ret = NULL;
	Process* p_interrupter = interrupter;
	if(p_interrupter != NULL && mode == 1)
	{
		pthread_mutex_lock(&INTERRUPT_MUTEX);
		ret = p_interrupter;
		pthread_mutex_unlock(&INTERRUPT_MUTEX);
		return ret;
	}
	else if(mode == 2)
	{
		
		return ret;
	}
	else if(mode == 3)
	{
		ret = NULL;
	}
}