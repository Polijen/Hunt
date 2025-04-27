#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h> //asta e pentru functia de open
#include <unistd.h>
#include <time.h> //pentru screrea timpului intr-un format citibil

//Intrebare pt prof:
// Hunt_ID e un file name
// treasure e doar un nume si o valoare

//la prima rulare sa faca un director special unde se vor tine toate hunturile(directoarele)




//Exemplu de command line de pe campus
// tresure_manager --remove 'game7' 'treasure7'
//hunt_id -- numele directorului ('game7')
//tresure_id  -- numele fisierului??? NU E CHIAR CORECT DAR E POSIBIL ('treasure 2')

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

//functii personale
T_info in_data(){ //ar fi bine sa introduc si verificare la input + limitatoare
  T_info temp;
  printf("Introdu datele comorii:\n");
  printf("ID: ");
  scanf("%s", temp.Treasure_ID);
  printf("User Name: ");
  scanf("%s", temp.User_Name);
  printf("GPS introduceti numele cu virgula fara spatiu inte ele, exemplu: x,y : ");
  if((scanf("%f,%f", &temp.GPS.X, &temp.GPS.Y)) != 2){
    printf("Introdu coordonatele corespunzator\n");
    return temp; //returneaza un temp care nu e bun si va pica la verifiacare, respectiv se verifica datele. Mai exat la valoare va pica programul
    //daca as da exit, as lasa o valoare alocata fara a putea de ai da free... 
  }
  printf("Clue: "); //ca sa citeasca o propozitie normala 
  getchar();
  fgets(temp.clue, sizeof(temp.clue), stdin);
  int len = strlen(temp.clue);
  if (len > 0 && temp.clue[len-1] == '\n') {
      temp.clue[len-1] = '\0';
  }
  printf("Value: ");
  scanf("%d", &temp.value);
  
  return temp;
}

void print_treasure_info(T_info treasure) { //functie de testare
  printf("Treasure ID: %s\n", treasure.Treasure_ID);
  printf("User Name: %s\n", treasure.User_Name);
  printf("GPS Coordinates: (%.2f, %.2f)\n", treasure.GPS.X, treasure.GPS.Y);
  printf("Clue: %s\n", treasure.clue);
  printf("Value: %d\n", treasure.value);
}

char* path_maker(char *hunt_id, char *treasure_id){
  char *path = NULL;
  int len = strlen(hunt_id) + strlen(treasure_id) + strlen("Hunts") + 2;
  if ((path = (char*)realloc(path, sizeof(char) * len)) == NULL){
    perror("Erroare de alocare\n");
    exit(-1);
  }
  strcpy(path, "Hunts");
  strcat(path, "/");
  strcat(path, hunt_id);
  strcat(path, "/");
  strcat(path, treasure_id);
  //printf("%s\n", path); //folosit pentru a verifica stringul de path
  return path;
}

char* folder_path_maker(char *hunt_id){
  char *folder_path = malloc(sizeof(char) * (strlen(hunt_id) + 8 ));
  strcpy(folder_path, "Hunts/");
  strcat(folder_path, hunt_id);
  return (folder_path);
}



//functiile de implementat
void log_operation(char *action, char *hunt_id, char *treasure_id) {

  char *path = path_maker(hunt_id, "logged_hunt");
  int fd = open(path, O_WRONLY | O_CREAT | O_APPEND, 0644);
  if (fd < 0){
    perror("Nu am putut deschide fisierul de log");
    free(path);
    return;
  }
  
  time_t now = time(NULL);
  struct tm *tm_info = localtime(&now);
  char time_str[64];
  strftime(time_str, sizeof(time_str), "%d-%m-%Y %H:%M:%S", tm_info);
  
  
  char log_line[1024];
  snprintf(log_line, sizeof(log_line), "%s - %s | ID: %s\n", time_str, action, treasure_id != NULL ? treasure_id : "-");
  
  
  if ((write(fd, log_line, strlen(log_line))) == -1){
    perror("Erroare de scriere\n");
    close(fd);
    free(path);
    exit(-2);
  }
  
  close(fd);
  free(path);
  
  char linkname[256];
  snprintf(linkname, sizeof(linkname), "logged_hunt-%s", hunt_id);
  struct stat sb;
  if (lstat(linkname, &sb) == -1) { // check for link existance 
    char *file = path_maker(hunt_id, "logged_hunt");
    if (symlink(file, linkname) == -1){
      perror("Eroare la creare symlink pentru log");
    }
    free(file);
  }
}

