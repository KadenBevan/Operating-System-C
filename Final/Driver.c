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
void* prunner_interrupt(void* time)
{
	// Cast the time.
	// This causes warnings when compiling
	// I dont know why.
	int run_time = (int)time;
	
	// Use Mr. Professors timer
	runTimer(run_time);
	
	// Done running the timer?
	// Flag th interrupt handler in the CPU.c
	interrupt();
	
	// return null because threads
	return NULL;
}

int main(int argc, char *argv[])
{
	// var to hold time
	char timestr[SMALL_STRING], output_buffer[MAX_STRING];
	accessTimer(0, timestr);
	
	// basic input error checking
	if (argc != 2)
	{
		printf("Input error!\nFormat: ./program.exe config_file.cnf");
		return -1;
	}
	
	// Declare a pointer to hold the config path string.
	// assume that this file is int the some dir as the 
	char *config_path;
	
	// assign it the above pointer
	config_path = argv[1];
	
	// Initialize a variable to hold the Config attributes
	Config config_data = {-1, -1, -1, -1, -1, "", "", "", ""};
	
	// check for errors parsing the config data
	if(parse_config(config_path, &config_data) != 0)
	{
		printf("Config Data Extraction Error.\n");
		return -1;
	}
	
	// send some output to the handler
	sprintf(output_buffer, "Simulator Program\n=================\n\n");
	handle_output(&config_data, output_buffer);
	
	// send some output to the handler
	sprintf(output_buffer, "Uploading Configuration Files\n\nUploading Meta Data Files\n\n");
	handle_output(&config_data, output_buffer);
	
	// send some output to the handler
	sprintf(output_buffer, "=================\nBegin Simulation\n\n");
	handle_output(&config_data, output_buffer);
	
	// send some output to the handler
	accessTimer(1, timestr);
	sprintf(output_buffer, "Time: %9s, System Start\n", timestr);
	handle_output(&config_data, output_buffer);
	
	// send some output to the handler
	accessTimer(1, timestr);
	sprintf(output_buffer, "Time: %9s, OS: Begin PCB Creation\n", timestr);
	handle_output(&config_data, output_buffer);
	
	// A pointer to hold the path of the metadata file
	// assume the file is in the same dir as the config.
	char *meta_path;
	
	// assign the path to the pointer.
	meta_path = config_data.metaFilePath;	
	
	// malloc some room for the metadata linked list
	MetaData *meta_head = malloc(sizeof(MetaData));
	// Initialize some of the attributes
	meta_head->next = NULL;
	meta_head->cycle_time = -1;
	
	// check for errors parsing the metadata
	if(parse_meta_data(meta_head, meta_path) != 0)
	{
		printf("MetaData Extraction Error.\n");
		return -1;
	}
	
	// initiate the MMU
	init_mmu(&config_data);
	
	// these three lines are seen often. It is how I add data to a string to be printed out
	// based on where the config file says it should go
	accessTimer(1, timestr);
	sprintf(output_buffer, "Time: %9s, OS: All processes now set in New state\n", timestr);
	handle_output(&config_data, output_buffer);
	
	// add metadata linked list to a single process
	Process *process;
	process = add_meta_to_process(meta_head);
	
	// find the total cycle time for the entire process
	int total_cycle_time;
	total_cycle_time = schedule_processes(process, &config_data);
	
	// Set all the processes to the ready state
	ready_all(process);
	
	// and output that we did so
	accessTimer(1, timestr);
	sprintf(output_buffer, "Time: %9s, OS: All processes now set in Ready state\n", timestr);
	handle_output(&config_data, output_buffer);
	
	// CPU.c:Start() running the processes
	start(process, &config_data);
	
	// output system info
	accessTimer(1, timestr);
	sprintf(output_buffer, "Time: %9s, OS: System stop\n", timestr);
	handle_output(&config_data, output_buffer);
	
	// output system info
	sprintf(output_buffer, "\nEnd Simulation - Complete\n=================\n\n");
	handle_output(&config_data, output_buffer);
	
	// free the process list
	free(process);
}

