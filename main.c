#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

static unsigned int maxthreads = 1;
typedef enum {false, true} bool;

typedef struct{
  struct fractal** buf;
  sem_t full;
  sem_t empty;
  int nextin;
  int nextout;
  sem_t pmut;
  sem_t cmut;
} buffer_t;

static buffer_t bstruct;

void error(int err, char*msg){
  fprintf(stderr, "%s a retourné %d, message d'erreur : ", msg, err, strerror(errno));
  exit(EXIT_FAILURE);
}

void init(unsigned int bsize){
  int ret;

  ret = sem_init(&bstruct.full, 0, 0);
    if(ret!=0)
      error(ret,"sem_init");

  ret = sem_init(&bstruct.empty, 0, maxthreads);
    if(ret!=0)
      error(ret,"sem_init");

  ret = sem_init(&bstruct.pmut, 0, 1);
    if(ret!=0)
      error(ret,"sem_init");

  ret = sem_init(&bstruct.cmut, 0, 1);
    if(ret!=0)
      error(ret,"sem_init");

  bstruct.nextin = bstruct.nextout = 0;
  bstruct.buf = malloc(bsize*(sizeof(struct fractal*)));
}

void destroy (void){
  int err;

  err = sem_destroy(&bstruct.empty);
    if(err!=0)
      error(err,"sem_destroy");

  err = sem_destroy(&bstruct.full);
    if(err!=0)
      error(err,"sem_destroy");

  err = sem_destroy(&bstruct.cmut);
    if(err!=0)
      error(err,"sem_destroy");

  err =sem_destroy(&bstruct.pmut);
    if(err!=0)
      error(err,"sem_destroy");

}

void* produce(void* arg){
  FILE * file = (FILE*)arg;
  char *eof;
  char line[512];

  if(file != NULL){
    while((eof = fgets(line, 192, file)) != NULL){
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
          struct fractal* f;
          f = fractal_new(name, width, height, a, b);

          sem_wait(&bstruct.empty);
          sem_wait(&bstruct.pmut);

          bstruct.buf[bstruct.nextin] = f;
          bstruct.nextin++;
          bstruct.nextin %= maxthreads;

          sem_post(&bstruct.pmut);
          sem_post(&bstruct.full);
        }
    }
  }
  fclose(file);
  return(NULL);
}

void* consume(void* arg){
  struct fractal *f;
  sem_wait(&bstruct.full);
  sem_wait(&bstruct.cmut);

  f=bstruct.buf[bstruct.nextout];

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

  bstruct.nextout++;
  bstruct.nextout %= maxthreads;

  sem_post(&bstruct.cmut);
  sem_post(&bstruct.empty);
  return(NULL);
}


