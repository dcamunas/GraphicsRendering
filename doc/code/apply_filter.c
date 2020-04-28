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
