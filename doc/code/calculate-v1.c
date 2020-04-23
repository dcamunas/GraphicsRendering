/*Calculate the minium value*/
long double calculate_min(const int rank, long double my_number, int *neighbors)
{
    int i;
    long double his_number;

    for (i = 1; i < L; i++)
    {
        /* Calculate rows */
        MPI_Send(&my_number, 1, MPI_LONG_DOUBLE, neighbors[SOUTH], SEND_TAG, MPI_COMM_WORLD);
        MPI_Recv(&his_number, 1, MPI_LONG_DOUBLE, neighbors[NORTH], MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
        my_number = (his_number > my_number ? my_number : his_number);
    }
    
    for (i = 1; i < L; i++){
        /* Calculate columns */
        MPI_Send(&my_number, 1, MPI_LONG_DOUBLE, neighbors[EAST], SEND_TAG, MPI_COMM_WORLD);
        MPI_Recv(&his_number, 1, MPI_LONG_DOUBLE, neighbors[WEST], MPI_ANY_TAG, MPI_COMM_WORLD, NULL);
        my_number = (his_number > my_number ? my_number : his_number);
    }

    free(neighbors);

    return my_number;
}

