#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) 
{
    char *task_name = argv[1];
    int burst_time = atoi(argv[2]);
    int remain_time = atoi(argv[3]);
    FILE *log = fopen("task.log", "a");
    int start = burst_time - remain_time + 1;
    for (int i = start; i <= burst_time; i++) 
    {
        fprintf(log, "[%s] Step %d/%d\n", task_name, i, burst_time);
        fflush(log);
        sleep(1);
    }
    //fprintf(log, "[%s] Task completed!\n", task_name);
    //fflush(log);
    fclose(log);
    while(1);
    return 0;
}