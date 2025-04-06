#include <stdio.h>
#include <stdlib.h>
#include <string.h>


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

//functiile de implementat

void add(char *hunt_id){ //Add a new treasure to the specified hunt (game session). Each hunt is stored in a separate directory.
  

}

void list(char *hunt_id){ //List all treasures in the specified hunt. First print the hunt name, the (total) file size and last modification time of its treasure file(s), then list the treasures.

}

void view(char hunt_id, char *ID){ //View details of a specific treasure

}

void remove_tresure(char *hunt_id, char *ID){ //Remove treasure

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
    printf("Adunare\n");
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
