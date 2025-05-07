#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void handle_usr2(int sig) {
    FILE *f = fopen("monitor_cmd.txt", "r");
    if (!f) {
        perror("Cannot open command file");
        return;
    }

    char cmd[512];
    if (fgets(cmd, sizeof(cmd), f)) {
        printf("[MONITOR] Executing: %s", cmd);
        system(cmd);
    }

    fclose(f);
}

void handle_term(int sig) {
    printf("[MONITOR] Shutting down after 2 seconds...\n");
    usleep(2000000); // simulate delay
    exit(0);
}

int main() {
    signal(SIGUSR2, handle_usr2);
    signal(SIGTERM, handle_term);

    printf("[MONITOR] Running with PID %d\n", getpid());

    while (1) {
        pause(); // wait for signals
    }

    return 0;
}
