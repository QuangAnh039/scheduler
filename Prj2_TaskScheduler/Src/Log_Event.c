#include "Inc/Log_Event.h"

void log_event(const char *msg) 
{
    FILE *f = fopen("tasks.log", "a");
    if (f) 
    {
        fprintf(f, "%s\n", msg);
        fclose(f);
    }
}