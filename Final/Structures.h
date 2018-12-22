#ifndef STRUCTURES_H
#define STRUCTURES_H

#include <pthread.h>

//Constants
#define MAX_STRING 256
#define MEDIUM_STRING 128
#define SMALL_STRING 64

// Struct to hold Config attributes
typedef struct Config
{
	int version;
	int q_time;
	int available_memory;
	int PCT;
	int IOCT;
	char log_path[MAX_STRING];
	char log_destination[MAX_STRING];
	char schedule_code[MAX_STRING];
	char metadata_path[MAX_STRING];

} Config;

// Holds process information. Pretty much a PCB
// also used as a linked list
typedef struct Process
{
	pthread_mutex_t lock;
	
	int PID;
	enum state {NEW, READY, BLOCKED, RUNNING, EXIT} state;
	int total_cycle;

	struct MetaData *operation_list;
	struct MetaData *current_operation;
	struct Process  *next;
} Process;

// holds task information. Also a linked list
// but this list is attached to a PCB or process.
typedef struct MetaData
{
	int cycle_time;
	char command;
	char* operation;
	int finished;

	struct MetaData *next;
	struct MetaData *tail;
} MetaData;

typedef struct IOThreadData
{
	int run_time;
	struct Process *process;
} IOThreadData;

typedef struct WatchThreadData
{
	struct Config *config;
} WatchThreadData;

#endif