int data_verification(T_info data){
  int good = 0; //we want it to be 0
  if((strlen(data.clue) == 0 ) || (strlen(data.Treasure_ID) == 0 ) || (strlen(data.User_Name) == 0 )){
    good = 1;
  }
  if(data.value <= 0){
    printf("Valoarea trebuie sa fie un numar pozitiv");
    good = 1;
  }
  return good;
}

//functii cu operatii

void add(char *hunt_id){ //Add a new treasure to the specified hunt (game session). Each hunt is stored in a separate directory.
  
  printf("Adaugam sesiunea: %s\n", hunt_id);
  char ask;
  char *folder_path = folder_path_maker(hunt_id);
 
  DIR *dr = opendir(folder_path);
  if (dr == NULL){
    printf("Doresti o noua sesiune ? [y, n]\n");
    if((scanf("%c", &ask)) != 1){
      perror("Introducere de data incompatibila");
      return;
    }
    if(ask == 'y'){
      mkdir(folder_path,  S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IROTH ); //creeaza directorul si dupa scrie in el
      
    }
    else if(ask == 'n'){
      printf("Reintrodu comanda cu numele corespunzator sesiunii existente\n");
      free(folder_path);
      closedir(dr);
      exit(0);
    }
  }
  closedir(dr);

  T_info data = in_data(); //trebuie sa verific integritatea datelor din data
  if (data_verification(data) == 1){ //ceva fin data nu a fost scris bine
    free(folder_path);
    exit(1);
  }
  char *path = path_maker(hunt_id, "Treasures.txt");
  int fd = open(path,O_WRONLY | O_CREAT | O_APPEND, 0644); // 000 101 100 100
  printf("\n%s\n", path);

  char buffer[2048];//screm asa pentru a fi mai usor de extras datele cu strtok sau alte functii
  sprintf(buffer, "%s|%s|%.2f|%.2f|%s|%d\n", data.Treasure_ID, data.User_Name, data.GPS.X, data.GPS.Y, data.clue, data.value);


  if( write(fd, buffer, strlen(buffer)) == -1 ){//scrie automat in fisier prin concatenare
    perror("Erroare de scriere\n");
  }

  close(fd);

  log_operation("add", hunt_id, data.Treasure_ID); //e pus aici pentru ca doresc sa inregistrez si numele la treasure 

//eliberam stringurile de path
 free(path);
 free(folder_path);
}

void list(char *hunt_id){ //List all treasures in the specified hunt. First print the hunt name, the (total) file size and last modification time of its treasure file(s), then list the treasures.
  char *folder_path = folder_path_maker(hunt_id);
  char *path = path_maker(hunt_id, "Treasures.txt");
  
  printf("Hunt Name: %s \n", hunt_id); //printeaza numele huntului specific
  
  struct stat st; //Printarea marimiii fisierului in bytes/data modificatii
  if (stat(path, &st) == 0) {
    printf("File size: %ld bytes\n", st.st_size);
    printf("Last modification time (secunde): %ld\n", st.st_mtim.tv_sec);

    // Convert modification time to local time
    struct tm *mod_time = localtime(&st.st_mtim.tv_sec);

    // Format time as a string
    char time_str[100];
    strftime(time_str, sizeof(time_str), "%d/%m/%Y %H:%M:%S", mod_time);
    
    printf("Last modification time: %s\n", time_str);

  }

  int fd = open(path, O_RDONLY );
  char *buffer = malloc(sizeof(char) * 129); //facem un buffer penru a citit, si ii dam free la final
  if(buffer == NULL){ //am marit cu 1 pentru a tine cont de '\0' ce repara o problema la printf de terminare de sir
    perror("Erroare de alocare de memorie in functia list\n");
    free(buffer);
    free(folder_path);
    free(path);
    close(fd);
    exit(-1);
  }
  printf("\nTreasures: \n");
  int Bytes_cititi = 0; //aparent int functioneaza dar nu e varianta preferata din ceva motive de marime. ssize_t e preferat
  while((Bytes_cititi = read(fd, buffer, 128)) > 0){ //listam treasures
    buffer[Bytes_cititi] = '\0'; // adaugăm terminator de string pentru al putea printa, fara el printul printeaza ceva random ca nu are unde sa se opreasca din buffer
    printf("%s", buffer);
}

  close(fd);
  free(buffer);
  free(path);
  free(folder_path);
}

