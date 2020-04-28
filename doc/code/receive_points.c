void receive_points(MPI_Comm parent_comm, MPI_Status *status)
{
      int point_to_paint[POINT_INFO_N];
      int i;

      for (i = 0; i < IMAGE_SIZE; i++)
      {
            MPI_Recv(&point_to_paint, POINT_INFO_N, MPI_INT, MPI_ANY_SOURCE, TAG, parent_comm, status);
            dibujaPunto(point_to_paint[ROW], point_to_paint[COLUMN], point_to_paint[R], point_to_paint[G], point_to_paint[B]);
      }
}
