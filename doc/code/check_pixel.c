void check_pixel(int *pixel_to_paint)
{
      int i;

      for (i = R; i <= B; i++)
      {
            if (pixel_to_paint[i] < 0){pixel_to_paint[i] = 0;}
            if (pixel_to_paint[i] > 255){pixel_to_paint[i] = 255;}
      }
}
