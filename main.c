#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#include "libfractal/fractal.h"

typedef enum {false, true} bool;

typedef struct{
  struct fractal** buf;
  sem_t full;
  sem_t empty;
  unsigned int maxthreads;
  int optionD;
  int numberfiles;
  FILE* fileToBeRead;
  char** inputNames;
  char* fractalFromCommandLine;
  int IDconsumer;
  int ID;
  char* outputName;
  int nextin;
  int nextout;
  sem_t pmut;
  sem_t cmut;
} buffer_t;

void error(int err, char*msg){
  fprintf(stderr, "%s a retourné %d, message d'erreur : ", msg, err, strerror(errno));
  exit(EXIT_FAILURE);
}

void* producer(void* arg){
  buffer_t *buffer = (buffer_t*)arg;
  char *eof;
  char line[512];
  for(buffer->ID ; buffer->ID < buffer->numberfiles ; buffer->ID ++){
    buffer->fileToBeRead = fopen(buffer->inputNames[buffer->ID], "r");
  if(buffer->fileToBeRead != NULL){
    while((eof = fgets(line, 192, buffer->fileToBeRead)) != NULL){
        if ((strdup(eof))[0] == '#' || (strdup(eof))[0] == '\n') {}

        else if (strlen(strdup(eof)) < 1) {}

        else {
          char* fra[5];
          char* p;
          p = strtok(strdup(eof), " ");
          int k = 0;
          while (p != NULL){
            fra[k] = p;
            k++;
            p = strtok (NULL, " ");
          }

          char const *name = fra[0];
          int width = atoi(fra[1]);
          int height = atoi(fra[2]);
          double a;
          double b;
          sscanf(fra[3], "%lf", &a);
          sscanf(fra[4], "%lf", &b);
          struct fractal* f =  fractal_new(name, width, height, a, b);

          sem_wait(&buffer->empty);
          sem_wait(&buffer->pmut);

          buffer->buf[buffer->nextin] = f;
          //printf("ID : %d a creer : %s\n", buffer->ID, fractal_get_name(buffer->buf[buffer->nextin]) );
          buffer->nextin++;
          buffer->nextin %= buffer->maxthreads;

          sem_post(&buffer->pmut);
          sem_post(&buffer->full);
        }
      }
  }
  fclose(buffer->fileToBeRead);
  }
  pthread_exit(NULL);
}

void* consumer(void* arg){
  buffer_t *buffer = (buffer_t*)arg;
  sem_wait(&buffer->full);
  sem_wait(&buffer->cmut);

struct fractal *f = buffer->buf[buffer->nextout];
//printf("Consommateur %d a consomme %s\n", buffer->IDconsumer, fractal_get_name(f));
int values [(fractal_get_width(f)+1)*(fractal_get_height(f)+1)];
int l = 0;
int x = 0;
int y = 0;
while(l < (fractal_get_width(f)+1)*(fractal_get_height(f)+1) ){
  if(y <= fractal_get_height(f)){
    values[l] = fractal_compute_value(f,x,y);
    l++;
    y++;
  }
  else{
    x++;
    y=0;
  }
}
if(write_bitmap_sdl(f, fractal_get_name(f)) == -1)
  printf("FAIL to generate BMP for the fractal");

  buffer->nextout++;
  buffer->nextout %= buffer->maxthreads;

  sem_post(&buffer->cmut);
  sem_post(&buffer->empty);

pthread_exit(NULL);
}