int main(int argc, char* argv[]){
  char arg1[] = "--maxthreads";
  char arg2[] = "-d";
  char arg3[] = "-";
  int argvIndex = 1;
  int optionD = false;
  char** inputNames;
  int fileToRead = 0;
  char* fractalFromCommandLine;
  char* outputName;

  if(strcmp(argv[1],arg1) == 0 && strcmp(argv[3],arg2) == 0){
    maxthreads = (unsigned int)atoi(argv[2]);
    optionD = true;
    outputName = malloc(512*sizeof(char));
    outputName = argv[argc-1];

    argvIndex = 4;
    inputNames = malloc((argc-argvIndex)*sizeof(char*));

    for(argvIndex ; argvIndex < argc-1 ; argvIndex++){
      if(strcmp(argv[argvIndex],arg3) != 0){
        inputNames[fileToRead] = (char *)malloc(512*sizeof(char));
        inputNames[fileToRead] = argv[argvIndex];
        fileToRead++;
      }
      else{
        printf("Vous avez choisi l'option - donc rentrez les données de la fractal : \n");
        fractalFromCommandLine = malloc(512*sizeof(char));
        scanf("%s", fractalFromCommandLine);
      }
    }
  }

  else if(strcmp(argv[1],arg2) == 0 && strcmp(argv[2],arg1) == 0){
    maxthreads = (unsigned int)atoi(argv[3]);
    optionD = true;
    outputName = malloc(512*sizeof(char));
    outputName = argv[argc-1];

    argvIndex = 4;
    inputNames = malloc((argc-argvIndex)*sizeof(char*));

    for(argvIndex ; argvIndex < argc-1 ; argvIndex++){
      if(strcmp(argv[argvIndex],arg3) != 0){
        inputNames[fileToRead] = (char *)malloc(512*sizeof(char));
        inputNames[fileToRead] = argv[argvIndex];
        fileToRead++;
      }
      else{
        printf("Vous avez choisi l'option - donc rentrez les données de la fractal : \n");
        fractalFromCommandLine = malloc(512*sizeof(char));
        scanf("%s", fractalFromCommandLine);
      }
    }
  }

  else if(strcmp(argv[1],arg1) == 0 && strcmp(argv[3],arg2) !=0){
    maxthreads = (unsigned int)atoi(argv[2]);
    optionD = false;
    outputName = malloc(512*sizeof(char));
    outputName = argv[argc-1];

    argvIndex = 3;
    inputNames = malloc((argc-argvIndex)*sizeof(char*));

    for(argvIndex ; argvIndex < argc-1 ; argvIndex++){
      if(strcmp(argv[argvIndex],arg3) != 0){
        inputNames[fileToRead] = (char *)malloc(512*sizeof(char));
        inputNames[fileToRead] = argv[argvIndex];
        fileToRead++;
      }
      else{
        printf("Vous avez choisi l'option - donc rentrez les données de la fractal : \n");
        fractalFromCommandLine = malloc(512*sizeof(char));
        scanf("%s", fractalFromCommandLine);
      }
    }
  }

  else if(strcmp(argv[1],arg2) == 0 && strcmp(argv[2],arg1) !=0){
    optionD = true;
    outputName = malloc(512*sizeof(char));
    outputName = argv[argc-1];

    argvIndex = 2;
    inputNames = malloc((argc-argvIndex)*sizeof(char*));

    for(argvIndex ; argvIndex < argc-1 ; argvIndex++){
      if(strcmp(argv[argvIndex],arg3) != 0){
        inputNames[fileToRead] = (char *)malloc(512*sizeof(char));
        inputNames[fileToRead] = argv[argvIndex];
        fileToRead++;
      }
      else{
        printf("Vous avez choisi l'option - donc rentrez les données de la fractal : \n");
        fractalFromCommandLine = malloc(512*sizeof(char));
        scanf("%s", fractalFromCommandLine);
      }
    }
  }

  else if(argvIndex == 1){
    optionD = false;
    outputName = malloc(512*sizeof(char));
    outputName = argv[argc-1];

    inputNames = malloc((argc-argvIndex)*sizeof(char*));

    for(argvIndex ; argvIndex < argc-1 ; argvIndex++){
      if(strcmp(argv[argvIndex],arg3) != 0){
        inputNames[fileToRead] = (char *)malloc(512*sizeof(char));
        inputNames[fileToRead] = argv[argvIndex];
        fileToRead++;
      }
      else{
        printf("Vous avez choisi l'option - donc rentrez les données de la fractal : \n");
        fractalFromCommandLine = malloc(512*sizeof(char));
        scanf("%s", fractalFromCommandLine);
      }
    }
  }

  else{
    int err;
    error(err,"L'ensemble d'instructions pour vérifier le format d'entrée ");
    error(err,"sem_destroy");
  }

  /*Partie thread*/

  pthread_t threadLecture[fileToRead];
  pthread_t threadCalcul[maxthreads];

  init(maxthreads);

  int err;

  for(int i = 0 ; i < fileToRead ; i++){
    err = pthread_create(&(threadLecture[i]), NULL, produce,(void*)fopen(inputNames[i],"r"));
    if(err!=0)
      error(err,"pthread_create");
  }

  for(int i = 0 ; i < maxthreads ; i++){
  err = pthread_create(&(threadCalcul[i]), NULL, consume, NULL);
  if(err!=0)
    error(err,"pthread_create");
  }

  for(int i=0 ; i < fileToRead ; i++) {
    void *r;
    err=pthread_join(threadLecture[i],(void **)&r);
    free(r);
    if(err!=0)
      error(err,"pthread_join");
  }

  for(int i=0 ; i < maxthreads ; i++) {
    void *r;
    err=pthread_join(threadCalcul[i],(void **)&r);
    free(r);
    if(err!=0)
      error(err,"pthread_join");
  }

  destroy();

  return(EXIT_SUCCESS);
}
