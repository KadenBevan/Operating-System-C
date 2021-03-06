#include "Config_Parser.h"

int parse_config(char *filename, Config *config_data)
{
	// open config file for reading and check success
	FILE* ptr_file;
	if ((ptr_file = fopen(filename, "r")) == NULL)
	{
		return OPEN_FILE_ERROR;
	}
	// set up line for fgets function
	char line[MAX_STRING];
	// for every line in the config file
	while (fgets(line, sizeof(line), ptr_file) != NULL)
	{
		// try every possible attribute on each config line
		if (strncmp(line, "Version/Phase:", 10) == 0)
		{
			sscanf(line, "Version/Phase: %d", &config_data->version);
		}
		if (strncmp(line, "File Path:", 7) == 0)
		{
			char temp_file[MEDIUM_STRING];
			sscanf(line, "File Path: %s", temp_file);
			strcpy(config_data->metadata_path, temp_file);
		}
		if (strncmp(line, "CPU Scheduling Code:", 15) == 0)
		{
			char temp_code[MEDIUM_STRING];
			sscanf(line, "CPU Scheduling Code: %s", temp_code);
			strcpy(config_data->schedule_code, temp_code);
		}
		if (strncmp(line, "Quantum Time (cycles):", 18) == 0)
		{
			sscanf(line, "Quantum Time (cycles): %d", &config_data->q_time);
		}
		if (strncmp(line, "Memory Available (KB):", 18) == 0)
		{
			sscanf(line, "Memory Available (KB): %d", &config_data->available_memory);
		}
		if (strncmp(line, "Processor Cycle Time (msec):", 20) == 0)
		{
			sscanf(line, "Processor Cycle Time (msec): %d", &config_data->PCT);
		}
		if (strncmp(line, "I/O Cycle Time (msec):", 15) == 0)
		{
			sscanf(line, "I/O Cycle Time (msec): %d", &config_data->IOCT);
		}
		if (strncmp(line, "Log To:", 5) == 0)
		{
			char temp_log_destination[MEDIUM_STRING];
			sscanf(line, "Log To: %s", temp_log_destination);
			strcpy(config_data->log_destination, temp_log_destination);
		}
		if (strncmp(line, "Log File Path:", 10) == 0)
		{
			char temp_logpath[MEDIUM_STRING];
			sscanf(line, "Log File Path: %s", temp_logpath);
			strcpy(config_data->log_path, temp_logpath);
		}
	}
	fclose(ptr_file);
	return is_config(config_data);
}

int is_config(Config *config_data)
{
	int error = 0;
	if (config_data->version > 10 || config_data->version < 0)
	{
		error++;
	}
	else if (config_data->q_time < 0 || config_data->q_time > 100)
	{
		error++;
	}
	else if (config_data->available_memory < 0 || config_data->available_memory > 1048576)
	{
		error++;
	}
	else if (config_data->PCT < 0 || config_data->PCT > 1000)
	{
		error++;
	}
	else if (config_data->IOCT < 0 || config_data->IOCT > 10000)
	{
		error++;
	}
	else if (config_data->metadata_path == NULL)
	{
		error++;
	}
	else if (config_data->log_path == NULL)
	{
		error++;
	}
	else if (config_data->log_destination == NULL)
	{
		error++;
	}
	else if (config_data->schedule_code == NULL)
	{
		error++;
	}
	return error;
}