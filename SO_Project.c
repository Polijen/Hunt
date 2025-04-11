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
  int len = strlen(hunt_id) + strlen(treasure_id) ;
  if ((path = (char*)realloc(path, sizeof(char) * len)) == NULL){
    perror("Erroare de alocare\n");
    exit(-1);
  } 
  strcpy(path, hunt_id);
  strcat(path, "/");
  strcat(path, treasure_id);
  printf("%s\n", path);
  return path;
}

char* data_buffer(T_info data){


}

//functiile de implementat

void add(char *hunt_id){ //Add a new treasure to the specified hunt (game session). Each hunt is stored in a separate directory.
  
  printf("Adaugam sesiunea: %s\n", hunt_id);
  char ask;
  FILE *f = NULL;
  
  DIR *dr = opendir(hunt_id);
  if (dr == NULL){
    printf("Doresti o noua sesiune ? [y, n]\n");
    scanf("%c", &ask);
    if(ask == 'y'){
      mkdir(hunt_id,  S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IWOTH | S_IROTH ); //creeaza directorul si dupa scrie in el
      //cd(hunt_id);

      T_info data = in_data();

      char *path = path_maker(hunt_id, "Treasures");
      int fd = open(path,O_WRONLY | O_CREAT | O_APPEND, 0644);

      if( write(fd, &data, sizeof(T_info)) == -1 ){
        perror("Erroare de scriere\n");
      }
  
      close(fd);


    }
    else if(ask == 'n'){
      printf("Reintrodu comanda cu numele corespunzator sesiunii existente\n");
    }
  }
  else{ //daca exista directorul,ma mut in el si adaug in now file 
    /*
    T_info data = in_data();
    print_treasure_info(data);
    f = fopen(data.Treasure_ID, "w"); //daca nu este imi va crea unul 

    fprintf(f, "Treasure_ID: %s\nUser_Name: %s\nCords: %f,%f\nClue: %s\nValue: %d\n", data.Treasure_ID, data.User_Name, data.GPS.X, data.GPS.Y, data.clue, data.value);

    fclose(f);
    */
    T_info data = in_data();
    char *path = path_maker(hunt_id, "Treasures");
    

    int fd = open(path,O_WRONLY | O_CREAT | O_APPEND, 0644);

    if( write(fd, &data, sizeof(T_info)) == -1 ){
      perror("Erroare de scriere\n");
    }

    close(fd);


  }
  closedir(dr);
}

void list(char *hunt_id){ //List all treasures in the specified hunt. First print the hunt name, the (total) file size and last modification time of its treasure file(s), then list the treasures.
  DIR *dr = opendir(hunt_id);


  closedir(dr);
}

void view(char hunt_id, char *ID){ //View details of a specific treasure

}

void remove_tresure(char *hunt_id, char *ID){ //Remove treasure
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
  printf("Argumen %s\n", argv[1]);
  
  if (!strcmp(argv[1], "add")){
    // printf("Adunare\n, %s" ,argv[2]);
    add(argv[2]);
  }
  else if (!strcmp(argv[1], "list")){
    printf("List\n");
  }
  else if (!strcmp(argv[1], "view")){
    printf("view\n");
  }
  else if (!strcmp(argv[1], "remove_treasure")){
    printf("rm treasure\n");
    
  }
  else if (!strcmp(argv[1], "remove_hunt")){
    printf("rn hunt\n");
  }
  else{
    printf("Operation does not exist\n");
  }
  
  
  return 0;
}
