#include "include/io.h"
#include <stdio.h>
#include <stdlib.h>

char *get_file_contents(const char *filepath)
{
    char *buffer = 0;
    long len;
    FILE *f = fopen(filepath, "rb");

    if (f)
    {
        fseek(f, 0, SEEK_END);
        len = ftell(f);
        fseek(f, 0, SEEK_SET);
        buffer = calloc(len, len);
        if (buffer)
            fread(buffer, 1, len, f);

        fclose(f);
        return buffer;
    }
    printf("\033[31m");
    printf("Error reading file: %s\n", filepath);
    exit(1);
}
