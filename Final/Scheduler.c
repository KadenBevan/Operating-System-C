#include "Scheduler.h"
#include "Driver.h"

// Function: ready_all is called from the Driver.c
// pretty much used to ready all the processes 
int ready_all(Process *process_list)
{
	// declare an iterator to loop through the
	// process list arg with out modifying it.
	pthread_mutex_lock(&PCB_MUTEX);
	Process *process_itr = process_list;
	while(process_itr->next != NULL)
	{
		// ready the process
		process_itr->state = READY;
		process_itr = process_itr->next;
	}
	pthread_mutex_unlock(&PCB_MUTEX);
	return 0;
}

int sub_cycle(MetaData *current_operation, int cycle_time)
{
	pthread_mutex_lock(&PCB_MUTEX);
	MetaData *operation = NULL;
	operation = current_operation;
	if(operation->cycle_time > 0)
	{
		operation->cycle_time -= cycle_time;
	}
	pthread_mutex_unlock(&PCB_MUTEX);
	return 0;
}

int run_process(Process *process, Config *config_data)
{
	pthread_mutex_lock(&PCB_MUTEX);
	char output_buffer[MAX_STRING], timestr[SMALL_STRING];
	
	Process *process_p = NULL;
	process_p = process;
	
	if (process_p != NULL)
	{
		process_p->state = RUNNING;
		accessTimer(1, timestr);
		sprintf(output_buffer, "Time: %9s, OS: Process %i set in RUNNING state\n", 
				timestr, process->PID);
		handle_output(config_data, output_buffer);
	}
	pthread_mutex_unlock(&PCB_MUTEX);
	return 0;
}

int ready_process(Process *process, Config *config_data)
{
	pthread_mutex_lock(&PCB_MUTEX);
	char output_buffer[MAX_STRING], timestr[SMALL_STRING];
	
	Process *process_p = NULL;
	process_p = process;
	
	if (process_p != NULL)
	{
		process_p->state = READY;
		accessTimer(1, timestr);
		sprintf(output_buffer, "Time: %9s, OS: Process %i set in READY state\n", 
				timestr, process->PID);
		handle_output(config_data, output_buffer);
	}
	pthread_mutex_unlock(&PCB_MUTEX);
	return 0;
}

int block_process(Process *process, Config *config_data)
{
	pthread_mutex_lock(&PCB_MUTEX);
	char output_buffer[MAX_STRING], timestr[SMALL_STRING];
	
	Process *process_p = NULL;
	process_p = process;
	if (process_p != NULL)
	{
		process_p->state = BLOCKED;
		accessTimer(1, timestr);
		sprintf(output_buffer, "Time: %9s, OS: Process %i set in BLOCKED state\n", 
				timestr, process->PID);
		handle_output(config_data, output_buffer);
	}
	pthread_mutex_unlock(&PCB_MUTEX);
	return 0;
}

int exit_process(Process *process, Config *config_data)
{
	pthread_mutex_lock(&PCB_MUTEX);
	char output_buffer[MAX_STRING], timestr[SMALL_STRING];
	
	Process *process_p = NULL;
	process_p = process;
	if (process_p != NULL)
	{
		process_p->state = EXIT;
		accessTimer(1, timestr);
		sprintf(output_buffer, "Time: %9s, OS: Process %i set in EXIT state\n", 
				timestr, process->PID);
		handle_output(config_data, output_buffer);
	}
	pthread_mutex_unlock(&PCB_MUTEX);
	return 0;
}

// Function: update_pcb_time acutally used to update the total cycle_time
// all the PCBs. this is where we need to know if the task is finished or not.
int update_pcb_time(Process *process_list, Config *config)
{
	pthread_mutex_lock(&PCB_MUTEX);
	int remaining_cycle;
	Process *pprocess = process_list;
	while(pprocess->next != NULL)
	{
		// get the total_cycle_time and update the PCB
		remaining_cycle = total_cycle_time(pprocess, config->IOCT, config->PCT);
		pprocess->total_cycle = remaining_cycle;
		pprocess = pprocess->next;
		remaining_cycle = 0;
	}
	pthread_mutex_unlock(&PCB_MUTEX);
	// return that new cycle time.
	return remaining_cycle;
}

