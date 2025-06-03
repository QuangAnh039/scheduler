#include "task_oper.h"

void start_task(Node *n)
{
    Task *t = &n->task;
    pid_t pid = fork();
    if (pid == 0) 
    { // Child
        char burst_str[4], remain_str[4];
        sprintf(burst_str, "%d", t->burst_time);
        sprintf(remain_str, "%d", t->remaining_time);
        execl("./task", "./task", t->name, burst_str, remain_str, (char *)NULL);
    } 
    else 
    { // Parent
        t->pid     = pid;
        t->started = 1;
    }
}
Node* find_highest_priority_ready(int current_time) {
    Node *best = NULL;
    Node *cur  = head;

    while (cur) {
        Task *t = &cur->task;
        if (!t->finished && t->arrival_time <= current_time) {
            if (best == NULL) {
                best = cur;
            } else {
                Task *b = &best->task;
                // Chọn priority lớn hơn
                if (t->priority > b->priority) {
                    best = cur;
                }
                // Nếu priority bằng, tie-break: remaining_time nhỏ hơn
                else if (t->priority == b->priority &&
                         t->remaining_time < b->remaining_time) {
                    best = cur;
                }
            }
        }
        cur = cur->next;
    }
    return best;
}
Node* dispatch_preempt(Node *current_node, Node *next_node, int current_time) 
{
    if (next_node == NULL) 
    {
        return current_node;
    }
    if (current_node == NULL) 
    {
        if (!next_node->task.started) 
        {
            start_task(next_node); 
        } else 
        {
            kill(next_node->task.pid, SIGCONT);
        }
        return next_node;
    }
    Task *c = &current_node->task;
    Task *n = &next_node->task;
    bool higher = (n->priority > c->priority);
    bool tie_smaller = (n->priority == c->priority && n->remaining_time < c->remaining_time);
    if (higher || tie_smaller)
    {
        kill(c->pid, SIGSTOP);
        if (!n->started) {
            start_task(next_node);
        } else {
            kill(n->pid, SIGCONT);
        }
        return next_node;
    }
    return current_node;
}
Node* check_completion(Node *current_node, int current_time) 
{
    if (current_node == NULL) return NULL;

    Task *t = &current_node->task;
    t->remaining_time--;

    if (t->remaining_time <= 0) {
       
        kill(t->pid, SIGKILL);
        int status;
        waitpid(t->pid, &status, 0);
        t->finished = true;
        printf("[Scheduler] Task %s (PID=%d) finished at time %d\n", 
               t->name, t->pid, current_time);
        return NULL; 
    }
    return current_node;
}

int count_nodes(Node *h) 
{
    int count = 0;
    Node *temp = h;
    while (temp != NULL) {
        count++;
        temp = temp->next;
    }
    return count;
}
void sort_task_list_by_priority() {
    if (head == NULL || head->next == NULL) {
        return;
    }

    Node *i, *j;
    Task temp_task;

    for (i = head; i->next != NULL; i = i->next) {
        for (j = i->next; j != NULL; j = j->next) {
            if (j->task.priority > i->task.priority) {
                temp_task = i->task;
                i->task = j->task;
                j->task = temp_task;
            }
        }
    }
}

void cleanUpTasks() 
{
    Node *current = head;
    Node *prev = NULL;

    while (current != NULL) {
        if (current->task.finished) 
        {
            if (prev == NULL) 
            {
                head = current->next;
                free(current);
                current = head;
            } 
            else 
            {
                prev->next = current->next;
                free(current);
                current = prev->next;
            }
        } 
        else 
        {
            current->task.started = false;
            prev = current;
            current = current->next;
        }
    }
}