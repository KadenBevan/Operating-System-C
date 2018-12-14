#ifndef CONFIG_PARSER
#define CONFIG_PARSER

// ERRORS
#define OPEN_FILE_ERROR -1
#define ATTRIBUTE_EXTRACTION_ERROR -2
#define INVALID_FIELD -3;

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Structures.h"
#include "Utility.h"

int parse_config(char *filename, Config *config_data);
int is_config(Config *config_data);

#endif