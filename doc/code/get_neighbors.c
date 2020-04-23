void get_neighbors(const int rank, int *neighbors)
{
    int row = rank / L;
    int column = rank % L;


    switch (row)
    {
    case 0:
        neighbors[NORTH] = L * (L - 1) + rank;
        neighbors[SOUTH] = rank + L;
        break;

    case L - 1:
        neighbors[NORTH] = rank - L;
        neighbors[SOUTH] = rank % L;
        break;

    default:
        neighbors[NORTH] = rank - L;
        neighbors[SOUTH] = rank + L;
        break;
    }

    switch (column)
    {
    case 0:
        neighbors[WEST] = rank + (L - 1);
        neighbors[EAST] = rank + 1;
        break;

    case L - 1:
        neighbors[WEST] = rank - 1;
        neighbors[EAST] = rank - (L - 1);
        break;

    default:
        neighbors[WEST] = rank - 1;
        neighbors[EAST] = rank + 1;
        break;
    }
}