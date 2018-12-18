#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdlib.h>
#include <stdio.h>
#include "Structures.h"
#include "Utility.h"
#include "Timer.h"

int schedule_processes(Process *processes, Config *config);
int total_cycle_time(Process *process, int IOCT, int PCT);
Process *get_next_process(Process *process_list, Config* config_data);
int ready_all(Process *process_list);
int ready_process(Process* process_list, int PID, Config *config_data);


#endif