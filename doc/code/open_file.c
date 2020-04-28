MPI_File open_file(int rank, long row_bytes)
{
      MPI_File image;
      MPI_File_open(MPI_COMM_WORLD, IMAGE_PATH, MPI_MODE_RDONLY, MPI_INFO_NULL, &image);
      MPI_File_set_view(image, rank * row_bytes, MPI_UNSIGNED_CHAR, MPI_UNSIGNED_CHAR, NATIVE_MOD, MPI_INFO_NULL);

      return image;
}
