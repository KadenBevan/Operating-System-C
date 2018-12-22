#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <stdlib.h>
#include <stdio.h>
#include "Structures.h"
#include "Utility.h"
#include "Timer.h"
#include "Threads.h"
#include "Driver.h"
#include "Output_Handler.h"

int update_pcb_time(Process *processes, Config *config);
int sub_cycle(MetaData *current_operation, int cycle_time);
int total_cycle_time(Process *process, int IOCT, int PCT);
Process *get_next_process(Process *process_list, Config* config_data);
int ready_all(Process *process_list);
int ready_process(Process *process, Config *config_data);
int block_process(Process *process, Config *config_data);
int exit_process(Process *process, Config *config_data);


#endif