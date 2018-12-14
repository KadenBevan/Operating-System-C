#include "MMU.h"

// static variables to keep track of memory
static Table table[1000];
static int max_memory = 0;
static int counter = 0;

int allocate_memory(int segment, int base, int allocate)
{
	int start, end, t_index, total;
	start = base;
	end = base + allocate;
	total = 0;
	if (start < 0 || end < 0 || start > end || end > max_memory || start > max_memory)
	{
		return -1;
	}
	for (t_index = 0; t_index < counter; t_index++)
	{
		// TODO if end or start is greater than max
		total += table[t_index].end - table[t_index].start;
		if (table[counter].segment == segment)
		{
			if ((end >= table[t_index].start && end <= table[t_index].end) || (start >= table[t_index].start && start <= table[t_index].end))
			{
				return -1;
			}
		}
	}
	if (total > max_memory)
	{
		return -1;
	}
	table[counter].start = start;
	table[counter].end = end;
	counter++;
	return 0;
}

int access_memory(int segment, int base, int allocate)
{
	int start, end, t_index;
	start = base;
	end = base + allocate;
	if (start < 0 || end < 0 || start > end || end > max_memory || start > max_memory)
	{
		return -1;
	}
	for (t_index = 0; t_index < counter; t_index++)
	{
		if(table[counter].segment == segment)
		{
			if ((table[t_index].start < start && table[t_index].end < end) || (table[t_index].start > start || table[t_index].end < end))
			{
				return -1;
			}
		}
	}
	return 0;
}

int init_mmu(Config *config)
{
	max_memory = config->availableMem;
	return 0;
}