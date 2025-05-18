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

typedef struct{ //coordonatele 
    float X;
    float Y;
}Cords;
  
  
typedef struct{ //User File ce stochiaza informatia despre trasures
    char Treasure_ID[20];
    char User_Name[128];
    Cords GPS;
    char clue[1024];
    int value;
}T_info;



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
int monitor_shutting_down = 0;
int cmd_pipe = -1;

void start_monitor(){ //starts a separate background process that monitors the hunts and prints to the standard output information about them when asked to
    if(pid > 0){
        printf("Monitor value is not good rn\n"); //testing because I can
        return;
    }


    int pipet[2];
    if (pipe(pipet) < 0) {
        perror("pipe");
        return;
    }


    pid = fork();
    if(pid == 0){ //proces copil, unde dam execute la program 
        //printf("Avem codul copil\n");
        close(pipet[1]);
        char t_string[16];
        snprintf(t_string, sizeof(t_string), "%d", pipet[0]);
        execl("./o", "o", t_string, NULL); //do I really need this ?

    }
    else if(pid > 0){ //proces parinte
        close(pipet[0]);
        cmd_pipe = pipet[1];
        printf("Monitor is PID: %d\n", pid);
    }
    else{ //nu sa deschis adica e -1
        perror("Erroare la fork\n");
    }

}

void list_hunts(){//the monitor to list the hunts and the total number of treasures in each
    //functie new ?? 

    if (pid <= 0) {
        printf("Monitor is not running\n");
        return;
    }

    DIR *dir;
    struct dirent *entry;

    dir = opendir("./Hunts");
    if (dir == NULL) {
        perror("Unable to open hunts directory\n");
        return;
    }

    printf("Available hunts:\n");
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0){ //face skip la folderele . so .. 
            continue;
        }
        char path[512];
        snprintf(path, sizeof(path), "./Hunts/%s", entry->d_name);
        struct stat statbuf;
        if (stat(path, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)){
            printf("- %s\n", entry->d_name); //aici avem numele la director -> incepem sa numeroatam treasures
            
            //make a path to read
            char *path = malloc(sizeof(char) * 32); //cred ca 28 de caractere e suficent
            path[0] = '\0'; //make an empty C string
            strcat(path, "Hunts/");
            strcat(path, entry->d_name); //String a hunt-ului curent
            strcat(path, "/");
            strcat(path, "Treasures.txt"); //folderul final, this is the path

            //open the folder and read lines
            int fd = open(path, O_RDONLY);
            char *buffer = malloc(sizeof(char) *2048); //1184 bytes e marimea la T_Info
            if(buffer == NULL){
                free(path);
                free(buffer);
                exit(-1);
            }
            //numeroteaza treasures
            int counter = 0; //se resetaeaza la fiecare hunt
	    
            int bytes_read;
	        char *remaining_buffer = NULL;
	        int remaining_len = 0;
	    
	    
	        while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0){
	            buffer[bytes_read] = '\0'; // Make a string to be a string in C :D
	      
	            //Append ce a ramas de la citirea precendenta ce nu ara o linie comora la read-ul current
	            char *combined_buffer = NULL;
	            int combined_len = remaining_len + bytes_read + 1;
	            if((combined_buffer = malloc(combined_len)) == NULL){
		            perror("Eroare de alocare memorie in functia view");
		            free(remaining_buffer);
		            close(fd);
		            free(path);
		            exit(-1);
	            }
	            if (remaining_buffer != NULL){ 
		            strcpy(combined_buffer, remaining_buffer);
		            free(remaining_buffer);
		            remaining_buffer = NULL;
	            }
	            else{
		            combined_buffer[0] = '\0'; //cand nu mai am ce sa adaug, sa fie un string gol ca imi face viata mai usora 
	            }
	            strcat(combined_buffer, buffer);
	            remaining_len = 0; // Reset
	      
	            char *newline_ptr;
	            char *last_pos = combined_buffer;
	      
	            while ((newline_ptr = strchr(last_pos, '\n')) != NULL){ //cat timp stringul are "\n" --> avem o linie comoara
                    //daca avem o linie -> count++, dupa trecem sa facem urmatoarea linie
		            counter++;
                    

		            last_pos = newline_ptr + 1; // Move past the newline
	            }
	      
	            // Store any remaining part of the combined buffer
	            if (*last_pos != '\0'){
		            remaining_len = strlen(last_pos);
		            remaining_buffer = malloc(remaining_len + 1);
		            if (remaining_buffer == NULL){
		                perror("Eroare de alocare memorie");
		                free(combined_buffer);
		                close(fd);
		                free(path);
		                exit(-1);
		            }
		            strcpy(remaining_buffer, last_pos);
	            }
	      
	            free(combined_buffer);
	      
            }
	    
            close(fd);
            free(path);
            free(remaining_buffer);
	    
            printf("Number of treasures in %s = %d\n",entry->d_name ,counter);
	    
        }
    }
    
    if ((closedir(dir)) != 0){
      perror("Erroare de inchidere a directorului");
    }
    
}


