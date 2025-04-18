#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h> //asta e pentru functia de open
#include <unistd.h>

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
  printf("%s\n", path);
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

  char buffer[2048];//screm asa pentru a fi mai usor de extras datele cu strtok
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
  DIR *dr = opendir(hunt_id);


  closedir(dr);
}

void view(char *hunt_id, char *ID){ //View details of a specific treasure
  
  //DIR *dr = opendir();


}

void remove_treasure(char *hunt_id, char *ID){ //Remove treasure, trebuie sa le sterg din fisier
  char *path = path_maker(hunt_id, ID);
  if(remove(path) != 0){
    perror("Erroare de stergere\n");

  }
}

void remove_hunt(char *hunt_id){ //Remove an entire hunt

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
    view(argv[1], argv[2]);
  }
  else if (!strcmp(argv[1], "remove_treasure")){
    remove_treasure(argv[1] ,argv[2]);
    
  }
  else if (!strcmp(argv[1], "remove_hunt")){
    printf("rn hunt\n");
  }
  else{
    printf("Operation does not exist\n");
  }
  
  
  return 0;
}
