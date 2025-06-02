#include "Inc/Parser.h"

void save_state_to_csv(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        perror("Không thể mở file để ghi backup");
        return;
    }

    // 1) Ghi current_time
    fprintf(f, "current_time,%d\n", current_time);

    // 2) Ghi header task
    fprintf(f, "name,command,arrival,burst,remaining,priority,started,finished\n");

    // 3) Ghi từng task
    for (int i = 0; i < n; ++i) {
        fprintf(f,
            "%s,%s,%d,%d,%d,%d,%d,%d\n",
            tasks[i].name,
            tasks[i].command,
            tasks[i].arrival_time,
            tasks[i].burst_time,
            tasks[i].remaining_time,
            tasks[i].priority,
            tasks[i].started ? 1 : 0,
            tasks[i].finished ? 1 : 0
        );
    }
    fclose(f);
    printf("\n[Save] Đã lưu trạng thái vào '%s' (current_time=%d)\n",
           filename, current_time);
}


int load_state_from_csv(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Không thể mở file backup để đọc");
        return -1;
    }

    char line[512];
    // 1) Đọc dòng current_time
    if (!fgets(line, sizeof(line), f)) {
        fclose(f);
        fprintf(stderr, "File backup trống hoặc format sai\n");
        return -1;
    }
    int saved_time = 0;
    if (sscanf(line, "current_time,%d", &saved_time) != 1) {
        fclose(f);
        fprintf(stderr, "Không parse được current_time từ file\n");
        return -1;
    }
    current_time = saved_time;

    // 2) Bỏ qua header
    if (!fgets(line, sizeof(line), f)) {
        fclose(f);
        fprintf(stderr, "Header không tồn tại hoặc file format sai\n");
        return -1;
    }

    // 3) Đọc từng dòng task
    n = 0;
    while (fgets(line, sizeof(line), f) && n < MAX_TASKS) {
        Task *t = &tasks[n];
        int started_i = 0, finished_i = 0;
        int cnt = sscanf(
            line,
            "%19[^,],%99[^,],%d,%d,%d,%d,%d,%d",
            t->name,
            t->command,
            &t->arrival_time,
            &t->burst_time,
            &t->remaining_time,
            &t->priority,
            &started_i,
            &finished_i
        );
        if (cnt != 8) {
            fprintf(stderr, "Dòng %d format sai: %s", n + 3, line);
            fclose(f);
            return -1;
        }
        t->started = (started_i != 0);
        t->finished = (finished_i != 0);
        t->pid = -1;
        n++;
    }
    fclose(f);

    // 4) Tái tạo tiến trình con cho những task chưa finished
    for (int i = 0; i < n; ++i) {
        if (!tasks[i].finished) {
            pid_t pid = fork();
            if (pid < 0) {
                perror("fork thất bại khi restore task");
                return -1;
            }
            if (pid == 0) {
                // Child: exec chương trình
                execlp(tasks[i].command, tasks[i].command, NULL);
                perror("execlp thất bại khi restore");
                exit(1);
            }
            // Parent: lưu pid và dừng ngay
            tasks[i].pid = pid;
            tasks[i].started = true;
            kill(pid, SIGSTOP);
            waitpid(pid, NULL, WUNTRACED);
        }
    }

    // Tính lại completed
    completed = 0;
    for (int i = 0; i < n; ++i) {
        if (tasks[i].finished) completed++;
    }

    printf("[Load] Đã nạp %d task từ '%s', current_time=%d, completed=%d\n",
           n, filename, current_time, completed);
    return 0;
}
