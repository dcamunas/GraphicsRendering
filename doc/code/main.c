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
            receive_points(commPadre, &status);
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

            assign_employee_lines(rank, &start_line, &end_line, lines_per_employee, rest_lines);

            image = open_file(rank, row_bytes);

            /* Coger modo de filtro */
            mode = argv[argc - 1][0];

            parse_image(rank, start_line, end_line, mode, image, commPadre);

            MPI_File_close(&image);
      }

      MPI_Finalize();

      return EXIT_SUCCESS;
}
