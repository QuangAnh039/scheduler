    #include <stdio.h>
    #include <stdlib.h>
    #include <unistd.h>
    #include <signal.h>
    #include <sys/wait.h>
    #include <string.h>
    #include <stdbool.h>
    #include <time.h>

    #define MAX_TASKS 10
    #define MAX_CMD_LEN 100

    typedef struct {
        char name[20];
        //char command[MAX_CMD_LEN];
        int arrival_time;
        int priority;
        int burst_time;    // Thời gian chạy CPU dự kiến (đơn vị giây)
        int remaining_time; // Thời gian còn lại
        pid_t pid;
        bool started;
        bool finished;
    } Task;

    Task tasks[MAX_TASKS];
    int n;

    void log_event(const char *msg) {
        FILE *f = fopen("tasks.log", "a");
        if (f) {
            fprintf(f, "%s\n", msg);
            fclose(f);
        }
    }

    void input_tasks() {
        printf("Nhập số lượng task: ");
        scanf("%d", &n);
        getchar(); // Clear buffer

        for (int i = 0; i < n; ++i) {
            printf("Task %d:\n", i + 1);
            printf("  Tên: ");
            fgets(tasks[i].name, sizeof(tasks[i].name), stdin);
            tasks[i].name[strcspn(tasks[i].name, "\n")] = 0;

            printf("  Command: ");
            fgets(tasks[i].command, sizeof(tasks[i].command), stdin);
            tasks[i].command[strcspn(tasks[i].command, "\n")] = 0;

            printf("  Arrival time: ");
            scanf("%d", &tasks[i].arrival_time);

            printf("  Burst time (giây): ");
            scanf("%d", &tasks[i].burst_time);
            tasks[i].remaining_time = tasks[i].burst_time;

            printf("  Priority: ");
            scanf("%d", &tasks[i].priority);
            getchar();

            tasks[i].pid = -1;
            tasks[i].started = false;
            tasks[i].finished = false;
        }
    }

    int find_highest_priority_ready(int current_time) {
        int idx = -1;
        for (int i = 0; i < n; ++i) {
            if (!tasks[i].finished && tasks[i].arrival_time <= current_time) {
                if (idx == -1 || tasks[i].priority > tasks[idx].priority) {
                    idx = i;
                }
            }
        }
        return idx;
    }

    void start_task(int idx) {
        pid_t pid = fork();
        if (pid == 0) {
            // Process con
            execlp(tasks[idx].command, tasks[idx].command, NULL);
            perror("execlp thất bại");
            exit(1);
        } else {
            tasks[idx].pid = pid;
            tasks[idx].started = true;

            // Ghi log bắt đầu
            char log_msg[256];
            snprintf(log_msg, sizeof(log_msg),
                    "[Time %d] Bắt đầu task: %s (PID=%d)",
                    tasks[idx].arrival_time, tasks[idx].name, pid);
            log_event(log_msg);
        }
    }

    int main() {
        input_tasks();
        FILE *f = fopen("tasks.log", "w");
        if (f) fclose(f); // Clear log file

        int current_time = 0;
        int completed = 0;
        int current_task = -1;

        printf("\n--- Bắt đầu Scheduler ---\n");
        while (completed < n) {
        int next_task = find_highest_priority_ready(current_time);

        if (next_task != -1) {
            if (current_task != -1) {
                // So sánh ưu tiên task mới và còn burst time để preempt
                if (tasks[next_task].priority > tasks[current_task].priority ||
                    (tasks[next_task].priority == tasks[current_task].priority && tasks[next_task].remaining_time < tasks[current_task].remaining_time)) {
                    printf("[Time %d] Preempt: %s -> %s\n", current_time,
                        tasks[current_task].name, tasks[next_task].name);
                    kill(tasks[current_task].pid, SIGSTOP);
                    if (!tasks[next_task].started) {
                        start_task(next_task);
                        sleep(1);
                    } else {
                        kill(tasks[next_task].pid, SIGCONT);
                    }
                    current_task = next_task;
                }
            } else {
                // Nếu chưa có task nào chạy
                if (!tasks[next_task].started) {
                    start_task(next_task);
                    sleep(1);
                } else {
                    kill(tasks[next_task].pid, SIGCONT);
                }
                current_task = next_task;
                printf("[Time %d] Chạy: %s (PID=%d)\n",
                    current_time, tasks[current_task].name, tasks[current_task].pid);
            }
        } else {
            printf("[Time %d] Không có task sẵn sàng\n", current_time);
        }

        // Giảm thời gian chạy còn lại 1 giây
        if (current_task != -1) {
            tasks[current_task].remaining_time--;
            if (tasks[current_task].remaining_time <= 0) {
                // Task hoàn thành, dừng process con
                kill(tasks[current_task].pid, SIGKILL);
                int status;
                waitpid(tasks[current_task].pid, &status, 0);
                printf("[Time %d] Hoàn thành: %s\n", current_time + 1, tasks[current_task].name);

                char log_msg[256];
                snprintf(log_msg, sizeof(log_msg),
                        "[Time %d] Hoàn thành: %s - %s",
                        current_time + 1, tasks[current_task].name,
                        (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? "SUCCESS" : "FAILURE");
                log_event(log_msg);

                tasks[current_task].finished = true;
                completed++;
                current_task = -1;
            }
        }

        sleep(1);
        current_time++;
    }

        printf("\n--- Tất cả task đã hoàn thành! ---\n");
        return 0;
    }
