void calculate_file_lines(int *lines_per_employee, int *rest_lines, long *row_bytes)
{
      *lines_per_employee = IMAGE_SIDE / EMPLOYEES_NUMBER;
      *rest_lines = IMAGE_SIDE % EMPLOYEES_NUMBER;
      *row_bytes = *lines_per_employee * IMAGE_SIDE * sizeof(unsigned char) * 3;
}

