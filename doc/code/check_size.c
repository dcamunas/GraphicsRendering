/* Check toroid's size (number of nodes) */
int check_size(const int size, const int numbers_n)
{
    int finish = FALSE;
    if (size != numbers_n)
    {
        fprintf(stderr, "[X] RANK[0]: Error, quantity of numbers (%d) is diferent at toroid's size (%d | L = %d)\n",
                numbers_n, size, L);
        finish = TRUE;
    }

    return finish;
}
