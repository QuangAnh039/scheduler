#ifndef _DATA_H_
#define _DATA_H_
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <string.h>
#include <stdbool.h>

#define MAX_TASKS    10
#define MAX_CMD_LEN  100

typedef struct {
    char name[20];
    char command[MAX_CMD_LEN];
    int arrival_time;
    int burst_time;
    int remaining_time;
    int priority;
    pid_t pid;
    bool started;
    bool finished;
} Task;

Task tasks[MAX_TASKS];
int n = 0;               // số lượng task
int current_time = 0;    // giờ “ảo” của scheduler
int completed = 0;       // số task đã finish
int current_task = -1;   // idx task đang chạy, -1 nếu none

char *backup_filename = NULL;  // nếu chạy với argv[1] = "backup.csv"

#endif