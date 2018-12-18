#ifndef CPU_H
#define CPU_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "Structures.h"
#include "Utility.h"
#include "Config_Parser.h"
#include "Scheduler.h"
#include "Output_Handler.h"
#include "MMU.h"
#include "IOThread.h"

int run_process(Process *process, Config *config_data);
int run_process_p(Process *process, Process *process_list, Config *config_data);
int start(Process *process, Config *config_data);
Process* interrupt(Process *interrupter, int mode);

#endif // !CPU_H
