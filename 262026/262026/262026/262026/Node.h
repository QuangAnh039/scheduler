#ifndef _NODE_H_
#define _NODE_H_
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#define MAX_CMD_LEN 10

typedef struct {
    char name[20];
    char command[MAX_CMD_LEN];
    int arrival_time;
    int priority;
    int burst_time;
    int remaining_time;
    pid_t pid;
    bool started;
    bool finished;
} Task;

typedef struct Node {
    Task          task;
    struct Node  *next;
} Node;
extern Node *head;
extern int current_time;

#endif