#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "lispc.h"

#define DEBUG 1

void usage(byte status)
{
    fprintf(stderr, "Usage: ./lispc <file>\n");
    exit(status);
}

char *toOutputFilename(const char *inputFilename)
{
    const char *extPlace = strrchr(inputFilename, '.');
    size_t filenameLength = extPlace - inputFilename;
    char *outputFilename = malloc(sizeof(char) * filenameLength + 3);
    strncpy(outputFilename, inputFilename, filenameLength);
    strcpy(outputFilename + filenameLength, ".s");
    return outputFilename;
}

int main(int argc, char **argv)
{
    FILE *inputStream, *outputStream;
#if DEBUG
    inputStream  = stdin;
    outputStream = stdout;
#else
    if (argc < 2) {
        usage(EXIT_FAILURE);
    }
    const char *inputFilename = argv[1];
    inputStream = fopen(inputFilename, "r");
    if (inputStream == NULL) {
        fprintf(stderr, "Error: Cannot open \"%s\"\n", inputFilename);
        exit(EXIT_FAILURE);
    }
    const char *outputFilename = toOutputFilename(inputFilename);
    outputStream = fopen(outputFilename, "w");
#endif
    fclose(inputStream);
    fclose(outputStream);
    return EXIT_SUCCESS;
}

