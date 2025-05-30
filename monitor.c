#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int monitor_fd;
int monitor_out_fd; 


void handle_usr2(int sig) {

    char cmd[512];
    ssize_t n = read(monitor_fd, cmd, sizeof(cmd) - 1);
    if (n <= 0) {
        perror("[MONITOR] read");
        return;
    }
    cmd[n] = '\0';
    printf("[MONITOR] Executing: %s", cmd);
    system(cmd);
    /*
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
    */
}

void handle_term(int sig) { 
    printf("[MONITOR] Shutting down after 2 seconds...\n");
    usleep(20000); 
    exit(0);
}

int main(int argc, char **argv) {

    if (argc != 3) { //verificam ca sa apelact bine programul (2 variabile)
        fprintf(stderr, "Usage: %s <cmd_pipe_read_fd>\n", argv[0]);
        return 1;
    }

    monitor_fd = atoi(argv[1]); 
    monitor_out_fd = atoi(argv[2]);  

    if (dup2(monitor_out_fd, STDOUT_FILENO) < 0){
        perror("dup2 stdout");
        return 1;
    }
    close(monitor_out_fd);

    struct sigaction sa;    
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    
    //am luat fiecare semnal in parte, daca nu e unul se trece la urmatorul
    sa.sa_handler = handle_usr2;    
    if (sigaction(SIGUSR2, &sa, NULL) < 0) {
        perror("sigaction SIGUSR2");
        return 1;
    }

    sa.sa_handler = handle_term;    
    if (sigaction(SIGTERM, &sa, NULL) < 0) {
        perror("sigaction SIGTERM");
        return 1;
    }    



/*
    signal(SIGUSR2, handle_usr2); //sa modific in sigaction !!!!!!!!!!!!
    signal(SIGTERM, handle_term);
*/
    printf("[MONITOR] Running with PID %d\n", getpid());

    while (1) {
        pause(); 
    }

    return 0;
}
