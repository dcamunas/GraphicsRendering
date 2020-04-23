/*Print the minium value by First Rank (Rank == 0)*/
void print_min_number(const int rank, long double min_number)
{
    if (rank == FIRST_RANK)
    {
        printf("\n[X] RANK[%d]: The minium value is: %.2Lf\n", rank, min_number);

        /*End message*/
        printf("----------------------------------------------------------------------------------------\n");
        printf("\t\t\t***** PROGRAM FINALIZED *****\n\n\n");
    }
}