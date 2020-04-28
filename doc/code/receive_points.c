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
