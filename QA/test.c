#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define MAX_TASKS 10

typedef struct {
    char name[10];
    int arrival_time;
    int burst_time;
    int remaining_time;
    int priority;
    bool finished;
} Process;

Process processes[MAX_TASKS];
int n;

void input_processes() {
    printf("Nhập số lượng tiến trình: ");
    scanf("%d", &n);

    for (int i = 0; i < n; ++i) {
        printf("Tiến trình %d:\n", i + 1);
        printf("  Tên: ");
        scanf("%s", processes[i].name);
        printf("  Arrival time: ");
        scanf("%d", &processes[i].arrival_time);
        printf("  Burst time: ");
        scanf("%d", &processes[i].burst_time);
        printf("  Priority (cao hơn = ưu tiên hơn): ");
        scanf("%d", &processes[i].priority);

        processes[i].remaining_time = processes[i].burst_time;
        processes[i].finished = false;
    }
}

int find_highest_priority_ready(int current_time) {
    int idx = -1;
    for (int i = 0; i < n; ++i) {
        if (!processes[i].finished && processes[i].arrival_time <= current_time) {
            if (idx == -1 || processes[i].priority > processes[idx].priority) {
                idx = i;
            }
        }
    }
    return idx;
}

int main() {
    input_processes();

    int completed = 0;
    int current_time = 0;
    int current_process = -1;

    printf("\n--- Bắt đầu mô phỏng ---\n");

    while (completed < n) {
        int next_process = find_highest_priority_ready(current_time);

        if (next_process != -1) {
            // Nếu có process đang chạy và process mới ưu tiên hơn, preempt
            if (current_process != -1 && processes[next_process].priority > processes[current_process].priority) {
                printf("[Time %d] Preempt: %s -> %s\n",
                       current_time, processes[current_process].name, processes[next_process].name);
                current_process = next_process;
            }

            // Nếu không có process đang chạy
            if (current_process == -1 || processes[current_process].finished) {
                current_process = next_process;
                printf("[Time %d] Bắt đầu: %s\n", current_time, processes[current_process].name);
            }

            // Thực thi 1 đơn vị thời gian
            processes[current_process].remaining_time -= 1;
            printf("[Time %d] %s đang chạy (còn %d)\n",
                   current_time, processes[current_process].name, processes[current_process].remaining_time);

            if (processes[current_process].remaining_time == 0) {
                processes[current_process].finished = true;
                completed++;
                printf("[Time %d] Hoàn thành: %s\n", current_time + 1, processes[current_process].name);
                current_process = -1;
            }
        } else {
            // Không có process sẵn sàng ⇒ idle
            printf("[Time %d] Idle...\n", current_time);
        }

        current_time++;
    }

    printf("\n--- Mô phỏng hoàn tất ---\n");
    return 0;
}
