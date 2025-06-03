#include "my_menu.h"
void show_menu() {
    printf("\n===== Scheduler Menu =====\n");
    printf("1. Add Task\n");
    printf("2. Modify Task\n");
    printf("3. Remove Task\n");
    printf("4. Print Task List\n");
    printf("5. Save to Backup\n");
    printf("6. Exit Menu\n");
    printf("==========================\n");
}

void add_task() {
    Task new_task;
    printf("Enter name: ");
    scanf("%s", new_task.name);
    printf("Enter command: ");
    scanf("%s", new_task.command);
    printf("Enter arrival time: ");
    scanf("%d", &new_task.arrival_time);
    printf("Enter priority: ");
    scanf("%d", &new_task.priority);
    printf("Enter burst time: ");
    scanf("%d", &new_task.burst_time);

    new_task.remaining_time = new_task.burst_time;
    new_task.started = 0;
    new_task.finished = 0;

    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->task = new_task;
    new_node->next = NULL;

    if (head == NULL) head = new_node;
    else {
        Node *temp = head;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = new_node;
    }
}
void modify_task() {
    char name[10];
    printf("Enter task name to modify: ");
    scanf("%s", name);
    Node *temp = head;
    while (temp) {
        if (strcmp(temp->task.name, name) == 0) {
            printf("Enter new command: ");
            scanf("%s", temp->task.command);
            printf("Enter new arrival time: ");
            scanf("%d", &temp->task.arrival_time);
            printf("Enter new priority: ");
            scanf("%d", &temp->task.priority);
            printf("Enter new burst time: ");
            scanf("%d", &temp->task.burst_time);
            temp->task.remaining_time = temp->task.burst_time;
            printf("Task modified.\n");
            return;
        }
        temp = temp->next;
    }
    printf("Task not found.\n");
}
void remove_task() {
    char name[10];
    printf("Enter task name to remove: ");
    scanf("%s", name);
    Node *temp = head, *prev = NULL;
    while (temp) {
        if (strcmp(temp->task.name, name) == 0) 
        {
            if (prev == NULL) head = temp->next;
            else prev->next = temp->next;
            free(temp);
            return;
        }
        prev = temp;
        temp = temp->next;
    }
    printf("Task not found.\n");
}