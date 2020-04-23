/* Pract2  RAP 09/10    Javier Ayllon*/

#include <openmpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <assert.h>
#include <unistd.h>

#include <definitions.h>

#define NIL (0)

/*Variables Globales */

XColor colorX;
Colormap mapacolor;
char cadenaColor[] = "#000000";
Display *dpy;
Window w;
GC gc;

int err_codes[CHILDS_NUMBER];

/* Declaracion de funciones */
void dibujaPunto(int x, int y, int r, int g, int b);
void initX();
void receive_points();

/* Programa principal */

int main(int argc, char *argv[])
{

      int rank, size, lines_per_employee, ;
      MPI_Comm commPadre;
      int tag;
      MPI_Status status;
      int point_to_paint[5];

      MPI_Init(&argc, &argv);
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
      MPI_Comm_size(MPI_COMM_WORLD, &size);
      MPI_Comm_get_parent(&commPadre);

      if ((commPadre == MPI_COMM_NULL) && (rank == 0))
      {
            /* Codigo del maestro */
            initX();

            printf("[MAESTRO]: Lanzando [%d] TRABAJADORES...\n", CHILDS_NUMBER);
            MPI_Comm_spawn(EXEC_PATH, MPI_ARGV_NULL, CHILDS_NUMBER, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &commPadre, err_codes);

            printf("[MAESTRO]: Dibujando imagen...\n");
            receive_points(commPadre);
      }

      else
      {
            /* Codigo de trabajadores */

      }

      MPI_Finalize();
}

void initX()
{

      dpy = XOpenDisplay(NIL);
      assert(dpy);

      int blackColor = BlackPixel(dpy, DefaultScreen(dpy));
      int whiteColor = WhitePixel(dpy, DefaultScreen(dpy));

      w = XCreateSimpleWindow(dpy, DefaultRootWindow(dpy), 0, 0,
                              400, 400, 0, blackColor, blackColor);
      XSelectInput(dpy, w, StructureNotifyMask);
      XMapWindow(dpy, w);
      gc = XCreateGC(dpy, w, 0, NIL);
      XSetForeground(dpy, gc, whiteColor);
      for (;;)
      {
            XEvent e;
            XNextEvent(dpy, &e);
            if (e.type == MapNotify)
                  break;
      }

      mapacolor = DefaultColormap(dpy, 0);
}

void dibujaPunto(int x, int y, int r, int g, int b)
{

      sprintf(cadenaColor, "#%.2X%.2X%.2X", r, g, b);
      XParseColor(dpy, mapacolor, cadenaColor, &colorX);
      XAllocColor(dpy, mapacolor, &colorX);
      XSetForeground(dpy, gc, colorX.pixel);
      XDrawPoint(dpy, w, gc, x, y);
      XFlush(dpy);
}

void receive_points(MPI_Comm comm_parent)
{
      int point_to_paint[5];
      unsigned int i;

      for (i = 0; i < IMAGE_SIZE; i++)
      {
            /*Se obtiene el punto a pintar*/
            MPI_Recv(&point_to_paint, 5, MPI_INT, 0, MPI_ANY_TAG, comm_parent, MPI_STATUS_IGNORE);
            dibujaPunto(point_to_paint[0], point_to_paint[1], point_to_paint[2], point_to_paint[3], point_to_paint[4]);
      }
}