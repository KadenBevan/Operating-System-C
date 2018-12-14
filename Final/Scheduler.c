#include "Scheduler.h"

// Function: ready_all is called from the Driver.c
// pretty much used to ready all the processes 
int ready_all(Process *process_list)
{
	// declare an iterator to loop through the
	// process list arg with out modifying it.
	Process *process_itr = process_list;
	while(process_itr->next != NULL)
	{
		// ready the process
		process_itr->state = READY;
		process_itr = process_itr->next;
	}
	return 0;
}

// Function: schedule_processes acutally used to update the total cycle_time
// all the PCBs. this is where we need to know if the task is finished or not.
int schedule_processes(Process *processes, Config *config)
{
	int remaining_cycle;
	Process *pprocess = processes;
	while(pprocess->next != NULL)
	{
		// get the total_cycle_time and update the PCb
		remaining_cycle = total_cycle_time(pprocess, config->IOCT, config->PCT);
		pprocess->total_cycle = remaining_cycle;
		pprocess = pprocess->next;
		remaining_cycle = 0;
	}
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
		int shortest_pid = -1;
		int shortest_job = 999999;
		Process *process_itr = process_list;
		while(process_itr->next != NULL)
		{
			if(process_itr->state == READY 
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
		schedule_processes(process_list, config_data);
	}
	else if(strncmp(config_data->scheduleCode, "FCFS-P", 5) == 0)
	{
		
	}
	else if(strncmp(config_data->scheduleCode, "RR-P", 2) == 0)
	{
		
	}
	return 0;
}