void view(char *hunt_id, char *ID){ //View details of a specific treasure
  char *folder_path = folder_path_maker(hunt_id);
  char *path = path_maker(hunt_id, "Treasures.txt");
  
  int fd = open(path, O_RDONLY); //doar sa citim un specific treasure
  char *buffer = malloc(sizeof(char) * 1024); 
  if(buffer == NULL){
    perror("Erroare de alocare de memorie in functia list\n");
    free(buffer);
    close(fd);
    free(path);
    free(folder_path);
    exit(-1);
  }

  int bytes_read;
  char *current_line = NULL;
  int current_line_len = 0;
  int current_line_capacity = 0;
  char *found_line = NULL;
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
        free(folder_path);
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
                free(folder_path);
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
            free(folder_path);
            exit(-1);
        }
        id_token = strtok(line_copy, "|"); //Asta e numele la treasure

        if (id_token != NULL && strcmp(id_token, ID) == 0){
            printf("Am gasit comoara:\n");
            printf("%s\n", current_line);
            found_line = strdup(current_line);
            free(line_copy);
            break; //exit inner loop (a fost gasit the treasure)
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
            free(folder_path);
            exit(-1);
        }
        strcpy(remaining_buffer, last_pos);
    }

    free(combined_buffer);

    if (found_line != NULL){
        break; // Exit outer loop if found
    }
}

if (found_line == NULL){
  printf("Comoara cu ID-ul '%s' nu a fost gasita.\n", ID);
}

close(fd);
free(path);
free(folder_path);
free(current_line);
free(found_line);
free(remaining_buffer);

}

void remove_treasure(char *hunt_id, char *ID){ //Remove treasure, trebuie sa le sterg din fisier

  char *folder_path = folder_path_maker(hunt_id);
  char *path = path_maker(hunt_id, "Treasures.txt");
  char *new_path = path_maker(hunt_id, "Temp.txt");
  int fd_temp = open(new_path, O_WRONLY | O_APPEND | O_CREAT| O_RDONLY, 0644); //am adaugat Read penru ca imi parea o iconita cu un lock pe fisier cand ma uitam din File Manager
  int fd = open(path, O_RDONLY); //doar sa citim un specific treasure
  char *buffer = malloc(sizeof(char) * 1024); 
  if(buffer == NULL){
    perror("Erroare de alocare de memorie in functia list\n");
    free(buffer);
    close(fd);
    close(fd_temp);
    free(new_path);
    free(path);
    free(folder_path);
    exit(-1);
  }

  int bytes_read;
  char *current_line = NULL;
  int current_line_len = 0;
  int current_line_capacity = 0;
  char *remaining_buffer = NULL;
  int remaining_len = 0;
  int found = 0;

  while ((bytes_read = read(fd, buffer, sizeof(buffer) - 1)) > 0){
    buffer[bytes_read] = '\0'; // Make a string to be a string in C :D

    //Append ce a ramas de la citirea precendenta ce nu ara o linie comora la read-ul current
    char *combined_buffer = NULL;
    int combined_len = remaining_len + bytes_read + 1;
    if ((combined_buffer = malloc(combined_len)) == NULL){
        perror("Eroare de alocare memorie in functia remove_treasure");
        free(remaining_buffer);
        free(buffer);
        close(fd);
        close(fd_temp);
        free(new_path);
        free(path);
        free(folder_path);
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
                perror("Eroare de realocare memorie in functia remove_treasure");
                free(current_line);
                free(combined_buffer);
                free(buffer);
                close(fd);
                close(fd_temp);
                free(new_path);
                free(path);
                free(folder_path);
                exit(-1);
            }
            current_line = temp;
        }
        strncpy(current_line + current_line_len, last_pos, line_part_len);
        current_line_len += line_part_len;
        current_line[current_line_len] = '\0';

        char *id_token;
        char *line_copy = strdup(current_line);
        if (line_copy == NULL) {
            perror("Eroare de alocare memorie");
            free(current_line);
            free(combined_buffer);
            free(buffer);
            close(fd);
            close(fd_temp);
            free(new_path);
            free(path);
            free(folder_path);
            exit(-1);
        }
        id_token = strtok(line_copy, "|");

        if (id_token != NULL && strcmp(id_token, ID) == 0) {
            found = 1;
        } else {
            write(fd_temp, current_line, strlen(current_line));
            write(fd_temp, "\n", 1); //NU prea imi dau seama daca chiar trebuie \n, ca imi pare ca se pierde dar idk 
        }
        free(line_copy);

        current_line_len = 0; // Reset for the next line
        last_pos = newline_ptr + 1; // Move past the newline
    }

    // Store any remaining part of the combined buffer
    if (*last_pos != '\0') {
        remaining_len = strlen(last_pos);
        remaining_buffer = malloc(remaining_len + 1);
        if (remaining_buffer == NULL) {
            perror("Eroare de alocare memorie");
            free(current_line);
            free(combined_buffer);
            free(buffer);
            close(fd);
            close(fd_temp);
            free(new_path);
            free(path);
            free(folder_path);
            exit(-1);
        }
        strcpy(remaining_buffer, last_pos);
    }

    free(combined_buffer);
}

