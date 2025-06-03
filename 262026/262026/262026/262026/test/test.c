#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

void my_handler(int signo)
{
    if (SIGINT == signo)
    {
        printf("Hello world\n");
    }
}

int main()
{
    // Đăng ký signal handler
    signal(SIGINT, my_handler);

    // In ra PGID của tiến trình cha
    printf("[PARENT] PID: %d, PGID: %d\n", getpid(), getpgrp());

    pid_t pid = fork();
    if (pid == 0)
    {
        // Tiến trình con
        printf("[CHILD] PID: %d, PGID: %d\n", getpid(), getpgrp());
        
        // Dùng exec để chạy /bin/ls thay thế tiến trình con
        execlp("./main", "./main", NULL);
        while(1);
        // Nếu exec fail
        perror("execlp");
        exit(1);
    }
    else if (pid > 0)
    {
        // Tiến trình cha: chờ con kết thúc
        while(1);
        wait(NULL);
    }
    else
    {
        perror("fork");
        return 1;
    }

    return 0;
}
