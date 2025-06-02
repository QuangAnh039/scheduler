#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdio.h>
#include "GlobalData.h"
void parse_input_csv(const char *filename);
void parse_backup_csv(const char *filename);
void save_backup_csv(const char *filename);

#endif