int main(int argc, char *argv[])
{

    /*Variables*/
    int rank, size, numbers_n, finish;
    long double number, min_number;
    long double *data = malloc(DATA_SIZE);
    int *neighbors = malloc(NEIGHBORS_SIZE);
    MPI_Status status;

    /* Initialize MPI program */
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == FIRST_RANK)
    {
        /*Get quantity of numbers*/
        numbers_n = load_data(data);

        finish = check_size(size, numbers_n);

        if (finish != TRUE)
        {
            add_numbers(data, size);
        }
    }

    MPI_Bcast(&finish, 1, MPI_INT, 0, MPI_COMM_WORLD);

    if (finish != TRUE)
    {
        MPI_Recv(&number, 1, MPI_LONG_DOUBLE, 0, MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
        printf("[X] RANK[%d] --> %.2Lf\n", rank, number);

        get_neighbors(rank, neighbors);

        min_number = calculate_min(rank, number, neighbors);

        print_min_number(rank, min_number);
    }

    /* Finalize MPI program */
    MPI_Finalize();

    return EXIT_SUCCESS;
}