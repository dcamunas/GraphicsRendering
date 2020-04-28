void assign_work_zone(int rank, int *start_line, int *end_line, int lines_per_employee, int rest_lines)
{
      *start_line = rank * lines_per_employee;

      rank == (EMPLOYEES_NUMBER - 1) ? (*end_line = (rank + 1) * lines_per_employee + rest_lines) : (*end_line = (rank + 1) * lines_per_employee);
}