int main(int argc, char* argv[]){

  buffer_t* bstruct = (buffer_t*)malloc(sizeof(buffer_t));

  bstruct->maxthreads = 1;
  bstruct->optionD = false;
  bstruct->numberfiles = 0;

  char arg1[] = "--maxthreads";
  char arg2[] = "-d";
  char arg3[] = "-";

  int argvIndex = 1;

  if(strcmp(argv[1],arg1) == 0 && strcmp(argv[3],arg2) == 0){
    bstruct->maxthreads = (unsigned int)atoi(argv[2]);
    bstruct->optionD = true;
    bstruct->outputName = (char*)malloc(512*sizeof(char));
    bstruct->outputName = argv[argc-1];

    bstruct->inputNames = (char**)malloc((argc-argvIndex)*sizeof(char*));

    for(argvIndex = 4 ; argvIndex < argc-1 ; argvIndex++){
      if(strcmp(argv[argvIndex],arg3) != 0){
        bstruct->inputNames[bstruct->numberfiles] = (char *)malloc(512*sizeof(char));
        bstruct->inputNames[bstruct->numberfiles] = argv[argvIndex];
        bstruct->numberfiles++;
      }
      else{
        printf("Vous avez choisi l'option - donc rentrez les données de la fractal : \n");
        bstruct->fractalFromCommandLine = (char*)malloc(512*sizeof(char));
        scanf("%s", bstruct->fractalFromCommandLine);
      }
    }
  }

  else if(strcmp(argv[1],arg2) == 0 && strcmp(argv[2],arg1) == 0){
    bstruct->maxthreads = (unsigned int)atoi(argv[3]);
    bstruct->optionD = true;
    bstruct->outputName = (char*)malloc(512*sizeof(char));
    bstruct->outputName = argv[argc-1];

    bstruct->inputNames = (char**)malloc((argc-argvIndex)*sizeof(char*));

    for(argvIndex = 4; argvIndex < argc-1 ; argvIndex++){
      if(strcmp(argv[argvIndex],arg3) != 0){
        bstruct->inputNames[bstruct->numberfiles] = (char *)malloc(512*sizeof(char));
        bstruct->inputNames[bstruct->numberfiles] = argv[argvIndex];
        bstruct->numberfiles++;
      }
      else{
        printf("Vous avez choisi l'option - donc rentrez les données de la fractal : \n");
        bstruct->fractalFromCommandLine = (char*)malloc(512*sizeof(char));
        scanf("%s", bstruct->fractalFromCommandLine);
      }
    }
  }

  else if(strcmp(argv[1],arg1) == 0 && strcmp(argv[3],arg2) !=0){
    bstruct->maxthreads = (unsigned int)atoi(argv[2]);
    bstruct->optionD = false;
    bstruct->outputName = (char*)malloc(512*sizeof(char));
    bstruct->outputName = argv[argc-1];

    bstruct->inputNames = (char**)malloc((argc-argvIndex)*sizeof(char*));

    for(argvIndex = 3 ; argvIndex < argc-1 ; argvIndex++){
      if(strcmp(argv[argvIndex],arg3) != 0){
        bstruct->inputNames[bstruct->numberfiles] = (char *)malloc(512*sizeof(char));
        bstruct->inputNames[bstruct->numberfiles] = argv[argvIndex];
        bstruct->numberfiles++;
      }
      else{
        printf("Vous avez choisi l'option - donc rentrez les données de la fractal : \n");
        bstruct->fractalFromCommandLine = (char*)malloc(512*sizeof(char));
        scanf("%s", bstruct->fractalFromCommandLine);
      }
    }
  }

  else if(strcmp(argv[1],arg2) == 0 && strcmp(argv[2],arg1) !=0){
    bstruct->optionD = true;
    bstruct->outputName = (char*)malloc(512*sizeof(char));
    bstruct->outputName = argv[argc-1];

    bstruct->inputNames = (char**)malloc((argc-argvIndex)*sizeof(char*));

    for(argvIndex = 2 ; argvIndex < argc-1 ; argvIndex++){
      if(strcmp(argv[argvIndex],arg3) != 0){
        bstruct->inputNames[bstruct->numberfiles] = (char *)malloc(512*sizeof(char));
        bstruct->inputNames[bstruct->numberfiles] = argv[argvIndex];
        bstruct->numberfiles++;
      }
      else{
        printf("Vous avez choisi l'option - donc rentrez les données de la fractal : \n");
        bstruct->fractalFromCommandLine = (char*)malloc(512*sizeof(char));
        scanf("%s", bstruct->fractalFromCommandLine);
      }
    }
  }

  else if(argvIndex == 1){
    bstruct->optionD = false;
    bstruct->outputName = (char*)malloc(512*sizeof(char));
    bstruct->outputName = argv[argc-1];

    bstruct->inputNames = (char**)malloc((argc-argvIndex)*sizeof(char*));

    for(argvIndex = 1 ; argvIndex < argc-1 ; argvIndex++){
      if(strcmp(argv[argvIndex],arg3) != 0){
        bstruct->inputNames[bstruct->numberfiles] = (char *)malloc(512*sizeof(char));
        bstruct->inputNames[bstruct->numberfiles] = argv[argvIndex];
        bstruct->numberfiles++;
      }
      else{
        printf("Vous avez choisi l'option - donc rentrez les données de la fractal : \n");
        bstruct->fractalFromCommandLine = (char*)malloc(512*sizeof(char));
        scanf("%s", bstruct->fractalFromCommandLine);
      }
    }
  }

  else{
    int err = -1;
    printf("Erreur formate d'entrée : %d\n", err);
  }

  /* THREAD PART */

  pthread_t threadLecture;
  pthread_t threadCalcul[bstruct->maxthreads];

  int ret;

  ret = sem_init(&bstruct->full, 0, 0);
    if(ret!=0)
      error(ret,"sem_init");

  ret = sem_init(&bstruct->empty, 0, bstruct->maxthreads);
    if(ret!=0)
      error(ret,"sem_init");

  ret = sem_init(&bstruct->pmut, 0, 1);
    if(ret!=0)
      error(ret,"sem_init");

  ret = sem_init(&bstruct->cmut, 0, 1);
    if(ret!=0)
      error(ret,"sem_init");

  bstruct->buf = (struct fractal**)malloc(bstruct->maxthreads * sizeof(struct fractal*));

  bstruct->nextin = 0;
  bstruct->nextout = 0;

  int err;

  err = pthread_create(&threadLecture, NULL, producer, (void*)bstruct);
    if(err!=0)
      error(err,"pthread_create");

  for(bstruct->IDconsumer = 0 ; bstruct->IDconsumer < bstruct->maxthreads ; bstruct->IDconsumer++){
    err = pthread_create(&threadCalcul[bstruct->IDconsumer], NULL, consumer, (void*)bstruct);
      if(err!=0)
        error(err,"pthread_create");
  }

  ret = pthread_join(threadLecture, NULL);

  for(int i = 0 ; i < bstruct->maxthreads ; i++){
    ret = pthread_join(threadCalcul[i], NULL);
  }

  return 0;
}
