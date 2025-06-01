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
    char command[MAX_CMD_LEN];
    int arrival_time;
    int priority;
    int burst_time;
    int remaining_time;
    pid_t pid;
    bool started;
    bool finished;
} Task;

Task tasks[MAX_TASKS];
int n;

/*===================== Định nghĩa các hàm con =====================*/

// 1. Ghi log sự kiện vào file
void log_event(const char *msg) {
    FILE *f = fopen("tasks.log", "a");
    if (f) {
        fprintf(f, "%s\n", msg);
        fclose(f);
    }
}

// 2. Đọc input từ người dùng
void input_tasks() {
    printf("Nhập số lượng task: ");
    scanf("%d", &n);
    getchar(); // xóa newline

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

// 3. Khởi tạo file log (xoá nội dung cũ)
void init_logfile() {
    FILE *f = fopen("tasks.log", "w");
    if (f) fclose(f);
}

// 4. Tìm task “ready” có priority cao nhất tại thời điểm current_time
int find_highest_priority_ready(int current_time) {
    int idx = -1;
    for (int i = 0; i < n; ++i) {
        if (!tasks[i].finished && tasks[i].arrival_time <= current_time) {
            if (idx == -1 ||
                tasks[i].priority > tasks[idx].priority ||
                (tasks[i].priority == tasks[idx].priority &&
                 tasks[i].remaining_time < tasks[idx].remaining_time))
            {
                idx = i;
            }
        }
    }
    return idx;
}

// 5. Fork + exec một task mới
void start_task(int idx) {
    pid_t pid = fork();
    if (pid == 0) {
        // Child
        execlp(tasks[idx].command, tasks[idx].command, NULL);
        perror("execlp thất bại");
        exit(1);
    } else {
        // Parent
        tasks[idx].pid = pid;
        tasks[idx].started = true;

        char log_msg[256];
        snprintf(log_msg, sizeof(log_msg),
                 "[Time %d] Bắt đầu task: %s (PID=%d)",
                 tasks[idx].arrival_time, tasks[idx].name, pid);
        log_event(log_msg);
    }
}

// 6. Xử lý preempt hoặc start mới, trả về idx của current_task sau khi xử lý
int dispatch_task(int current_task, int next_task, int current_time) {
    if (next_task == -1) {
        // Không có task ready, vẫn giữ current_task cũ (có thể = -1)
        return current_task;
    }

    if (current_task == -1) {
        // Chưa có task nào chạy, chạy next_task luôn
        if (!tasks[next_task].started) {
            start_task(next_task);
            // sleep(1);
        } else {
            kill(tasks[next_task].pid, SIGCONT);
        }
        printf("[Time %d] Chạy: %s (PID=%d)\n",
               current_time, tasks[next_task].name, tasks[next_task].pid);
        return next_task;
    }

    // Nếu đã có task chạy, kiểm tra preempt
    bool higher_priority = tasks[next_task].priority > tasks[current_task].priority;
    bool same_priority_shorter = (tasks[next_task].priority == tasks[current_task].priority &&
                                  tasks[next_task].remaining_time < tasks[current_task].remaining_time);

    if (higher_priority || same_priority_shorter) {
        printf("[Time %d] Preempt: %s -> %s\n", current_time,
               tasks[current_task].name, tasks[next_task].name);
        kill(tasks[current_task].pid, SIGSTOP);

        if (!tasks[next_task].started) {
            start_task(next_task);
            // sleep(1);
        } else {
            kill(tasks[next_task].pid, SIGCONT);
        }
        return next_task;
    }
    // Nếu không preempt, tiếp tục chạy current_task
    return current_task;
}

// 7. Giảm remaining_time, kiểm tra nếu task hoàn thành
int check_and_handle_completion(int current_task, int current_time, int *completed) {
    if (current_task == -1) return -1;

    tasks[current_task].remaining_time--;
    if (tasks[current_task].remaining_time <= 0) {
        kill(tasks[current_task].pid, SIGKILL);
        int status;
        waitpid(tasks[current_task].pid, &status, 0);
        printf("[Time %d] Hoàn thành: %s\n", current_time + 1,
               tasks[current_task].name);

        char log_msg[256];
        snprintf(log_msg, sizeof(log_msg),
                 "[Time %d] Hoàn thành: %s - %s",
                 current_time + 1, tasks[current_task].name,
                 (WIFEXITED(status) && WEXITSTATUS(status) == 0) ? "SUCCESS" : "FAILURE");
        log_event(log_msg);

        tasks[current_task].finished = true;
        (*completed)++;
        return -1; // Không còn task chạy
    }
    return current_task;
}

// 8. Vòng lặp scheduler (mỗi giây)
void scheduler_loop() {
    int current_time = 0;
    int completed = 0;
    int current_task = -1;

    printf("\n--- Bắt đầu Scheduler ---\n");
    while (completed < n) {
        int next_task = find_highest_priority_ready(current_time);
        current_task = dispatch_task(current_task, next_task, current_time);
        current_task = check_and_handle_completion(current_task, current_time, &completed);

        sleep(1);
        current_time++;
    }
    printf("\n--- Tất cả task đã hoàn thành! ---\n");
}

/*===================== Hàm main() ngắn gọn =====================*/

int main() {
    input_tasks();    // Nhập danh sách task
    init_logfile();   // Chuẩn bị file tasks.log (xoá cũ)
    scheduler_loop(); // Chạy vòng lặp định thời và xử lý preemptive priority
    return 0;
}
