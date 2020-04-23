void send_numbers(long double *data, const int size)
{
    int i;
    long double number;

    for (i = 0; i < size; i++)
    {
        number = data[i];
        MPI_Send(&number, 1, MPI_LONG_DOUBLE, i, SEND_TAG, MPI_COMM_WORLD);
    }

    free(data);
}
