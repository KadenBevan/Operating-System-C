#include "CPU.h"

// This STATIC not GLOBAL variable is used to flag an interrupt
// from the prunner_interrupt() thread function found in the Driver.c
static int interrupt_flag = -1;

// Function: Start() called from the Driver.c to initiate the running of Processes.
// Basically we decide if we are going to use preemption or not, get the next Process
// based on what the Schedular.c says and run it.
int start(Process *process, Config *config_data)
{
	// For output bound data.
	char output_buffer[MAX_STRING], timestr[SMALL_STRING];
	
	// A pointer to hold the next process.
	Process *next_process;
	
	// Assign it down here to be pretty.
	next_process = get_next_process(process, config_data);
	
	// while my scheduler finds a process to run.
	// if its null it most likly is done
	while(next_process != NULL)
	{
		// Preemptive?
		if(strncmp(config_data->scheduleCode, "SRTF-P", 5) == 0
		|| strncmp(config_data->scheduleCode, "FCFS-P", 5) == 0
		|| strncmp(config_data->scheduleCode, "RR-P", 2) == 0)
		{
			// get the new total cycle times of the processes
			schedule_processes(process, config_data);
			
			// tell the output handler what process we are selecting
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process %d selected with %d ms remaining\n", timestr, next_process->PID, next_process->total_cycle);
			handle_output(config_data, output_buffer);
			
			// run the process
			run_process_p(next_process, config_data);
			next_process = get_next_process(process, config_data);
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
			next_process = get_next_process(process, config_data);
		}
	}
	// Not much error checking so return 0.
	return 0;
}

// get the next process based on schedule code

// run that process one cycle based on quantum time

// check for interrupt

// if interrupted
//		update meta's cycle time
//		update Processes total_cycle time

// rinse and repeate

