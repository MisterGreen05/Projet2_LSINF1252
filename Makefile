CC = gcc
CFLAGS = -g -Wall
LIB = -lpthread -lSDL -L/usr/include/SDL -L$(HOME)/local/lib
INCLUDE = -Ilibfractal/ -I$(HOME)/local/include -I$(HOME)/local/lib
SRC = main.c
TARGET = main

all: $(TARGET)

$(TARGET): main.o libfractal.a
	$(CC) $(CFLAGS) -o $(TARGET) main.o libfractal/libfractal.a $(LIB)

libfractal.a:
	make -C libfractal/

main.o: main.c
	$(CC) $(CFLAGS) -c main.c $(INCLUDE)

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
	rm -rf main
