DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/
DIRFIL := files/

CFLAGS := -I $(DIRHEA)
LDLIBS := -lX11
CC := mpicc
RUN := mpirun

all : dirs main

dirs:
	mkdir -p $(DIREXE)

main:
	$(CC) $(DIRSRC)pract2.c $(CFLAGS) $(LDLIBS) -o $(DIREXE)pract2

solution:
	$(RUN) -np 1 ./$(DIREXE)pract2

solution_sepia:
	$(RUN) -np 1 ./$(DIREXE)pract2 Sepia

solution_blackwhite:
	$(RUN) -np 1 ./$(DIREXE)pract2 BlackWhite

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~ 
