#include "CPU.h"

// Function: Start() called from the Driver.c to initiate the running of Processes.
// Basically we decide if we are going to use preemption or not, get the next Process
// based on what the Schedular.c says and run it.
int start(Process *process_list, Config *config_data)
{
	// For output bound data.
	char output_buffer[MAX_STRING], timestr[SMALL_STRING];
	
	// A pointer to hold the next process.
	Process *next_process;
	
	// Assign it down here to be pretty.
	next_process = get_next_process(process_list, config_data);
	
	// while my scheduler finds a process to run.
	// if its null it most likly is done
	if(next_process != NULL)
	{
		// Preemptive?
		if(strncmp(config_data->scheduleCode, "SRTF-P", 5) == 0
		|| strncmp(config_data->scheduleCode, "FCFS-P", 5) == 0
		|| strncmp(config_data->scheduleCode, "RR-P", 2) == 0)
		{
			// tell the output handler what process we are selecting
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process %d selected with %d ms remaining\n", timestr, next_process->PID, next_process->total_cycle);
			handle_output(config_data, output_buffer);
			
			// run the process
			run_process_p(next_process, process_list, config_data);
			next_process = get_next_process(process_list, config_data);
		}
		// not preemptive
		else
		{
			// tell the output handler what process we are selecting
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process %d selected with %d ms remaining\n", timestr, next_process->PID, next_process->total_cycle);
			handle_output(config_data, output_buffer);
			
			// We dont need to update the total times so just run
			run_process(next_process, config_data);
			next_process = get_next_process(process_list, config_data);
		}
	}
	// If next_process is NULL the system is idle so we're going to wait for one of the
	// IO threads to come back and flag the interrupter
	else
	{
		// output that the system is idle
		accessTimer(1, timestr);
		sprintf(output_buffer, "Time: %9s, OS: System/CPU idle\n", timestr);
		handle_output(config_data, output_buffer);
		
		// Set a Process pointer to the value of interrupt() with the flag set to read
		Process *interrupter = interrupt(NULL, 2);
		// While out interrupter pointer is NULL and no process has been set to READY
		while(next_process == NULL && interrupter == NULL)
		{
			// Update the next_process to see if any processes have been set to the READY state.
			next_process = get_next_process(process_list, config_data);
			// Also update the interrupter pointer.
			interrupter = interrupt(NULL, 2);
		}
		// If the interrupter gets set to something.
		if(interrupter != NULL)
		{
			// Immediatly reset the interrupt with the flag set to reset.
			interrupt(NULL, 3);
			// Output what process interrupted.
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Interrupt by Process %i\n", timestr, interrupter->PID);
			handle_output(config_data, output_buffer);
			
			// Output what task interrupted
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process %i %s IO end\n", timestr, interrupter->PID, interrupter->current_operation->operation);
			handle_output(config_data, output_buffer);
			
			// set the interrupting process to READY.
			ready_process(process_list, interrupter->PID, config_data);
		}
		
		// Recurse with the process list that may have been updated.
		start(process_list, config_data);
	}
	// Not much error checking so return 0.
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
	if(interrupter != NULL && mode == 1)
	{
		ret = interrupter;
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


// Function: runs a process chosen by the next_process() function called in the
// 		Start() function above.
// this function does NOT deal with preemptive scheduling.
int run_process(Process *process, Config *config_data)
{
	// For output bound data.
	char output_buffer[MAX_STRING], timestr[SMALL_STRING];
	
	// Set the process to the running state and output that.
	accessTimer(1, timestr);
	process->state = RUNNING;
	sprintf(output_buffer, "Time: %9s, OS: Process %d set in RUNNING state\n", timestr, process->PID);
	handle_output(config_data, output_buffer);
	
	// Declare a pointer to hold the MetaData string from a process.
	MetaData *operation;
	// assign it
	operation = process->operation_list;
	
	// While we are not at the end of our operation, and the current task does not
	// have an end operation.
	while(operation->next != NULL && strncmp(operation->operation, "end", 2) != 0)
	{
		// Declare a thread
		pthread_t thread; 
		// Int to hold how long we are going to run for.
		int time_to_run = 0;
		
		// if the task is MMU bound
		if (operation->command == 'M')
		{
			int original, segment, base, allocation;
			original = operation->cycle_time;
			segment = original / 1000000;
			base = (original - (segment * 1000000))/1000;
			allocation = original - ((segment * 1000000) + (base * 1000));
			if (str_n_cmp(operation->operation, "allocate", 4) == 0)
			{
				accessTimer(1, timestr);
				sprintf(output_buffer, "Time: %9s, OS: Process %i MMU Allocation: %i/%i/%i start\n", timestr, process->PID, segment, base, allocation);
				handle_output(config_data, output_buffer);
				
				// call MMU
				if (allocate_memory(segment, base, allocation) == 0)
				{
					accessTimer(1, timestr);
					sprintf(output_buffer, "Time: %9s, OS: Process %i MMU Allocation: Successful\n", timestr, process->PID);
					handle_output(config_data, output_buffer);
				}
				else
				{
					accessTimer(1, timestr);
					sprintf(output_buffer, "Time: %9s, OS: Process %i MMU Allocation: Failed\n", timestr, process->PID);
					handle_output(config_data, output_buffer);
					
					accessTimer(1, timestr);
					sprintf(output_buffer, "Time: %9s, OS: Process %i Segmentation Fault - Process ended\n", timestr, process->PID);
					handle_output(config_data, output_buffer);
				}
			}
			else if (str_n_cmp(operation->operation, "access", 4) == 0)
			{
				accessTimer(1, timestr);
				sprintf(output_buffer, "Time: %9s, OS: Process %i MMU Access: %i/%i/%i start\n", timestr, process->PID, segment, base, allocation);
				handle_output(config_data, output_buffer);
				if (access_memory(segment, base, allocation) == 0)
				{
					accessTimer(1, timestr);
					sprintf(output_buffer, "Time: %9s, OS: Process %i MMU Access: Successful\n", timestr, process->PID);
					handle_output(config_data, output_buffer);
				}
				else
				{
					accessTimer(1, timestr);
					sprintf(output_buffer, "Time: %9s, OS: Process %i MMU Access: Failed\n", timestr, process->PID);
					handle_output(config_data, output_buffer);
					
					accessTimer(1, timestr);
					sprintf(output_buffer, "Time: %9s, OS: Process %i Segmentation Fault - Process ended\n", timestr, process->PID);
					handle_output(config_data, output_buffer);
				}
			}
		}
		
		// If the current task is a Input command
		if(operation->command == 'I')
		{
			// Sent the start text output bound.
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process: %d, %s input start\n", timestr, process->PID, operation->operation);
			handle_output(config_data, output_buffer);
			
			// Assing int to the correct run time.
			// In a non-preemptive fashion.
			time_to_run = operation->cycle_time * config_data->IOCT;
			
			// Create our thread to run. The prunner() function can be found
			// in the Driver.c
			pthread_create(&thread, NULL, prunner, (void*)time_to_run);
			pthread_join(thread, NULL);
			
			// Show that we are done with the current task output bound.
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process: %d, %s input end\n", timestr, process->PID, operation->operation);
			handle_output(config_data, output_buffer);
			
			// flag the task as finished
			// this helps especially for preemption.
			operation->finished = 1;
		}
		// If the current task is an Output command
		else if(operation->command == 'O')
		{
			// Sent the start text output bound.
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process: %d, %s output start\n", timestr, process->PID, operation->operation);
			handle_output(config_data, output_buffer);
			
			// Assing int to the correct run time.
			// In a non-preemptive fashion.
			time_to_run = operation->cycle_time * config_data->IOCT;
			
			// Create our thread to run. The prunner() function can be found
			// in the Driver.c
			pthread_create(&thread, NULL, prunner, (void*)time_to_run);
			pthread_join(thread, NULL);
			
			// Show that we are done with the current task output bound.
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process: %d, %s output end\n", timestr, process->PID, operation->operation);
			handle_output(config_data, output_buffer);
			
			// flag the task as finished
			// this helps especially for preemption.
			operation->finished = 1;
		}
		// If the operation command is a P command.
		else if(operation->command == 'P')
		{
			// Send start output bound
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process: %d, run operation start\n", timestr, process->PID);
			handle_output(config_data, output_buffer);
			
			// Use PCT here for P commands
			time_to_run = operation->cycle_time * config_data->PCT;
			
			// some more threads
			pthread_create(&thread, NULL, prunner, (void*)time_to_run);
			pthread_join(thread, NULL);
			
			// output bound ending the task
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process: %d, run operation end\n", timestr, process->PID);
			handle_output(config_data, output_buffer);
			
			// Aaand flag that its done
			operation->finished = 1;
		}
		// Done with the task? get a new one.
		operation = operation->next;
	}
	// All out of tasks? set the Process to the exit state
	// and sent that output bound
	process->state = EXIT;
	accessTimer(1, timestr);
	sprintf(output_buffer, "Time: %9s, OS: Process %d ended and set in EXIT state\n", timestr, process->PID);
	handle_output(config_data, output_buffer);
	
	// No significant error checking here so just return 0
	return 0;
}

// Function: run_process_p() is the about the same as the run_process() but is used for
// preemption.
int run_process_p(Process *process, Process *process_list, Config *config_data)
{
	
	// For output bound data.
	char output_buffer[MAX_STRING], timestr[SMALL_STRING];
	
	// Declare a pointer to hold the MetaData string from a process.
	MetaData *operation;
	
	// Then.. assign it.
	operation = process->operation_list;
	

	// Set the process to the running state and output that.
	accessTimer(1, timestr);
	process->state = RUNNING;
	sprintf(output_buffer, "Time: %9s, OS: Process %d set in RUNNING state\n", timestr, process->PID);
	handle_output(config_data, output_buffer);
	
	Process *interrupter = interrupt(NULL, 2);
	if(interrupter != NULL)
	{
		// reset interrupt
		interrupt(NULL, 3);
		accessTimer(1, timestr);
		sprintf(output_buffer, "Time: %9s, OS: Process %i interrupted by Process %i\n", timestr, process->PID, interrupter->PID);
		handle_output(config_data, output_buffer);
		
		accessTimer(1, timestr);
		sprintf(output_buffer, "Time: %9s, OS: Process %i %s IO end\n", timestr, interrupter->PID, interrupter->current_operation->operation);
		handle_output(config_data, output_buffer);
		
		// set the interrupting process to READY.
		ready_process(process_list, interrupter->PID, config_data);
	}
	
	// While we are not at the end of our operation, and the current task does not
	// have an end operation.	
	while(operation->next != NULL && strncmp(operation->operation, "end", 2) != 0)
	{
		// Declare a thread
		pthread_t thread; 
		// Int to hold how long we are going to run for.
		int time_to_run = 0;
		// if the current command is an Input

		if(operation->finished == -1)
		{
			process->current_operation = operation;

			// if the task is MMU bound
			if (operation->command == 'M')
			{
				int original, segment, base, allocation;
				original = operation->cycle_time;
				segment = original / 1000000;
				base = (original - (segment * 1000000))/1000;
				allocation = original - ((segment * 1000000) + (base * 1000));
				if (str_n_cmp(operation->operation, "allocate", 4) == 0)
				{
					accessTimer(1, timestr);
					sprintf(output_buffer, "Time: %9s, OS: Process %i MMU Allocation: %i/%i/%i start\n", timestr, process->PID, segment, base, allocation);
					handle_output(config_data, output_buffer);
					
					// call MMU
					if (allocate_memory(segment, base, allocation) == 0)
					{
						accessTimer(1, timestr);
						sprintf(output_buffer, "Time: %9s, OS: Process %i MMU Allocation: Successful\n", timestr, process->PID);
						handle_output(config_data, output_buffer);
					}
					else
					{
						accessTimer(1, timestr);
						sprintf(output_buffer, "Time: %9s, OS: Process %i MMU Allocation: Failed\n", timestr, process->PID);
						handle_output(config_data, output_buffer);
						
						accessTimer(1, timestr);
						sprintf(output_buffer, "Time: %9s, OS: Process %i Segmentation Fault - Process ended\n", timestr, process->PID);
						handle_output(config_data, output_buffer);
					}
				}
				else if (str_n_cmp(operation->operation, "access", 4) == 0)
				{
					accessTimer(1, timestr);
					sprintf(output_buffer, "Time: %9s, OS: Process %i MMU Access: %i/%i/%i start\n", timestr, process->PID, segment, base, allocation);
					handle_output(config_data, output_buffer);
					if (access_memory(segment, base, allocation) == 0)
					{
						accessTimer(1, timestr);
						sprintf(output_buffer, "Time: %9s, OS: Process %i MMU Access: Successful\n", timestr, process->PID);
						handle_output(config_data, output_buffer);
					}
					else
					{
						accessTimer(1, timestr);
						sprintf(output_buffer, "Time: %9s, OS: Process %i MMU Access: Failed\n", timestr, process->PID);
						handle_output(config_data, output_buffer);
						
						accessTimer(1, timestr);
						sprintf(output_buffer, "Time: %9s, OS: Process %i Segmentation Fault - Process ended\n", timestr, process->PID);
						handle_output(config_data, output_buffer);
					}
				}
				operation->finished = 1;
			}
			if(operation->command == 'I')
			{
				accessTimer(1, timestr);
				sprintf(output_buffer, "Time: %9s, OS: Process %i %s input start\n", timestr, process->PID, operation->operation);
				handle_output(config_data, output_buffer);
				
				ThreadData *data = malloc(sizeof(ThreadData));
				data->process = process;
				data->run_time = operation->cycle_time * config_data->IOCT;
				
				//process->total_cycle -= (operation->cycle_time * config_data->IOCT);
				
				pthread_create(&thread, NULL, prunner_interrupt, (void*)data);
				//pthread_join(thread, NULL);
				
				process->state = BLOCKED;
				accessTimer(1, timestr);
				sprintf(output_buffer, "Time: %9s, OS: Process %i set in BLOCKED state\n", timestr, process->PID);
				handle_output(config_data, output_buffer);
				
				operation->finished = 1;
				start(process_list, config_data);
			}
			// If the current time is an Output
			else if(operation->command == 'O')
			{
				accessTimer(1, timestr);
				sprintf(output_buffer, "Time: %9s, OS: Process %i %s output start\n", timestr, process->PID, operation->operation);
				handle_output(config_data, output_buffer);
				
				//process->total_cycle -= (operation->cycle_time * config_data->IOCT);
				
				ThreadData *data = malloc(sizeof(ThreadData));
				data->process = process;
				data->run_time = operation->cycle_time * config_data->IOCT;
				
				pthread_create(&thread, NULL, prunner_interrupt, (void*)data);
				//pthread_join(thread, NULL);
				
				process->state = BLOCKED;
				accessTimer(1, timestr);
				sprintf(output_buffer, "Time: %9s, OS: Process %i set in BLOCKED state\n", timestr, process->PID);
				handle_output(config_data, output_buffer);
				
				operation->finished = 1;
				start(process_list, config_data);
			}
			// If the current command is run
			else if(operation->command == 'P')
			{
				accessTimer(1, timestr);
				sprintf(output_buffer, "Time: %9s, OS: Process %i run operation start\n", timestr, process->PID);
				handle_output(config_data, output_buffer);
				int run_time = operation->cycle_time*config_data->PCT;
				while(run_time > 0)
				{
					time_to_run = config_data->qTime;
					runTimer(time_to_run);
					
					interrupter = interrupt(NULL, 2);
					
					// check interrupt
					if(interrupter != NULL)
					{
						// reset interrupt
						interrupt(NULL, 3);
						accessTimer(1, timestr);
						sprintf(output_buffer, "Time: %9s, OS: Process %i interrupted by Process %i\n", timestr, process->PID, interrupter->PID);
						handle_output(config_data, output_buffer);
						
						accessTimer(1, timestr);
						sprintf(output_buffer, "Time: %9s, OS: Process %i %s IO end\n", timestr, interrupter->PID, interrupter->current_operation->operation);
						handle_output(config_data, output_buffer);
						
						// set the interrupting process to READY.
						ready_process(process_list, interrupter->PID, config_data);
					}
					run_time -= config_data->qTime;
				}
				
				//process->total_cycle -= config_data->PCT * operation->cycle_time;
				
				operation->finished = 1;
				accessTimer(1, timestr);
				sprintf(output_buffer, "Time: %9s, OS: Process %i run operation end\n", timestr, process->PID);
				handle_output(config_data, output_buffer);
				
				accessTimer(1, timestr);
				process->state = READY;
				sprintf(output_buffer, "Time: %9s, OS: Process %i set in READY state\n", timestr, process->PID);
				handle_output(config_data, output_buffer);
				
				start(process_list, config_data);
			}
		}
		if(strncmp(operation->next->operation, "end", 2) == 0 && process->state != EXIT)
		{
			process->state = EXIT;
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process %i set in EXIT state\n", timestr, process->PID);
			handle_output(config_data, output_buffer);
			
			start(process_list, config_data);
		}
		operation = operation->next;	
	}
	return 0;
}