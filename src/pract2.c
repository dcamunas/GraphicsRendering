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

unsigned int parse_mode(char character);
void initX();
void dibujaPunto(int x, int y, int r, int g, int b);
void calculate_file_lines(int *lines_per_employee, int *rest_lines, long *row_bytes);
void assign_employee_lines(int rank, int *start_line, int *end_line, int lines_per_employee, int rest_lines);
MPI_File open_file(int rank, long row_bytes);
void parse_image(int rank, int start_line, int end_line, char mode, MPI_File image, MPI_Comm parent_comm);
void put_filter(int row, int column, unsigned char *pixel, char mode, MPI_Comm parent_comm);
void check_point(int *point_to_paint);
void receive_points(MPI_Comm parent_comm);

/* Programa principal */
int main(int argc, char *argv[])
{

      int rank, size, tag;
      MPI_Comm commPadre;
      MPI_Status status;
      MPI_File image;
      int error_codes[EMPLOYEES_NUMBER];
      double finish, init = MPI_Wtime();

      MPI_Init(&argc, &argv);
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
      MPI_Comm_size(MPI_COMM_WORLD, &size);
      MPI_Comm_get_parent(&commPadre);

      if ((commPadre == MPI_COMM_NULL) && (rank == 0))
      {
            /* Codigo del maestro */
            initX();
            printf("[MAESTRO]: Lanzando [%d] TRABAJADORES...\n", EMPLOYEES_NUMBER);
            MPI_Comm_spawn(EXEC_PATH, argv, EMPLOYEES_NUMBER, MPI_INFO_NULL, MASTER_RANK, MPI_COMM_WORLD, &commPadre, error_codes);

            printf("[MAESTRO]: Dibujando imagen...\n");
            receive_points(commPadre);
            finish = MPI_Wtime();
            printf("Tiempo de ejecucion: %f\n", finish - init);
      }

      else
      {
            /* Codigo de todos los trabajadores */
            /* Variables locales */
            int lines_per_employee, rest_lines, start_line, end_line;
            long row_bytes;
            char mode;
            MPI_File image;

            calculate_file_lines(&lines_per_employee, &rest_lines, &row_bytes);

            assign_employee_lines(rank, &start_line, &end_line, lines_per_employee, rest_lines);
            //printf("TRABAJADOR [%d]: START: %d | END: %d\n", rank, start_line, end_line);

            image = open_file(rank, row_bytes);

            /* Coger modo de filtro */
            mode = argv[argc - 1][0];

            parse_image(rank, start_line, end_line, mode, image, commPadre);
            MPI_File_close(&image);
      }
      MPI_Finalize();

      return EXIT_SUCCESS;
}

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

void calculate_file_lines(int *lines_per_employee, int *rest_lines, long *row_bytes)
{
      /* Calcular lineas por empleado */
      *lines_per_employee = IMAGE_SIDE / EMPLOYEES_NUMBER;
      *rest_lines = IMAGE_SIDE % EMPLOYEES_NUMBER;
      *row_bytes = *lines_per_employee * IMAGE_SIDE * sizeof(unsigned char) * 3;
}

void assign_employee_lines(int rank, int *start_line, int *end_line, int lines_per_employee, int rest_lines)
{
      *start_line = rank * lines_per_employee;

      rank == (EMPLOYEES_NUMBER - 1) ? (*end_line = (rank + 1) * lines_per_employee + rest_lines) : (*end_line = (rank + 1) * lines_per_employee);
}

MPI_File open_file(int rank, long row_bytes)
{
      MPI_File image;
      MPI_File_open(MPI_COMM_WORLD, IMAGE_PATH, MPI_MODE_RDONLY, MPI_INFO_NULL, &image);
      MPI_File_set_view(image, rank * row_bytes, MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR, NATIVE_MOD, MPI_INFO_NULL);

      return image;
}

void parse_image(int rank, int start_line, int end_line, char mode, MPI_File image, MPI_Comm parent_comm)
{
      unsigned char pixel[PRIMARY_COLORS_N];
      int i, j;

      for (i = start_line; i < end_line; i++)
      {
            for (j = 0; j < IMAGE_SIDE; j++)
            {
                  MPI_File_read(image, pixel, PRIMARY_COLORS_N, MPI_UNSIGNED_CHAR, NULL);
                  put_filter(j, i, pixel, mode, parent_comm);
            }
      }
}

void put_filter(int row, int column, unsigned char *pixel, char mode, MPI_Comm parent_comm)
{
      int point_to_paint[POINT_INFO_N];

      point_to_paint[ROW] = row;
      point_to_paint[COLUMN] = column;

      switch (mode)
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
            point_to_paint[R] = pixel[R];
            point_to_paint[G] = pixel[G];
            point_to_paint[B] = pixel[B];
            break;
      }

      check_point(point_to_paint);
      MPI_Send(&point_to_paint, POINT_INFO_N, MPI_INT, MASTER_RANK, 1, parent_comm);
}

void check_point(int *point_to_paint)
{
      int i;

      for (i = R; i <= B; i++)
      {
            if (point_to_paint[i] < 0)
            {
                  point_to_paint[i] = 0;
            }
            if (point_to_paint[i] > 255)
            {
                  point_to_paint[i] = 255;
            }
      }
}

void receive_points(MPI_Comm parent_comm)
{
      int point_to_paint[POINT_INFO_N];
      int i;

      for (i = 0; i < IMAGE_SIZE; i++)
      {
            MPI_Recv(&point_to_paint, POINT_INFO_N, MPI_INT, MPI_ANY_SOURCE, 1, parent_comm, NULL);
            dibujaPunto(point_to_paint[ROW], point_to_paint[COLUMN], point_to_paint[R], point_to_paint[G], point_to_paint[B]);
      }
}