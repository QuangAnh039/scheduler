#include "file_oper.h"

int parse_input(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("Error opening input file");
        return -1;
    }

    char line[1024];
    int line_num = 0;

    // Đọc từng dòng trong file
    while (fgets(line, sizeof(line), fp)) {
        // Bỏ ký tự newline
        line[strcspn(line, "\n")] = '\0';

        // Dòng đầu tiên: chứa current_time
        if (line_num == 0) {
            // Format mong muốn: current_time,<giá trị>
            if (sscanf(line, "current_time,%d", &current_time) != 1) {
                fprintf(stderr, "Malformed current_time line: %s\n", line);
                current_time = 0;  // mặc định = 0 nếu lỗi
            }
        }
        // Dòng thứ hai: tiêu đề CSV, bỏ qua luôn
        else if (line_num == 1) {
            // expecting: name,command,arrival_time,priority,burst_time,remaining_time,started,finished
            // Không làm gì
        }
        else {
            // Các dòng dữ liệu task
            Task task;
            int started_int, finished_int;
            // Đảm bảo format đủ 8 trường
            int scanned = sscanf(
                line,
                "%19[^,],%255[^,],%d,%d,%d,%d,%d,%d",
                task.name,            // chuỗi name (tối đa 19 ký tự)
                task.command,         // chuỗi command (tối đa 255 ký tự)
                &task.arrival_time,
                &task.priority,
                &task.burst_time,
                &task.remaining_time,
                &started_int,
                &finished_int
            );

            if (scanned != 8) {
                fprintf(stderr, "Warning: Malformed task line: %s\n", line);
                // Bỏ qua nếu format không đúng
                line_num++;
                continue;
            }

            task.started  = (started_int  != 0);
            task.finished = (finished_int != 0);
            task.pid      = -1;  // Chưa fork nên chưa có PID

            // Tạo node mới và nối vào linked list
            Node *new_node = (Node *)malloc(sizeof(Node));
            if (!new_node) {
                perror("malloc failed");
                fclose(fp);
                return -1;
            }
            new_node->task = task;
            new_node->next = NULL;

            if (head == NULL) {
                head = new_node;
            } else {
                Node *temp = head;
                while (temp->next) {
                    temp = temp->next;
                }
                temp->next = new_node;
            }
        }

        line_num++;
    }

    fclose(fp);
    return 0;
}
int save_to_backup(const char *backup_file)
{
    FILE *fp = fopen(backup_file, "w");
    fprintf(fp, "current_time,%d\n", current_time);
    fprintf(fp, "name,command,arrival_time,priority,burst_time,remaining_time,started,finished\n");
    Node *temp = head;
    while (temp != NULL)
    {
        Task task = temp->task;
        fprintf(fp, "%s,%s,%d,%d,%d,%d,%d,%d\n",
                task.name,
                task.command,
                task.arrival_time,
                task.priority,
                task.burst_time,
                task.remaining_time,
                task.started,
                task.finished);
        temp = temp->next;
    }
    fclose(fp);
    return 0;
}