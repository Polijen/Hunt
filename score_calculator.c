#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

typedef struct {
    char user[128];
    int score;
} UserScore;


char* path_maker(char *hunt_id){
    char *path = NULL;
    int len = strlen(hunt_id) + strlen("Treasures.txt") + strlen("Hunts") + 2;
    if ((path = (char*)realloc(path, sizeof(char) * len)) == NULL){
      perror("Erroare de alocare\n");
      exit(-1);
    }
    strcpy(path, "Hunts");
    strcat(path, "/");
    strcat(path, hunt_id);
    strcat(path, "/");
    strcat(path, "Treasures.txt");
    //printf("%s\n", path); //folosit pentru a verifica stringul de path
    return path;
  }


void calculate_scores(char *hunt_id){ //View details of a specific treasure
    
    char *path = path_maker(hunt_id);
    
    int fd = open(path, O_RDONLY); //doar sa citim un specific treasure
    char *buffer = malloc(sizeof(char) * 1024); 
    if(buffer == NULL){
      perror("Erroare de alocare de memorie in functia list\n");
      free(buffer);
      close(fd);
      free(path);
      exit(-1);
    }

    UserScore scores[100] = {0}; //array poentru memora sumele de value 
    int scores_count = 0; //utilizatori unici

  
    int bytes_read;
    char *current_line = NULL;
    int current_line_len = 0;
    int current_line_capacity = 0;
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
          int line_part_len = newline_ptr - last_pos;
  
          // Extend current_line
          if (current_line_len + line_part_len >= current_line_capacity){
              current_line_capacity = current_line_len + line_part_len + 1;
              char *temp = realloc(current_line, current_line_capacity);
              if (temp == NULL){
                  perror("Eroare de realocare memorie in functia view _second while");
                  free(current_line);
                  free(combined_buffer);
                  close(fd);
                  free(path);
                  exit(-1);
              }
              current_line = temp;
          }
          strncpy(current_line + current_line_len, last_pos, line_part_len);
          current_line_len += line_part_len;
          current_line[current_line_len] = '\0';
  
          
          //printf("\n%s\n", current_line); //avem linia si o verificam
          char *id_token;
          char *line_copy = strdup(current_line);
          if (line_copy == NULL){
              perror("Eroare de alocare memorie");
              free(current_line);
              free(combined_buffer);
              close(fd);
              free(path);
              exit(-1);
          }
          



            strtok(line_copy, "|");                //skip treasure name
            id_token = strtok(NULL, "|");          //field 1 is Username
            strtok(NULL, "|");                     //skip coordinates
            strtok(NULL, "|");
            strtok(NULL, "|");                    //skip clue text
            char *value_str = strtok(NULL, "|");   //field 4 is value
            int value = value_str ? atoi(value_str) : 0;  //convert to int

            int found = 0;
            for (int i = 0; i < scores_count; i++) {
                if (strcmp(scores[i].user, id_token) == 0) {
                    scores[i].score += value;  //aici adaugam valoarea
                    found = 1;
                    break;
                }
            }
            if (!found && scores_count < 100) { //verificam daca e new user
                strncpy(scores[scores_count].user, id_token, sizeof(scores[scores_count].user)-1);
                scores[scores_count].user[sizeof(scores[scores_count].user)-1] = '\0'; 
                scores[scores_count].score = value;  
                scores_count++;                     
            }

          
          

          free(line_copy);
          
          current_line_len = 0; // Reset for the next line
          last_pos = newline_ptr + 1; // Move past the newline
      }
  
      // Store any remaining part of the combined buffer
      if (*last_pos != '\0'){
          remaining_len = strlen(last_pos);
          remaining_buffer = malloc(remaining_len + 1);
          if (remaining_buffer == NULL){
              perror("Eroare de alocare memorie");
              free(current_line);
              free(combined_buffer);
              close(fd);
              free(path);
              exit(-1);
          }
          strcpy(remaining_buffer, last_pos);
      }
  
      free(combined_buffer);
    }

    for (int i = 0; i < scores_count; i++) {
        printf("%s: %d\n", scores[i].user, scores[i].score);
    }

}

int main(int argc, char **argv) {
    if (argc != 2) return 1;
    calculate_scores(argv[1]);
    return 0;
}
