#ifndef IOThread_H
#define IOThread_H

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

void* prunner_interrupt(void* time);

#endif // !IOThread_H