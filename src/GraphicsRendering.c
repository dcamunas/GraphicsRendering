/* Pract2  RAP 09/10    Javier Ayllon*/

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <assert.h>
#include <unistd.h>
#define NIL (0)

/*Variables Globales */

XColor colorX;
Colormap mapacolor;
char cadenaColor[] = "#000000";
Display *dpy;
Window w;
GC gc;

/*Funciones auxiliares */

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

/* Programa principal */

int main(int argc, char *argv[])
{

  int rank, size;
  MPI_Comm commPadre;
  int tag;
  MPI_Status status;
  int buf[5];

  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_get_parent(&commPadre);
  if ((commPadre == MPI_COMM_NULL) && (rank == 0))
  {

    initX();

    /* Codigo del maestro */

    /*En algun momento dibujamos puntos en la ventana algo como
	dibujaPunto(x,y,r,g,b);  */
  }

  else
  {
    /* Codigo de todos los trabajadores */
    /* El archivo sobre el que debemos trabajar es foto.dat */
  }

  MPI_Finalize();
}
