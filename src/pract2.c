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

int err_codes[EMPLOYEES_NUMBER];

/* Declaracion de funciones */
void dibujaPunto(int x, int y, int r, int g, int b);
void initX();
void assign_employee_lines(int rank, int *start_line, int *end_line, int lines_per_employee, int rest_lines);
void parse_image(int rank, int start_line, int end_line, long row_bytes, MPI_Comm comm_parent, MPI_File image, char *mode);
void put_filter(int row, int column, unsigned char *pixel, MPI_Comm comm_parent, char *mode);
void receive_points();

/* Programa principal */

int main(int argc, char *argv[])
{

      int rank, size, lines_per_employee, rest_lines;
      long row_bytes;
      char *mode;
      MPI_Comm commPadre;
      int tag;
      MPI_Status status;
      int point_to_paint[POINT_INFO_N];

      MPI_Init(&argc, &argv);
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
      MPI_Comm_size(MPI_COMM_WORLD, &size);
      MPI_Comm_get_parent(&commPadre);

      /* Calcular lineas por empleado */
      lines_per_employee = IMAGE_SIDE / EMPLOYEES_NUMBER;
      rest_lines = IMAGE_SIDE % EMPLOYEES_NUMBER;
      /*Calcular bytes por fila (usados para posicionar al trabajador)*/
      row_bytes = lines_per_employee * IMAGE_SIDE * sizeof(unsigned char) * 3; // X3 --> RGB

      if ((commPadre == MPI_COMM_NULL) && (rank == 0))
      {
            /* Codigo del maestro */
            initX();

            printf("[MAESTRO]: Lanzando [%d] TRABAJADORES...\n", EMPLOYEES_NUMBER);
            MPI_Comm_spawn(EXEC_PATH, MPI_ARGV_NULL, EMPLOYEES_NUMBER, MPI_INFO_NULL, 0, MPI_COMM_WORLD, &commPadre, err_codes);

            printf("[MAESTRO]: Dibujando imagen...\n");
            receive_points(commPadre);
      }

      else
      {
            /* Codigo de trabajadores */
            int start_line, end_line;
            assign_employee_lines(rank, &start_line, &end_line, lines_per_employee, rest_lines);
            //printf("SOY EL TRABAJADOR [%d]: START=%d | END=%d\n", rank, start_line, end_line);

            MPI_File image;
            MPI_File_open(MPI_COMM_WORLD, IMAGE_PATH, MPI_MODE_RDONLY, MPI_INFO_NULL, &image);

            /* Para posicionar al trabajador dentro del archivo */
            MPI_File_set_view(image, rank * row_bytes, MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR, "native", MPI_INFO_NULL);

            parse_image(rank, start_line, end_line, row_bytes, commPadre, image, argv[argc - 1]);
            MPI_File_close(&image);
      }

      MPI_Finalize();
      return EXIT_SUCCESS;
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
      int point_to_paint[POINT_INFO_N];
      unsigned int i;

      for (i = 0; i < IMAGE_SIZE; i++)
      {
            /*Se obtiene el punto a pintar*/
            MPI_Recv(&point_to_paint, POINT_INFO_N, MPI_INT, 0, MPI_ANY_TAG, comm_parent, MPI_STATUS_IGNORE);
            dibujaPunto(point_to_paint[0], point_to_paint[1], point_to_paint[2], point_to_paint[3], point_to_paint[4]);
      }
}

void assign_employee_lines(int rank, int *start_line, int *end_line, int lines_per_employee, int rest_lines)
{
      *start_line = rank * lines_per_employee;

      if (rank != EMPLOYEES_NUMBER - 1)
      {
            *end_line = (rank + 1) * lines_per_employee;
      }
      else
      {
            *end_line = (rank + 1) * lines_per_employee + rest_lines;
      }
}

void parse_image(int rank, int start_line, int end_line, long row_bytes, MPI_Comm comm_parent, MPI_File image, char *mode)
{
      int i, j;
      unsigned char pixel[PRIMARY_COLORS_N];

      for (i = start_line; i <= end_line; i++)
      {
            for (j = 0; j < IMAGE_SIDE; j++)
            {
                  MPI_File_read(image, pixel, PRIMARY_COLORS_N, MPI_UNSIGNED_CHAR, MPI_STATUS_IGNORE);

                  put_filter(j, i, pixel, comm_parent, mode);
            }
      }
}

void put_filter(int row, int column, unsigned char *pixel, MPI_Comm comm_parent, char *mode)
{
      int point_to_paint[POINT_INFO_N];
      point_to_paint[ROW] = row;
      point_to_paint[COLUMN] = column;

      switch (*mode)
      {
      case SEPIA:
            point_to_paint[R] = (int)(pixel[R] * 0.393) + (int)(pixel[G] * 0.769) + (int)(pixel[B] * 0.189);
            point_to_paint[G] = (int)(pixel[R] * 0.349) + (int)(pixel[G] * 0.686) + (int)(pixel[B] * 0.168);
            point_to_paint[B] = (int)(pixel[R] * 0.272) + (int)(pixel[G] * 0.534) + (int)(pixel[B] * 0.131);
            break;

      case BLACK_WHITE:
            point_to_paint[R] = (int)(pixel[R] * 0.2986) + (int)(pixel[G] * 0.587) + (int)(pixel[B] * 0.114);
            point_to_paint[G] = (int)(pixel[R] * 0.2986) + (int)(pixel[G] * 0.587) + (int)(pixel[B] * 0.114);
            point_to_paint[B] = (int)(pixel[R] * 0.2986) + (int)(pixel[G] * 0.587) + (int)(pixel[B] * 0.114);
            break;

      default:
            point_to_paint[R] = (int)pixel[R];
            point_to_paint[G] = (int)pixel[G];
            point_to_paint[B] = (int)pixel[B];
            break;
      }

      int i;
      for (i = R; i < B; i++)
      {
            if (point_to_paint[i] > 255)
            {
                  point_to_paint[i] = 255;
            }
            else if (point_to_paint[i] < 0)
            {
                  point_to_paint[i] = 0;
            }
      }

      MPI_Send(&point_to_paint, 5, MPI_INT, 0, 1, comm_parent);
}
