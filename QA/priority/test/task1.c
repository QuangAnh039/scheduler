#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void log_event(const char *msg) {
    FILE *f = fopen("tasks1.log", "a");  // Mở file append
    if (!f) {
        perror("Mở file lỗi");
        return;
    }
    fprintf(f, "%s\n", msg);
    fclose(f);
}

int main() {
    for (int i = 0; i < 5; ++i) {
        
        char log_msg[100];
        snprintf(log_msg, sizeof(log_msg), "Task1 ghi lần thứ %d (PID=%d)", i+1, getpid());
        log_event(log_msg);
        sleep(1);
    }
    return 0;
}
