#ifndef DRIVER_H
#define DRIVER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "Structures.h"
#include "Utility.h"
#include "Config_Parser.h"
#include "Process_Parser.h"
#include "Output_Handler.h"
#include "Scheduler.h"
#include "Timer.h"
#include "CPU.h"
#include "Threads.h"

extern pthread_mutex_t PCB_MUTEX;
extern pthread_mutex_t INTERRUPT_MUTEX;

#endif // !DRIVER_H
