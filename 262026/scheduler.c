#include "file_oper.h"
#include "Node.h"
#include "my_menu.h"
#include <signal.h>
#include "task_oper.h"

Node *head = NULL;
int current_time = 0;

volatile sig_atomic_t menu_requested = 0;

void print_task_list()
{
    Node *temp = head;
    if (temp == NULL) {
        printf("Task list is empty.\n");
        return;
    }

    printf("Task List:\n");
    printf("-----------------------------------------------------------\n");
    while (temp != NULL) {
        Task t = temp->task;
        printf("Name: %s\n", t.name);
        printf("Command: %s\n", t.command);
        printf("Arrival Time: %d\n", t.arrival_time);
        printf("Priority: %d\n", t.priority);
        printf("Burst Time: %d\n", t.burst_time);
        printf("Remaining Time: %d\n", t.remaining_time);
        printf("Started: %d\n", t.started);
        printf("Finished: %d\n", t.finished);
        printf("-----------------------------------------------------------\n");
        temp = temp->next;
    }
}
void sigtstp_handler() 
{
    menu_requested = 1;  
}
void sigint_handler()
{
    save_to_backup("backup.csv");
    printf("Already backup\n");
    exit(0);
}
void scheduler_loop() 
{
    //current_time = 0;
    int total_tasks  = count_nodes(head);
    int completed    = 0;
    Node *current_node = NULL;
    printf("=== Scheduler starts (time %d) ===\n", current_time);
    while (completed < total_tasks) 
    {
        if (menu_requested)
        {
            menu_requested = 0;
            show_menu();        
            total_tasks = count_nodes(head);
            sort_task_list_by_priority();
        }
        Node *next_node = find_highest_priority_ready(current_time);
        current_node = dispatch_preempt(current_node, next_node, current_time);
        current_time++;
        sleep(1);
        current_node = check_completion(current_node, current_time);
        if (current_node == NULL) 
        {
            completed++;
        }
        printf("=== All tasks completed at time %d ===\n", current_time);
    }
}
int main()
{
    signal(SIGTSTP, sigtstp_handler);
    signal(SIGINT, sigint_handler);
    parse_input("backup.csv");

    print_task_list();
    printf("\n-------------------------------------\n");
    cleanUpTasks();
    print_task_list();
    // sort_task_list_by_priority();
    // printf("so luong task = %d\n", count_nodes(head));
    // print_task_list();
    // current_time = 3;
    // Node *node_test = find_highest_priority_ready(current_time); // hoat dong dung
    // printf("\n-------------------------------------\n");
    // printf("Node have highest priority is: %s\n", node_test->task.name);
    //save_to_backup("backup.csv");
    //scheduler_loop();
    while(1)
    {

    }


    return 0;
}