// Function: total_cycle_time will go through every metadata in a PCB and add up
// all the cycle times based on the config PCT and IOCT.
int total_cycle_time(Process *process, int IOCT, int PCT)
{
	int total = 0;
	MetaData *pOperations;
	pOperations = process->operation_list;
	
	// While we have not reached the end of a process string
	// We want to process metadata from A(start) to A(end), nothing more.
	while(pOperations->next != NULL && strncmp(pOperations->operation, "end", 2) != 0)
	{
		// check if the command is finished running or not.
		if(pOperations->finished == -1)
		{
			// If its an IO operation
			if(pOperations->command == 'I' || pOperations->command == 'O')
			{
				total += pOperations->cycle_time*IOCT;
			}
			// otherwise it will have PCT
			else if(pOperations->command == 'P')
			{
				total += pOperations->cycle_time*PCT;
			}
			pOperations = pOperations->next;
		}
		// if the metadata is done running go to the next metadata
		// we dont want to count it in our total.
		else 
		{
			pOperations = pOperations->next;
		}
	}
	return total;
}

Process *get_next_process(Process *process_list, Config* config_data)
{
	if(strncmp(config_data->scheduleCode, "FCFS-N", 5) == 0 
	|| strncmp(config_data->scheduleCode, "NONE", 5) == 0)
	{
		Process *process_itr = process_list;
		while(process_itr->next != NULL)
		{
			if(process_itr->state == READY)
			{
				return process_itr;
			}
			process_itr = process_itr->next;
		}
		return NULL;
	}
	else if(strncmp(config_data->scheduleCode, "SJF-N", 5) == 0)
	{
		int max_int = 2147483647;
		int shortest_pid = -1;
		int shortest_job = max_int;
		
		Process *process_itr = process_list;
		while(process_itr->next != NULL)
		{
			if(process_itr->total_cycle >= max_int)
			{
				char output_buffer[MAX_STRING], timestr[SMALL_STRING];
				accessTimer(1, timestr);
				sprintf(output_buffer, "Time: %9s, OS: Process %d is too long and will not be run.\n", timestr, process_itr->PID);
				handle_output(config_data, output_buffer);
				return NULL;
			}
			else if(process_itr->state == READY 
			&& process_itr->total_cycle < shortest_job)
			{
				shortest_job = process_itr->total_cycle;
				shortest_pid = process_itr->PID;
			}
			process_itr = process_itr->next;
		}
		process_itr = process_list;
		while(process_itr->next != NULL)
		{
			if(process_itr->PID == shortest_pid)
			{
				return process_itr;
			}
			process_itr = process_itr->next;
		}
		return NULL;
	}
	else if(strncmp(config_data->scheduleCode, "SRTF-P", 5) == 0)
	{
		// get the new times of the processes
		int max_int = 2147483647;
		int shortest_pid = -1;
		int shortest_job = max_int;
		
		Process *process_itr = process_list;
		while(process_itr->next != NULL)
		{
			if(process_itr->total_cycle >= max_int)
			{
				char output_buffer[MAX_STRING], timestr[SMALL_STRING];
				accessTimer(1, timestr);
				sprintf(output_buffer, "Time: %9s, OS: Process %d is too long and will not be run.\n", timestr, process_itr->PID);
				handle_output(config_data, output_buffer);
				return NULL;
			}
			else if(process_itr->state == READY 
			&& process_itr->total_cycle < shortest_job)
			{
				shortest_job = process_itr->total_cycle;
				shortest_pid = process_itr->PID;
			}
			process_itr = process_itr->next;
		}
		process_itr = process_list;
		while(process_itr->next != NULL)
		{
			if(process_itr->PID == shortest_pid)
			{
				return process_itr;
			}
			process_itr = process_itr->next;
		}
		return NULL;
	}
	else if(strncmp(config_data->scheduleCode, "FCFS-P", 5) == 0)
	{
		Process *process_itr = process_list;
		while(process_itr->next != NULL)
		{
			if(process_itr->state == READY && process_itr->total_cycle >= 1)
			{
				return process_itr;
			}
			process_itr = process_itr->next;
		}
		return NULL;
	}
	return 0;
}