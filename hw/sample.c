void load_file(const char *filename)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    lines = malloc(MAX_LINES * sizeof(char *));
    if (!lines)
    {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    total_lines = 0;

    while ((read = getline(&line, &len, file)) != -1)
    {
        if (total_lines < MAX_LINES)
        {
            lines[total_lines] = strdup(line);
            // Calculate the number of display lines this single line will take up
            int line_length = strlen(line);
            int wrapped_lines = (line_length + cols - 1) / cols; // How many lines needed on display
            total_lines += wrapped_lines; // Increment total_lines by the number of wrapped lines
        }
        else
        {
            break;
        }
    }

    free(line);
    fclose(file);
}
