#include "Process_Parser.h"


// Function: add_meta_to_process this function, or whole
// file rather, gets kinda confusing so try and keep up.
// This function is for adding a metadata string to a process
// declared in Driver.c.
// The metadata string is created using parse_meta_data() with the help
// of add_new_node()
Process *add_meta_to_process(MetaData *metadata)
{
	// keep track of the current PID
	int pid = 0;
	
	// create room for a new Process
	Process *new_process = malloc(sizeof(Process));
	// Initialize some attributes
	new_process->next = NULL;
	
	// declare a pointer to the first process in the list
	// this is what we will return.
	Process *pret_process = new_process;
	
	// create a pointer to keep track of the metadata arg
	// without modifiying the actual arg
	MetaData *pmeta_head = metadata;
	
	// while the current metadata is not a system ending string
	while(!(metadata->command == 'S' && strncmp(metadata->operation, "end", 2) == 0))
	{
		// while the current metadata is not a process ending string
		while(!(metadata->command == 'A' && strncmp(metadata->operation, "end", 2) == 0))
		{
			// find the end of the process
			metadata = metadata->next;
		}
		MetaData *temp = metadata->next;
		metadata->next = NULL;
		metadata = temp;
		
		new_process->PID = pid;
		pid++;
		new_process->operation_list = pmeta_head;
		new_process->next = malloc(sizeof(Process));
		new_process = new_process->next;
		new_process->next = NULL;
		
		pmeta_head = temp;
		//MetaData *temp = metadata;
		//metadata->next = NULL;
		//// give the new process the data
		//
		//
		//// incriment the PID
		//pid++;
		//
		//// move the metadata pointer to the next process segment
		//pmeta_head = temp;
		//
		//// keep track of the metadata
		//metadata = metadata->next;
	}
	// return the freshly created process linked list
	return pret_process;
}

// Function: parse_meta_data is used to open the metadata file
// declared in the config and send metadata segments to add_new_node.
// a meta_head segment has this formant O(hardrive)5;
int parse_meta_data(MetaData *meta_head, char* filename)
{
	// declare some arrays to hold metadata information
	char buffer[32];
	char line[256];
	char currentChar;
	int buffcount = 0;
    
    // Return if could not open file 
	FILE *fp;
	if ((fp = fopen(filename, "r")) == NULL)
	{
		// ERROR checking
		return OPEN_FILE_ERROR;
	}
	
	// iterate through each line in the file
	while(fgets(line, sizeof(line), fp) != NULL)
	{
		// if its the beginning line or the end one skip it.
		if(strncmp(line, "Start Program Meta-Data Code:", 28) == 0
			|| strncmp(line, "End Program Meta-Data Code.", 25) == 0)
		{
			continue;
		}
		// for character in the current line
		for (int i = 0; line[i] != 0; i++)
		{
			// assign the current character
			currentChar = line[i];
			
			// metadata segments are never over 31 chars
			// if they are? error
			if (buffcount > 31) 
			{
				break;
			}
			// we reached the end of a segment.
			else if(currentChar == ';')
			{	
				// sprintf is stdio.h so dont mark me down
				sprintf(&buffer[buffcount], "%c", currentChar);
				
				// call the precious add_new_node() to fill the data into
				// an actuall metadata list
				add_new_node(buffer, meta_head);
				
				// reset the buffer count.
				buffcount = 0;
				
				// clear the buffer
				sprintf(&buffer[buffcount], "%c", ' ');
			}
			else
			{
				sprintf(&buffer[buffcount], "%c", currentChar);
			}
			buffcount++;
		}
	}
    // close the file and get out.
    fclose(fp);
    return 0;
}

// Function: add_new_node I am proud of this function. This takes a meta segment
// parses it using my precious ragex formater and adds the data to a list.
int add_new_node( char *string, MetaData *meta_head )
{
	// data to go into a metadata
	char opCode;
	char *type = malloc(sizeof(char)*32);
	int cycle;

	// precious regex formatter
	if(sscanf(string, " %c%*[(]%[a-z]%*[)]%d", &opCode, type, &cycle) == 3)
	{
		// an iterator
		MetaData *iterator = meta_head;
		
		// a node to be added to the list that is initialized
		MetaData *new_node = malloc(sizeof(MetaData));
		new_node->cycle_time = cycle;
		new_node->operation = type;
		new_node->command = opCode;
		new_node->finished = -1;
		new_node->next = NULL;
		
		if(iterator->tail == NULL)
		{
			iterator->cycle_time = cycle;
			iterator->operation = type;
			iterator->command = opCode;
			iterator->finished = -1;
			iterator->next = NULL;
			iterator->tail = iterator;
		}
		else
		{
			MetaData *pTail = meta_head->tail;
			pTail->next = new_node;
			meta_head->tail = new_node;
		}
	}
	// get out
	return 0;
}