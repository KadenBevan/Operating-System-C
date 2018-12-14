#ifndef PROCESS_PARSER
#define PROCESS_PARSER

// ERRORS
#define OPEN_FILE_ERROR -1
#define ATTRIBUTE_EXTRACTION_ERROR -2

#include <stdlib.h>
#include <stdio.h>
#include "Structures.h"
#include "Utility.h"

int meta_datas_to_list(char *filename, MetaData *meta_list);
Process *add_meta_to_process(MetaData *metadata);
int add_new_node( char *string, MetaData *meta_head );
int parse_meta_data(MetaData *meta_head, char* filename);

#endif 