DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/
DIRFIL := files/

CFLAGS := -I $(DIRHEA)
LDLIBS := -lX11
CC := mpicc
RUN := mpirun
EMPLONUM := -DEMPLOYEES_NUMBER=$$employees

all : dirs compile

dirs:
	mkdir -p $(DIREXE)

compile:
	@read -p "[X] Introduzca la cantidad de trabajadores: " employees;\
	$(CC) $(DIRSRC)pract2.c $(CFLAGS) $(LDLIBS) $(EMPLONUM) -o $(DIREXE)pract2

solution:
	$(RUN) -np 1 ./$(DIREXE)pract2

solution_sepia:
	$(RUN) -np 1 ./$(DIREXE)pract2 Sepia

solution_blackwhite:
	$(RUN) -np 1 ./$(DIREXE)pract2 BlackWhite

solution_negative:
	$(RUN) -np 1 ./$(DIREXE)pract2 Negative

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~ 
