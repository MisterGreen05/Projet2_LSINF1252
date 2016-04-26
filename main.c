/*Autheur : AMAN SINGH
 *Version : 26/04/16
 */


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#include "libfractal/fractal.h"

typedef enum {false, true} bool; /*definition du type bool*/

int main(int argc, char* argv[]){

  /* PREMIERE PARTIE : Cette partie du programme permet de gerer le */
  /* format d'entrée sachant que -d ou --maxthreads sont d'offices les */
  /* premiers arguments placés par l'utilisateur en ligne de commande */
  /* (ils peuvent etre optionels) et le dernier est d'office le fichierOUT*/

    char arg1[] = "--maxthreads"; /*deux tableaux de caracteres pour pouvoir gerer differents cas de format d'entree via strcmp*/
    char arg2[] = "-d";

    int numberfiles = 0; /*initialisation du nombre de fichiers*/
    int numberFilesIN = 0;
    size_t maxsizefilename = 64; /*Taille max du nom du fichier*/
    char **array; /*Tableau contenant les noms de fichiers d'entrée et sortie*/
    unsigned int maxthreads = 1; /*Par defaut maxthreads vaut 1 si pas specifie*/
    bool optionD = false; /*Par defaut l'option d est desactive (vaut 0)*/

    int i = 1; /*permet de parcourir argv et gerer les differents cas*/

      /*Premier cas de la forme : ./main --maxthreads X -d fichier1 .. fichierout*/

      if(strcmp(argv[1],arg1) == 0 && strcmp(argv[3],arg2) == 0){
        maxthreads = (unsigned int)atoi(argv[2]);
        optionD = true;
        i = 4;
        numberfiles = argc-i;
        numberFilesIN = numberfiles-1;
        array = malloc(numberfiles * sizeof(char *));
        int j=0;
          while(j <= numberfiles){
            array[j] = (char *)malloc(maxsizefilename);
            array[j] = argv[i];
            //debug : printf("%s\n", array[j]);
            j++;
            i++;
          }
        }

      /*Deuxième cas de la forme : ./main -d --maxthreads X fichier1 .. fichierout*/

      else if(strcmp(argv[1],arg2) == 0 && strcmp(argv[2],arg1) == 0){
        maxthreads = (unsigned int)atoi(argv[3]);
        optionD = true;
        i = 4;
        numberfiles = argc-i;
        numberFilesIN = numberfiles-1;
        array = malloc(numberfiles * sizeof(char *));
        int j=0;
          while(j <= numberfiles){
            array[j] = (char *)malloc(maxsizefilename);
            array[j] = argv[i];
            //debug : printf("%s\n", array[j]);
            j++;
            i++;
          }
        }

      /*Troisième cas de la forme : ./main --maxthreads X fichier1 .. fichierout*/

      else if(strcmp(argv[1],arg1) == 0 && strcmp(argv[3],arg2) !=0){
        maxthreads = (unsigned int)atoi(argv[2]);
        i = 3;
        numberfiles = argc-i;
        numberFilesIN = numberfiles-1;
        array = malloc(numberfiles * sizeof(char *));
        int j=0;
          while(j <= numberfiles){
            array[j] = (char *)malloc(maxsizefilename);
            array[j] = argv[i];
            //debug : printf("%s\n", array[j]);
            j++;
            i++;
          }
        }

      /*Quatrième cas de la forme : ./main -d fichier1 .. fichierout*/

      else if(strcmp(argv[1],arg2) == 0 && strcmp(argv[2],arg1) !=0){
        optionD = true;
        i = 2;
        numberfiles = argc-i;
        numberFilesIN = numberfiles-1;
        array = malloc(numberfiles * sizeof(char *));
        int j=0;
          while(j <= numberfiles){
            array[j] = (char *)malloc(maxsizefilename);
            array[j] = argv[i];
            //debug : printf("%s\n", array[j]);
            j++;
            i++;
          }
        }

      /*Cinquième cas de la forme : ./main fichier1 .. fichierout*/

      else if(i == 1){
        numberfiles = argc-i;
        numberFilesIN = numberfiles-1;
        array = malloc(numberfiles * sizeof(char *));
        int j=0;
          while(j <= numberfiles){
            array[j] = (char *)malloc(maxsizefilename);
            array[j] = argv[i];
            //debug : printf("%s\n", array[j]);
            j++;
            i++;
          }
      }

      /*Sixième cas en cas d'erreur*/

      else{
        fprintf(stderr, "%s\n", strerror(errno));
      }

      /*for debug :
      printf("L'option d vaut %u (0 desactive et 1 active)\n", optionD);
      printf("Le nombre de thread vaut %d\n", maxthreads);
      printf("%d\n", numberFilesIN);
      for(int i = 0 ; i<numberfiles ; i++){
        printf("%s\n", array[i]);
      }*/



    /* Deuxième partie du programme : modèle basé sur les producteurs/consommateurs */
    /* 1 - charger les descriptions des fractales depuis une source, que ce soit
           l’entreée standard ou un fichier via un thread de lecture(consommateur) */
    /* 2 - calculer la fractal, 1 thread de calcul par fractal (producteur) */

    /*compter le nombre de lignes totales de tous les fichiers in*/

      FILE* fichier[numberFilesIN];
      int ch;
      int numberlines = 0;

      for (int i = 0 ; i<numberFilesIN ; i++){
          fichier[i] = fopen(array[i],"r") ;

          if(fichier[i] != NULL){
            while(ch != EOF){
              ch = fgetc(fichier[i]);
              if(ch == '\n'){
                numberlines++;
              }
            }
            numberlines++;
            ch = 0;
            fclose(fichier[i]);
          }
      }

      char *lines[numberlines];
      char *eof;
      char line[192];
      int j = 0;
      for (int i = 0 ; i<numberFilesIN ; i++){
        fichier[i] = fopen(array[i],"r") ;
        if(fichier[i] != NULL){
          while((eof = fgets(line, 192, fichier[i])) != NULL){
            lines[j] = strdup(eof);
            j++;
          }
          fclose(fichier[i]);
        }
      }
      /*for debug :
      for(int i = 0 ; i<numberlines ; i++){
        printf("%s\n", lines[i]);
      }*/
      for(int i = 0 ; i<numberlines ; i++){
        if((lines[i])[0] == '\n' || (lines[i])[0] == '#'){
          //ignore
        }

      }



}
