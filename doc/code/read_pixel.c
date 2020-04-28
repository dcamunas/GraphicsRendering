void read_pixel(int rank, int start_line, int end_line, char mode, MPI_File image, MPI_Comm parent_comm)
{
      unsigned char pixel[PRIMARY_COLORS_N];
      int i, j;

      for (i = start_line; i < end_line; i++)
      {
            for (j = 0; j < IMAGE_SIDE; j++)
            {
                  MPI_File_read(image, pixel, PRIMARY_COLORS_N, MPI_UNSIGNED_CHAR, MPI_STATUS_IGNORE);
                  put_filter(j, i, pixel, mode, parent_comm);
            }
      }
}