close(fd);
close(fd_temp);
free(buffer);
free(current_line);
free(remaining_buffer);

//Asta e bucata noua care se ocupa ca fisierul sa ramana consistent
if (found){
    if (remove(path) == -1){
        perror("Eroare la stergerea fisierului original");
    }else{
        if (rename(new_path, path) == -1){
            perror("Eroare la redenumirea fisierului temporar");
        }else{
            printf("Comoara cu ID-ul '%s' a fost stearsa.\n", ID);
        }
    }
}
else{
    printf("Comoara cu ID-ul '%s' nu a fost gasita.\n", ID);
    if (remove(new_path) == -1){
        perror("Eroare la stergerea fisierului temporar");
    }
}

free(new_path);
free(path);
free(folder_path);
}




void remove_hunt(char *hunt_id){ //Remove an entire hunt
  char *folder_path = folder_path_maker(hunt_id);
  char path[64] = "Hunts/"; //realistic ne trebuie 27 de caractere, restul e sa fie
  char *file_name = "/Treasures.txt";
  strcat(path, hunt_id);
  strcat(path, file_name);
  //DIR *dr = opendir(folder_path);
  
  if(remove(path) != 0){ //sa adaug stergere pentru mai multe fisiere, dar nu e cazul pentru ca lucrez doar cu un singur fisier
    perror("Erroare de stergere\n");

  }
  
  if(rmdir(folder_path) != 0){ //stergem fisierul vechi, dupa ce lam golit
    perror("Erroare de stergere\n");

  }
  free(folder_path);
  //closedir(dr);
}





int main(int argc, char **argv){

  if (argc < 3 || argc > 4){ //verificam ca numarul de argumente corect
    perror("Arguments do not match\n");
    exit(-2);
  }
  //Verific/creiez un nou director "Hunts" ce va tine toata hunt-urile
  DIR *dr = opendir("Hunts");
  if (dr == NULL){
    mkdir("Hunts", S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IROTH ); //Super Director
  
    closedir(dr);
  }
  else{
    closedir(dr);
  }

  
  
  if (!strcmp(argv[1], "add")){
    // printf("Adunare\n, %s" ,argv[2]);
    add(argv[2]);
    
  }
  else if (!strcmp(argv[1], "list")){
    list(argv[2]);
    log_operation(argv[1], argv[2], NULL);
  }
  else if (!strcmp(argv[1], "view")){
    view(argv[2], argv[3]);
    log_operation(argv[1], argv[2], NULL);
  }
  else if (!strcmp(argv[1], "remove_treasure")){
    remove_treasure(argv[2] ,argv[3]);
    log_operation(argv[1], argv[2], NULL);
  }
  else if (!strcmp(argv[1], "remove_hunt")){
    remove_hunt(argv[2]);
    log_operation(argv[1], argv[2], NULL);
  }
  else{
    printf("Operation does not exist\n");
  }
  
  
  return 0;
}
