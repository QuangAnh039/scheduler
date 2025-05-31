#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
    const char* tasks[] = {"./task1", "./task2", "./task3"};
    pid_t pids[3];

    for (int i = 0; i < 3; i++) {
        pid_t pid = fork();
        if (pid < 0) {
            perror("fork thất bại");
            exit(EXIT_FAILURE);
        } else if (pid == 0) {
            // Process con
            printf("Process con %d chạy %s\n", getpid(), tasks[i]);
            execlp(tasks[i], tasks[i], NULL);
            // Nếu execlp thất bại
            perror("execlp thất bại");
            exit(EXIT_FAILURE);
        } else {
            // Process cha lưu PID của con
            pids[i] = pid;
        }
    }

    // Process cha: đợi từng process con kết thúc
    int status;
    for (int i = 0; i < 3; i++) {
        pid_t pid = waitpid(pids[i], &status, WNOHANG); // đợi đúng process con
        if (pid == -1) {
            perror("waitpid thất bại");
        } else {
            printf("Process con %d kết thúc.\n", pid);
            if (WIFEXITED(status)) {
                printf("Thoát với mã %d\n", WEXITSTATUS(status));
            } else if (WIFSIGNALED(status)) {
                printf("Bị kill bởi signal %d\n", WTERMSIG(status));
            }
        }
    }

    printf("Process cha hoàn thành.\n");
    return 0;
}