int interrupt()
{
	interrupt_flag = 1;
	return 0;
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
	MetaData *tasks;
	// assign it
	tasks = process->operation_list;
	
	// While we are not at the end of our tasks, and the current task does not
	// have an end operation.
	while(tasks->next != NULL && strncmp(tasks->operation, "end", 2) != 0)
	{
		// Declare a thread
		pthread_t thread; 
		// Int to hold how long we are going to run for.
		int time_to_run = 0;
		
		// if the task is MMU bound
		if (tasks->command == 'M')
		{
			int original, segment, base, allocation;
			original = tasks->cycle_time;
			segment = original / 1000000;
			base = (original - (segment * 1000000))/1000;
			allocation = original - ((segment * 1000000) + (base * 1000));
			if (str_n_cmp(tasks->operation, "allocate", 4) == 0)
			{
				accessTimer(1, timestr);
				sprintf(output_buffer, "Time: %9s, OS: Process %i, MMU Allocation: %i/%i/%i start\n", timestr, process->PID, segment, base, allocation);
				handle_output(config_data, output_buffer);
				
				// call MMU
				if (allocate_memory(segment, base, allocation) == 0)
				{
					accessTimer(1, timestr);
					sprintf(output_buffer, "Time: %9s, OS: Process %i, MMU Allocation: Successful\n", timestr, process->PID);
					handle_output(config_data, output_buffer);
				}
				else
				{
					accessTimer(1, timestr);
					sprintf(output_buffer, "Time: %9s, OS: Process %i, MMU Allocation: Failed\n", timestr, process->PID);
					handle_output(config_data, output_buffer);
					
					accessTimer(1, timestr);
					sprintf(output_buffer, "Time: %9s, OS: Process %i, Segmentation Fault - Process ended\n", timestr, process->PID);
					handle_output(config_data, output_buffer);
				}
			}
			else if (str_n_cmp(tasks->operation, "access", 4) == 0)
			{
				accessTimer(1, timestr);
				sprintf(output_buffer, "Time: %9s, OS: Process %i, MMU Access: %i/%i/%i start\n", timestr, process->PID, segment, base, allocation);
				handle_output(config_data, output_buffer);
				if (access_memory(segment, base, allocation) == 0)
				{
					accessTimer(1, timestr);
					sprintf(output_buffer, "Time: %9s, OS: Process %i, MMU Access: Successful\n", timestr, process->PID);
					handle_output(config_data, output_buffer);
				}
				else
				{
					accessTimer(1, timestr);
					sprintf(output_buffer, "Time: %9s, OS: Process %i, MMU Access: Failed\n", timestr, process->PID);
					handle_output(config_data, output_buffer);
					
					accessTimer(1, timestr);
					sprintf(output_buffer, "Time: %9s, OS: Process %i, Segmentation Fault - Process ended\n", timestr, process->PID);
					handle_output(config_data, output_buffer);
				}
			}
		}
		
		// If the current task is a Input command
		if(tasks->command == 'I')
		{
			// Sent the start text output bound.
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process: %d, %s input start\n", timestr, process->PID, tasks->operation);
			handle_output(config_data, output_buffer);
			
			// Assing int to the correct run time.
			// In a non-preemptive fashion.
			time_to_run = tasks->cycle_time * config_data->IOCT;
			
			// Create our thread to run. The prunner() function can be found
			// in the Driver.c
			pthread_create(&thread, NULL, prunner, (void*)time_to_run);
			pthread_join(thread, NULL);
			
			// Show that we are done with the current task output bound.
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process: %d, %s input end\n", timestr, process->PID, tasks->operation);
			handle_output(config_data, output_buffer);
			
			// flag the task as finished
			// this helps especially for preemption.
			tasks->finished = 1;
		}
		// If the current task is an Output command
		else if(tasks->command == 'O')
		{
			// Sent the start text output bound.
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process: %d, %s output start\n", timestr, process->PID, tasks->operation);
			handle_output(config_data, output_buffer);
			
			// Assing int to the correct run time.
			// In a non-preemptive fashion.
			time_to_run = tasks->cycle_time * config_data->IOCT;
			
			// Create our thread to run. The prunner() function can be found
			// in the Driver.c
			pthread_create(&thread, NULL, prunner, (void*)time_to_run);
			pthread_join(thread, NULL);
			
			// Show that we are done with the current task output bound.
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process: %d, %s output end\n", timestr, process->PID, tasks->operation);
			handle_output(config_data, output_buffer);
			
			// flag the task as finished
			// this helps especially for preemption.
			tasks->finished = 1;
		}
		// If the tasks command is a P command.
		else if(tasks->command == 'P')
		{
			// Send start output bound
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process: %d, run operation start\n", timestr, process->PID);
			handle_output(config_data, output_buffer);
			
			// Use PCT here for P commands
			time_to_run = tasks->cycle_time * config_data->PCT;
			
			// some more threads
			pthread_create(&thread, NULL, prunner, (void*)time_to_run);
			pthread_join(thread, NULL);
			
			// output bound ending the task
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process: %d, run operation end\n", timestr, process->PID);
			handle_output(config_data, output_buffer);
			
			// Aaand flag that its done
			tasks->finished = 1;
		}
		// Done with the task? get a new one.
		tasks = tasks->next;
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
int run_process_p(Process *process, Config *config_data)
{
	// For output bound data.
	char output_buffer[MAX_STRING], timestr[SMALL_STRING];
	
	// Set the process to the running state and output that.
	accessTimer(1, timestr);
	process->state = RUNNING;
	sprintf(output_buffer, "Time: %9s, OS: Process %d set in RUNNING state\n", timestr, process->PID);
	handle_output(config_data, output_buffer);
	
	// Declare a pointer to hold the MetaData string from a process.
	MetaData *tasks;
	
	// Then.. assign it.
	tasks = process->operation_list;
	
	// While we are not at the end of our tasks, and the current task does not
	// have an end operation.	
	while(tasks->next != NULL && strncmp(tasks->operation, "end", 2) != 0)
	{
		// Declare a thread
		pthread_t thread; 
		// Int to hold how long we are going to run for.
		int time_to_run = 0;
		// if the current command is an Input
		if(tasks->command == 'I')
		{
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process: %d, %s input start\n", timestr, process->PID, tasks->operation);
			handle_output(config_data, output_buffer);
			
			time_to_run = tasks->cycle_time * config_data->IOCT;
			
			pthread_create(&thread, NULL, prunner_interrupt, (void*)time_to_run);
			pthread_join(thread, NULL);
			
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process: %d, %s input end\n", timestr, process->PID, tasks->operation);
			handle_output(config_data, output_buffer);
			
			tasks->finished = 1;
		}
		// If the current time is an Output
		else if(tasks->command == 'O')
		{
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process: %d, %s output start\n", timestr, process->PID, tasks->operation);
			handle_output(config_data, output_buffer);
			
			time_to_run = tasks->cycle_time * config_data->IOCT;
			
			pthread_create(&thread, NULL, prunner_interrupt, (void*)time_to_run);
			pthread_join(thread, NULL);
			
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process: %d, %s output end\n", timestr, process->PID, tasks->operation);
			handle_output(config_data, output_buffer);
			
			tasks->finished = 1;
		}
		// If the current command is run
		else if(tasks->command == 'P')
		{
			accessTimer(1, timestr);
			sprintf(output_buffer, "Time: %9s, OS: Process: %d, run operation start\n", timestr, process->PID);
			handle_output(config_data, output_buffer);
		
			time_to_run = config_data->qTime;
			
			pthread_create(&thread, NULL, prunner, (void*)time_to_run);
			pthread_join(thread, NULL);
			
			tasks->cycle_time -= config_data->qTime;
			if(tasks->cycle_time <= 0)
			{
				tasks->finished = 1;
				accessTimer(1, timestr);
				sprintf(output_buffer, "Time: %9s, OS: Process: %d, run operation end\n", timestr, process->PID);
				handle_output(config_data, output_buffer);
			}
		}	
	}
	return 0;
}