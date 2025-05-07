#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h> 
#include <unistd.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>



char* path_maker(char *f_name ,char *hunt_id, char *treasure){
    char *path = NULL;
    //int len = strlen(hunt_id) + strlen(treasure) + strlen("./p") + 3 + strlen(f_name);
    if ((path = (char*)realloc(path, sizeof(char) * 512)) == NULL){
        perror("Erroare de alocare\n");
        exit(-1);
      }
      path[0] = '\0'; //pentru a nu avea unwanted behaviour 
      strcat(path, "./p");
      strcat(path, " ");
    if (f_name != NULL){
        strcat(path, f_name);
    }
    else{
        printf("Function name not found\n");
    }
    strcat(path, " ");
    if (hunt_id != NULL){
        strcat(path, hunt_id);
    }
    else{
        printf("Function name not found\n");
    }
    strcat(path, " ");
    if (treasure != NULL){
        strcat(path, treasure);
    }
    return path;
}

//everything lower than this comment are the fuinctions requested in the documentation
pid_t pid = -1;

void start_monitor(){ //starts a separate background process that monitors the hunts and prints to the standard output information about them when asked to
    if(pid > 0){
        printf("Monitor value is not good rn\n"); //testing because I can
        return;
    }

    pid = fork();
    if(pid == 0){ //proces copil, unde dam execute la program 
        printf("Avem codul copil\n");
        execl("./o", "o", NULL);

    }
    else if(pid > 0){ //proces parinte
        printf("Monitor is PID: %d\n", pid);
    }
    else{ //nu sa deschis adica e -1
        perror("Erroare la fork\n");
    }

}

void list_hunts(){//the monitor to list the hunts and the total number of treasures in each
    //functie new ?? 
    DIR *dir;
    struct dirent *entry;

    dir = opendir("./Hunts");
    if (dir == NULL) {
        perror("Unable to open hunts directory\n");
        return;
    }

    printf("Available hunts:\n");
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char path[512];
        snprintf(path, sizeof(path), "./Hunts/%s", entry->d_name);
        struct stat statbuf;
        if (stat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
            printf("- %s\n", entry->d_name);
        }
    }

    closedir(dir);
}


void list_treasures(){ //tells the monitor to show the information about all treasures in a hunt, the same way as the command line at the previous stage did
    //practic functia list din phase1
    char hunt_id[128];
    printf("Introdu Hunt-ul: ");
    if (scanf("%s", hunt_id) != 1){
        perror("Erroare de citire hunt_id: \n");
        return;
    }

    char *path = path_maker("list", hunt_id, NULL);

    system(path); //functioneaza fara monitor

    free(path);
}

void view_treasure(){ //tells the monitor to show the information about a treasure in hunt, the same way as the command line at the previous stage did
    char hunt_id[128]; //practic comanda view din phase 1
    char treasure[218];
    printf("Introdu Hunt-ul: ");
    if (scanf("%s", hunt_id) != 1){
        perror("Erroare de citire hunt_id: \n");
        return;
    }
    printf("Introdu Treasure-ul: ");
    if (scanf("%s", treasure) != 1){
        perror("Erroare de citire treasure: \n");
        return;
    }


    char *path = path_maker("list", hunt_id, treasure);

    system(path); //functioneaza fara monitor

    free(path);
    
}
void stop_monitor( ){ //asks the monitor to end then returns to the prompt. Prints monitor's  termination state when it ends.
/*
If the stop_monitor command was given, any user attempts to input commands will end up with an error message until 
the monitor process actually ends. To test this feature, the monitor program will delay its exit (usleep()) when it responds.
 The communication with the monitor process is done using signals
*/
    if(pid <= 0){
        printf("The monitor is not running\n");
        return;
    }

    //wait(pid);
    kill(pid, SIGTERM);
    waitpid(pid, NULL, 0);

    pid = -1; //reset ig
}

void exit_program( ){ //if the monitor still runs, prints an error message, otherwise ends the program
    if(pid > 0){
        printf("Monitor is running, stop it first !!! \n");
        return;
    }
    printf("We will miss you \n");
    exit(0);
}

int main() {
    char command[256];
    int monitor_shutting_down = 0;
    //pid_t pid = -1; //asta il vom trimite de colo-n colo, -1 pentru ca e valoarea de erroare
    //system("./p list hunt1"); //am gasit un cheap version
    while (1) {
        printf(">> ");
        if (!fgets(command, sizeof(command), stdin)) {
            break;
        }
        if (monitor_shutting_down) {
            printf("Monitor is shutting down, please wait...\n");
            continue;
        }

        if (strncmp(command, "start_monitor", 13) == 0) {
            start_monitor();
        } else if (strncmp(command, "list_hunts", 10) == 0) {
            list_hunts();
        } else if (strncmp(command, "list_treasures", 14) == 0) {
            list_treasures();
        } else if (strncmp(command, "view_treasure", 13) == 0) {
            view_treasure();
        } else if (strncmp(command, "stop_monitor", 12) == 0) {
            stop_monitor();
        } else if (strncmp(command, "exit", 4) == 0) {
            exit_program();
        } else {
            printf("Unknown command.\n");
        }
    }

    return 0;
}
