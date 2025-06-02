#ifndef _PARSER_H_
#define _PARSER_H_

#include <stdio.h>
#include "GlobalData.h"
int load_state_from_csv(const char *filename);
void save_state_to_csv(const char *filename);

#endif