#ifndef MMU_H
#define MMU_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Structures.h"
#include "Utility.h"
#include "Config_Parser.h"
#include "Scheduler.h"

int allocate_memory(int segment, int base, int allocate);
int access_memory(int segment, int base, int allocate);
int init_mmu(Config *config);

typedef struct Table
{
	int start;
	int end;
	int segment;
}Table;

#endif