#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h> 
#include <unistd.h>
#include <time.h>

void start_monitor(){ //starts a separate background process that monitors the hunts and prints to the standard output information about them when asked to


}

void list_hunts(){ //the monitor to list the hunts and the total number of treasures in each


}

void list_treasures(){ //tells the monitor to show the information about all treasures in a hunt, the same way as the command line at the previous stage did

}

void view_treasure(){ //tells the monitor to show the information about a treasure in hunt, the same way as the command line at the previous stage did

}
void stop_monitor(){ //asks the monitor to end then returns to the prompt. Prints monitor's  termination state when it ends.
/*
If the stop_monitor command was given, any user attempts to input commands will end up with an error message until 
the monitor process actually ends. To test this feature, the monitor program will delay its exit (usleep()) when it responds.
 The communication with the monitor process is done using signals
*/


}

void exit(){ //if the monitor still runs, prints an error message, otherwise ends the program

}

int main() {
    char command[256];
    int monitor_shutting_down = 0;

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
