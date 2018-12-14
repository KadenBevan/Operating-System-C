#include "Utility.h"

// used to remove spaces of a string.
void remove_spaces(char* source)
{
	char* place = source;
	char* origin = source;
	while (*origin != 0)
	{
		*place = *origin++;
		if (*place != ' ')
			place++;
	}
	*place = '\0';
}

// Decides if arg is a valid command
int is_command(char command)
{
	if (command == 'S' || command == 'A' || command == 'P'
		|| command == 'M' || command == 'I' || command == 'O')
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

// Decides if arg is a valid operation
int is_operation(char* operation)
{
	if (str_n_cmp(operation, "start", 4)
		|| str_n_cmp(operation, "allocate", 7)
		|| str_n_cmp(operation, "harddrive", 8)
		|| str_n_cmp(operation, "run", 2)
		|| str_n_cmp(operation, "keyboard", 7)
		|| str_n_cmp(operation, "monitor", 6)
		|| str_n_cmp(operation, "access", 5)
		|| str_n_cmp(operation, "printer", 6)
		|| str_n_cmp(operation, "end", 2))
	{
		return 0;
	}
	else
	{
		return -1;
	}
}

// my own strncmp function because we have to.
int str_n_cmp(const char* string1, const char* string2, size_t size)
{
	while (size && *string1 && (*string1 == *string2))
	{
		++string1;
		++string2;
		--size;
	}
	if (size == 0)
	{
		return 0;
	}
	else
	{
		// cast to unsigned char because it is safer
		return (*(unsigned char *)string1 - *(unsigned char *)string2);
	}
}

// also a char to integer cool trick.
int a_to_i(char* str)
{
	int new_int = 0, index;

	for (index = 0; str[index] != '\0'; index++)
	{
		new_int = new_int * 10 + str[index] - '0';
	}
	return new_int;
}
