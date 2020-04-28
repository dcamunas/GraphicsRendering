/* Pract2  RAP 09/10    Javier Ayllon*/

#include <openmpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <assert.h>
#include <unistd.h>

#include <definitions.h>
#define NIL (0)

void print_title();
void initX();
void dibujaPunto(int x, int y, int r, int g, int b);
void calculate_file_lines(int *lines_per_employee, int *rest_lines, long *row_bytes);
void assign_work_zone(int rank, int *start_line, int *end_line, int lines_per_employee, int rest_lines);
MPI_File open_file(int rank, long row_bytes);
void read_pixel(int rank, int start_line, int end_line, char mode, MPI_File image, MPI_Comm parent_comm);
void apply_filter(int row, int column, unsigned char *pixel, char mode, MPI_Comm parent_comm);
void check_pixel(int *pixel_to_paint);
void receive_pixels(MPI_Comm parent_comm, MPI_Status *status);
void print_final_info(double init);


/*Variables Globales */
XColor colorX;
Colormap mapacolor;
char cadenaColor[] = "#000000";
Display *dpy;
Window w;
GC gc;

/* Programa principal */
int main(int argc, char *argv[])
{

      int rank, size;
      MPI_Comm commPadre;
      MPI_Status status;
      int error_codes[EMPLOYEES_NUMBER];
      double init = MPI_Wtime();

      MPI_Init(&argc, &argv);
      MPI_Comm_rank(MPI_COMM_WORLD, &rank);
      MPI_Comm_size(MPI_COMM_WORLD, &size);
      MPI_Comm_get_parent(&commPadre);

      if ((commPadre == MPI_COMM_NULL) && (rank == MASTER_RANK))
      {
            /* Codigo del maestro */
            print_title();
            initX();
            MPI_Comm_spawn(EXEC_PATH, argv, EMPLOYEES_NUMBER, MPI_INFO_NULL, MASTER_RANK, MPI_COMM_WORLD, &commPadre, error_codes);

            printf("[MAESTRO] :: Dibujando imagen...\n");
            receive_pixels(commPadre, &status);
            print_final_info(init);
      }

      else
      {
            /* Codigo de todos los trabajadores */
            int lines_per_employee, rest_lines, start_line, end_line;
            long row_bytes;
            char mode;
            MPI_File image;

            calculate_file_lines(&lines_per_employee, &rest_lines, &row_bytes);

            assign_work_zone(rank, &start_line, &end_line, lines_per_employee, rest_lines);

            image = open_file(rank, row_bytes);

            /* Coger modo de filtro */
            mode = argv[argc - 1][0];

            read_pixel(rank, start_line, end_line, mode, image, commPadre);

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

/* Recibir pixel a pintar */
void receive_pixels(MPI_Comm parent_comm, MPI_Status *status)
{
      int pixel_to_paint[PIXEL_INFO_N];
      int i;

      for (i = 0; i < IMAGE_SIZE; i++)
      {
            MPI_Recv(&pixel_to_paint, PIXEL_INFO_N, MPI_INT, MPI_ANY_SOURCE, TAG, parent_comm, status);
            dibujaPunto(pixel_to_paint[ROW], pixel_to_paint[COLUMN], pixel_to_paint[R], pixel_to_paint[G], pixel_to_paint[B]);
      }
}

/* Calcular líneas por trabajador */
void calculate_file_lines(int *lines_per_employee, int *rest_lines, long *row_bytes)
{
      *lines_per_employee = IMAGE_SIDE / EMPLOYEES_NUMBER;
      *rest_lines = IMAGE_SIDE % EMPLOYEES_NUMBER;
      *row_bytes = *lines_per_employee * IMAGE_SIDE * sizeof(unsigned char) * 3;
}

/* Asignar zona de trabajo de cada trabajador */
void assign_work_zone(int rank, int *start_line, int *end_line, int lines_per_employee, int rest_lines)
{
      *start_line = rank * lines_per_employee;

      rank == (EMPLOYEES_NUMBER - 1) ? (*end_line = (rank + 1) * lines_per_employee + rest_lines) : (*end_line = (rank + 1) * lines_per_employee);
}

/* Apertura del archivo, devolvera el descriptor del mismo */
MPI_File open_file(int rank, long row_bytes)
{
      MPI_File image;
      MPI_File_open(MPI_COMM_WORLD, IMAGE_PATH, MPI_MODE_RDONLY, MPI_INFO_NULL, &image);
      MPI_File_set_view(image, rank * row_bytes, MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR, NATIVE_MOD, MPI_INFO_NULL);

      return image;
}


/* Lectura del pixel de la imagen */
void read_pixel(int rank, int start_line, int end_line, char mode, MPI_File image, MPI_Comm parent_comm)
{
      unsigned char pixel[PRIMARY_COLORS_N];
      int i, j;

      for (i = start_line; i < end_line; i++)
      {
            for (j = 0; j < IMAGE_SIDE; j++)
            {
                  MPI_File_read(image, pixel, PRIMARY_COLORS_N, MPI_UNSIGNED_CHAR, MPI_STATUS_IGNORE);
                  apply_filter(j, i, pixel, mode, parent_comm);
            }
      }
}

/* Aplicar filtro al pixel */
void apply_filter(int row, int column, unsigned char *pixel, char mode, MPI_Comm parent_comm)
{
      int pixel_to_paint[PIXEL_INFO_N];

      pixel_to_paint[ROW] = row;
      pixel_to_paint[COLUMN] = column;

      switch (mode)
      {
      case SEPIA:
            pixel_to_paint[R] = (int)(pixel[R] * 0.393) + (int)(pixel[G] * 0.769) + (int)(pixel[B] * 0.189);
            pixel_to_paint[G] = (int)(pixel[R] * 0.349) + (int)(pixel[G] * 0.686) + (int)(pixel[B] * 0.168);
            pixel_to_paint[B] = (int)(pixel[R] * 0.272) + (int)(pixel[G] * 0.534) + (int)(pixel[B] * 0.131);
            break;

      case BLACK_WHITE:
            pixel_to_paint[R] = (int)(pixel[R] * 0.2986) + (int)(pixel[G] * 0.587) + (int)(pixel[B] * 0.114);
            pixel_to_paint[G] = (int)(pixel[R] * 0.2986) + (int)(pixel[G] * 0.587) + (int)(pixel[B] * 0.114);
            pixel_to_paint[B] = (int)(pixel[R] * 0.2986) + (int)(pixel[G] * 0.587) + (int)(pixel[B] * 0.114);
            break;

      case NEGATIVE:
            pixel_to_paint[R] = 255 - (int)pixel[R];
            pixel_to_paint[G] = 255 - (int)pixel[G];
            pixel_to_paint[B] = 255 - (int)pixel[B];
            break;

      default:
            pixel_to_paint[R] = (int)pixel[R];
            pixel_to_paint[G] = (int)pixel[G];
            pixel_to_paint[B] = (int)pixel[B];
            break;
      }

      check_pixel(pixel_to_paint);
      MPI_Send(&pixel_to_paint, PIXEL_INFO_N, MPI_INT, MASTER_RANK, TAG, parent_comm);
}

/* Comprobar pixel a enviar */
void check_pixel(int *pixel_to_paint)
{
      int i;

      for (i = R; i <= B; i++)
      {
            if (pixel_to_paint[i] < 0)
            {
                  pixel_to_paint[i] = 0;
            }
            if (pixel_to_paint[i] > 255)
            {
                  pixel_to_paint[i] = 255;
            }
      }
}

/* Imprimir titulor */ 
void print_title()
{
      printf("-------------------------------------------------------------------------\n");
      printf("\t\t********  RENDERIZADOR GRAFICO  ********\n\n");
      printf("[MAESTRO] :: Lanzando [%d] TRABAJADORES...\n", EMPLOYEES_NUMBER);
}

/* Imprimir informacion final */
void print_final_info(double init)
{
      printf("[MAESTRO] :: El tiempo de renderizado grafico ha sido de: %0.2f segundos.\n", MPI_Wtime() - init);
      printf("[MAESTRO] :: Mostrando imagen...\n\n");
      sleep(5);
      printf("-------------------------------------------------------------------------\n");
      printf("\t\t********  PROGRAMA FINALIZADO  ********\n\n");
}