#ifndef Threads_H
#define Threads_H

#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include "Structures.h"
#include "Utility.h"
#include "Config_Parser.h"
#include "Scheduler.h"
#include "Output_Handler.h"
#include "MMU.h"
#include "CPU.h"

void* prunner(void* time);

void* io_thread(void* time);

void* interrupt_watcher(void* data_in);

int init_interrupt_watcher(Config *config_data);

#endif // !Threads_H