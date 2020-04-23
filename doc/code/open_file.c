/* Open file */
FILE *open_file(const char *path, const char *mode)
{
    FILE *file;
    if ((file = fopen(path, mode)) == NULL)
    {
        fprintf(stderr, "[X] RANK[0]: Error opening file.\n");
        exit(EXIT_FAILURE);
    }
    return file;
}
