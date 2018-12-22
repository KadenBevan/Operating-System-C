#include "Output_Handler.h"

// these functions are not safe to be called outside of this file.
static void output_monitor(Config *config, char line[]);
static void output_file(Config *config, char line[]);

// Sends the output to the desired location.
// pretty basic
void handle_output(Config *config, char line[])
{
	if (str_n_cmp(config->log_destination, "Both", 3) == 0)
	{
		output_file(config, line);
		output_monitor(config, line);
	}
	else if (str_n_cmp(config->log_destination, "Monitor", 3) == 0)
	{
		output_monitor(config, line);
	}
	else if (str_n_cmp(config->log_destination, "File", 3) == 0)
	{
		output_file(config, line);
	}
	
	return;
}

void output_monitor(Config *config, char line[])
{
	printf("%s", line);
	return;
}

void output_file(Config *config, char line[])
{
	FILE* logfile;
	if ((logfile = fopen(config->log_path, "a")) == NULL)
	{
		return;
	}
	fprintf(logfile, "%s", line);
	fclose(logfile);
	
	return;
}
