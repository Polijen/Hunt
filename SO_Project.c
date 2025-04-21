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
  printf("GPS (x,y): ");
  scanf("%f,%f", &temp.GPS.X, &temp.GPS.Y);
  printf("Clue: ");
  scanf("%s", temp.clue);
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

void add(char *hunt_id){ //Add a new treasure to the specified hunt (game session). Each hunt is stored in a separate directory.
  
  printf("Adaugam sesiunea: %s\n", hunt_id);
  char ask;
  char *folder_path = folder_path_maker(hunt_id);
 
  DIR *dr = opendir(folder_path);
  if (dr == NULL){
    printf("Doresti o noua sesiune ? [y, n]\n");
    scanf("%c", &ask);
    if(ask == 'y'){
      mkdir(folder_path,  S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IROTH ); //creeaza directorul si dupa scrie in el
      
    }
    else if(ask == 'n'){
      printf("Reintrodu comanda cu numele corespunzator sesiunii existente\n");
      exit(0);
    }
  }
  closedir(dr);

  T_info data = in_data(); //

  char *path = path_maker(hunt_id, "Treasures.txt");
  int fd = open(path,O_WRONLY | O_CREAT | O_APPEND, 0644); // 000 101 100 100
  printf("\n%s\n", path);

  char buffer[2048];//screm asa pentru a fi mai usor de extras datele cu strtok sau alte functii
  sprintf(buffer, "%s|%s|%.2f|%.2f|%s|%d\n", data.Treasure_ID, data.User_Name, data.GPS.X, data.GPS.Y, data.clue, data.value);


  if( write(fd, buffer, strlen(buffer)) == -1 ){//scrie automat in fisier prin concatenare
    perror("Erroare de scriere\n");
  }

  close(fd);

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
    exit(-1);
  }
  int Bytes_cititi = 0;
  while ((Bytes_cititi = read(fd, buffer, 1024)) > 0){//aici citim si prelucram stringul pentru a gasi si arata doar hunt-ul 


  }

  close(fd);
  free(path);
  free(folder_path);
}

void remove_treasure(char *hunt_id, char *ID){ //Remove treasure, trebuie sa le sterg din fisier
  //char *path = path_maker(hunt_id, ID);
  

  
  
  /*
  if(remove(path) != 0){ //stergem fisierul vechi
    perror("Erroare de stergere\n");

  }*/
}

void remove_hunt(char *hunt_id){ //Remove an entire hunt
  char *folder_path = folder_path_maker(hunt_id);
  char path[64] = "Hunts/"; //realistic ne trebuie 27 de caractere, restul e sa fie
  char *file_name = "/Treasures.txt";
  strcat(path, hunt_id);
  strcat(path, file_name);
  
  if(remove(path) != 0){ //sa adaug stergere pentru mai multe fisiere
    perror("Erroare de stergere\n");

  }
  
  if(rmdir(folder_path) != 0){ //stergem fisierul vechi, dupa ce lam golit
    perror("Erroare de stergere\n");

  }
  free(folder_path);
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
  }
  else if (!strcmp(argv[1], "view")){
    view(argv[2], argv[3]);
  }
  else if (!strcmp(argv[1], "remove_treasure")){
    remove_treasure(argv[2] ,argv[3]);
    
  }
  else if (!strcmp(argv[1], "remove_hunt")){
    remove_hunt(argv[2]);
  }
  else{
    printf("Operation does not exist\n");
  }
  
  
  return 0;
}
