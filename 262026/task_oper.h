#ifndef _TASK_OPER_H_
#define _TASK_OPER_H_
#include "Node.h"
#include <limits.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>
void start_task(Node *n);
Node* find_highest_priority_ready(int current_time);
Node* dispatch_preempt(Node *current_node, Node *next_node, int current_time);
Node* check_completion(Node *current_node, int current_time);
int count_nodes(Node *h);
void sort_task_list_by_priority();
void cleanUpTasks();
#endif