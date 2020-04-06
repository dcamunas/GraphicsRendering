DIROBJ := obj/
DIREXE := exec/
DIRHEA := include/
DIRSRC := src/
DIRFIL := files/

CFLAGS := -I $(DIRHEA)
LDLIBS := -lX11
CC := mpicc
RUN := mpirun

all : dirs graphicsRendering

dirs:
	mkdir -p $(DIREXE)

graphicsRendering:
	$(CC) $(DIRSRC)GraphicsRendering.c $(CFLAGS) $(LDLIBS) -o $(DIREXE)GraphicsRendering

test:
	$(RUN) -n 2 ./$(DIREXE)GraphicsRendering

solution:
	$(RUN) ./$(DIREXE)GraphicsRendering 

clean : 
	rm -rf *~ core $(DIROBJ) $(DIREXE) $(DIRHEA)*~ $(DIRSRC)*~ 
