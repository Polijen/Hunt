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

    if ((fclose(f)) != 0){
        perror("Erroare de inchide e fisierului de\n");
    }
}

void handle_term(int sig) {
    printf("[MONITOR] Shutting down after 2 seconds...\n");
    usleep(20000); 
    exit(0);
}

int main() {
    signal(SIGUSR2, handle_usr2); //sa modific in sigaction !!!!!!!!!!!!
    signal(SIGTERM, handle_term);

    printf("[MONITOR] Running with PID %d\n", getpid());

    while (1) {
        pause(); 
    }

    return 0;
}
