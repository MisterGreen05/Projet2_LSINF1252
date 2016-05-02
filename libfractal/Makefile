CC=gcc
AR=ar
CFLAGS=-g -Wall -W -I/usr/include/SDL
LDFLAGS=
SRC=fractal.c tools.c
OBJ=$(SRC:.c=.o)
LIBFILE=libfractal.a

all: $(SRC) $(LIBFILE)

$(LIBFILE): $(OBJ)
	$(AR) r $@ $(OBJ)

*.o:
	$(CC) -o $@ -c $< $(CFLAGS)

################################################################################
#Nettoyage des fichiers .o : se deplacer a l'endroit du makefile et lancer     #
# "make clean" via le terminal 	   					       #
################################################################################

.PHONY: clean

clean:
	rm -rf *o

################################################################################
#Nettoyage de l'exec alloc : se deplacer a l'endroit du makefile et lancer     #
# "make mrproper" via le terminal 	   				       #
################################################################################

mrproper: clean
	rm -rf libfractal.a

