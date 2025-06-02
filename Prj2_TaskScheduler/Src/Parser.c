#include "Inc/Parser.h"

void parse_input_csv(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Không thể mở input file");
        exit(1);
    }

    char line[MAX_LINE_LEN];
    int  line_no = 0;

    while (fgets(line, sizeof(line), f)) {
        line_no++;
        // Bỏ qua dòng header nếu có (giả sử dòng đầu tiên chứa "name,command,arrival,burst,priority")
        if (line_no == 1) {
            // Kiểm tra xem dòng đầu có phải header không
            if (strstr(line, "name") && strstr(line, "command")) {
                continue;
            }
        }

        if (strlen(line) < 5) continue; // Bỏ qua dòng rỗng

        Task t;
        t.pid            = -1;
        t.started        = false;
        t.finished       = false;
        t.remaining_time = 0;

        // Tách 5 cột
        char *tok = strtok(line, ",");
        if (!tok) continue;
        strncpy(t.name, tok, sizeof(t.name));
        t.name[sizeof(t.name)-1] = '\0';

        tok = strtok(NULL, ",");
        if (!tok) { fprintf(stderr, "Format sai ở dòng %d\n", line_no); exit(1); }
        strncpy(t.command, tok, sizeof(t.command));
        t.command[sizeof(t.command)-1] = '\0';

        tok = strtok(NULL, ",");
        t.arrival_time = atoi(tok);

        tok = strtok(NULL, ",");
        t.burst_time = atoi(tok);
        t.remaining_time = t.burst_time;

        tok = strtok(NULL, ",");
        t.priority = atoi(tok);

        tasks[n++] = t;
        if (n >= MAX_TASKS) break;
    }
    fclose(f);

    // Sau khi parse xong, in ra thông tin để kiểm tra
    printf("[Parse Input] Đã load %d task từ '%s'\n", n, filename);
    for (int i = 0; i < n; ++i) {
        printf("  [%d] %-8s cmd=%s, arrival=%d, burst=%d, prio=%d\n",
               i, tasks[i].name, tasks[i].command,
               tasks[i].arrival_time, tasks[i].burst_time,
               tasks[i].priority);
    }
}

// ======== Hàm parse backup CSV (mode recover) ========
//
// backup_file.csv có format:
//   current_time,42
//   name,command,arrival,burst,remaining,priority,pid,started,finished
//   T1,./prog1,0,5,2,2,12345,1,0
//   T2,./prog2,3,4,0,5,23456,1,1
//   T3,./prog3,6,7,7,3,-1,0,0
//
void parse_backup_csv(const char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        perror("Không thể mở backup file");
        exit(1);
    }

    char line[MAX_LINE_LEN];
    int  line_no = 0;

    // 1) Đọc dòng current_time
    if (fgets(line, sizeof(line), f)) {
        line_no++;
        int ct = 0;
        if (sscanf(line, "current_time,%d", &ct) == 1) {
            current_time = ct;
        } else {
            fprintf(stderr, "Backup CSV format sai (dòng current_time)\n");
            exit(1);
        }
    }

    // 2) Đọc header (bỏ qua)
    if (fgets(line, sizeof(line), f)) {
        line_no++;
    }

    // 3) Đọc từng dòng task
    n = 0;
    while (fgets(line, sizeof(line), f)) {
        line_no++;
        if (strlen(line) < 10) continue;

        Task t;
        int pid_i = -1, started_i = 0, finished_i = 0;
        int cnt = sscanf(
            line,
            "%31[^,],%127[^,],%d,%d,%d,%d,%d,%d,%d",
            t.name,
            t.command,
            &t.arrival_time,
            &t.burst_time,
            &t.remaining_time,
            &t.priority,
            &pid_i,
            &started_i,
            &finished_i
        );
        if (cnt != 9) {
            fprintf(stderr, "Backup CSV format sai ở dòng %d\n", line_no);
            exit(1);
        }

        t.pid      = (pid_t)pid_i;
        t.started  = (started_i != 0);
        t.finished = (finished_i != 0);

        // Nếu PID <= 0 hoặc tiến trình đã exit, coi như chưa start
        if (t.pid <= 0 || kill(t.pid, 0) < 0) {
            t.pid      = -1;
            t.started  = false;
            // Nếu finished==false nhưng pid không còn, ta coi như finished
            if (!t.finished) {
                t.finished = true;
                completed++;
            }
        } else {
            // Gửi SIGSTOP để đảm bảo child nằm trong trạng thái STOPPED
            kill(t.pid, SIGSTOP);
            waitpid(t.pid, NULL, WUNTRACED);
        }

        tasks[n++] = t;
        if (n >= MAX_TASKS) break;
    }
    fclose(f);

    // Tính lại completed
    for (int i = 0; i < n; ++i) {
        if (tasks[i].finished) completed++;
    }

    printf("[Parse Backup] Đã nạp %d task từ '%s', current_time=%d, completed=%d\n",
           n, filename, current_time, completed);
    // In thông tin để kiểm tra
    for (int i = 0; i < n; ++i) {
        printf("  [%d] %-8s cmd=%s, arr=%d, burst=%d, rem=%d, prio=%d, pid=%d, started=%d, finished=%d\n",
               i,
               tasks[i].name, tasks[i].command,
               tasks[i].arrival_time, tasks[i].burst_time,
               tasks[i].remaining_time, tasks[i].priority,
               (int)tasks[i].pid, tasks[i].started, tasks[i].finished);
    }
}

// ======== Hàm save backup CSV ========
//
// Lưu format giống parse_backup_csv nhưng in ra file.
void save_backup_csv(const char *filename) {
    FILE *f = fopen(filename, "w");
    if (!f) {
        perror("Không thể mở file backup để ghi");
        return;
    }
    // 1) Ghi current_time
    fprintf(f, "current_time,%d\n", current_time);

    // 2) Header
    fprintf(f, "name,command,arrival,burst,remaining,priority,pid,started,finished\n");

    // 3) Mỗi task
    for (int i = 0; i < n; ++i) {
        fprintf(f,
            "%s,%s,%d,%d,%d,%d,%d,%d,%d\n",
            tasks[i].name,
            tasks[i].command,
            tasks[i].arrival_time,
            tasks[i].burst_time,
            tasks[i].remaining_time,
            tasks[i].priority,
            (int)tasks[i].pid,
            tasks[i].started ? 1 : 0,
            tasks[i].finished ? 1 : 0
        );
    }

    fclose(f);
    printf("[Save Backup] Đã lưu trạng thái %d task vào '%s'\n", n, filename);
}