void list_treasures(){//tells the monitor to show the information about all treasures in a hunt, the same way as the command line at the previous stage did
    //practic functia list din phase1
    if (pid <= 0) {
        printf("Monitor is not running\n");
        return;
    }

    char hunt_id[128];
    printf("Introdu Hunt-ul: ");
    if (scanf("%s", hunt_id) != 1) {
        perror("Erroare de citire hunt_id: \n");
        return;
    }
    getchar();

    char buffer[512];
    int len = snprintf(buffer, sizeof(buffer), "./p list %s\n", hunt_id);
    if((write(cmd_pipe, buffer, len)) == -1){
        perror("Erroare de scriere in pipe");
        return;
    } //scriem in pipe


    /*
    FILE *f = fopen("monitor_cmd.txt", "w");
    if (!f) {
        perror("Can't open command file");
        return;
    }
    fprintf(f, "./p list %s\n", hunt_id);

    if ((fclose(f)) != 0){
        perror("Erroare de inchidere a fisierului executabil dupa scriere\n");
        exit(2);
    }
    */
    if ((kill(pid, SIGUSR2)) == -1){
        perror("Signal failed to be sent\n");
        exit(3);
    }  // tell the monitor to run the command
}


void view_treasure() { //tells the monitor to show the information about a treasure in hunt, the same way as the command line at the previous stage did
    //practic comanda view din phase 1
    if (pid <= 0) {
        printf("Monitor is not running\n");
        return;
    }

    char hunt_id[128];
    char treasure[128];
    printf("Introdu Hunt-ul: ");
    if (scanf("%s", hunt_id) != 1) {
        perror("Erroare de citire hunt_id: \n");
        return;
    }
    printf("Introdu Treasure-ul: ");
    if (scanf("%s", treasure) != 1) {
        perror("Erroare de citire treasure: \n");
        return;
    }
    getchar(); //pentru a scapa de enter 


    char buffer[512];
    int len = snprintf(buffer, sizeof(buffer), "./p view %s %s\n", hunt_id, treasure);
    if((write(cmd_pipe, buffer, len)) == -1){
        perror("Erroare de scriere in pipe");
        return;
    } //scriem in pipe
    
    /*
    FILE *f = fopen("monitor_cmd.txt", "w"); //inlocuim fisierul cu pipes pentrua trimite comanda de executie 
    if (!f) {
        perror("Can't open command file");
        return;
    }
    fprintf(f, "./p view %s %s\n", hunt_id, treasure);

    if ((fclose(f)) != 0){
        perror("Erroare de inchidere a fisierului executabil dupa scriere\n");
        exit(2);
    }
    */
    if ((kill(pid, SIGUSR2)) == -1){
        perror("Signal filed to be sent\n");
        exit(3);

    }
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
    monitor_shutting_down = 1;
    if ((kill(pid, SIGTERM)) == -1){
        perror("Signal filed to be sent\n");
        exit(3);
    }
    if ((waitpid(pid, NULL, 0)) == -1){
        perror("Filed to change the state of the process\n");
        exit(4);

    }
    if((close(cmd_pipe)) == -1){
        perror("Erroare la close pipe");
    } //curata the pipe

    monitor_shutting_down = 0;

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
