#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "lispc.h"

#define DEBUG 1
#define MAX_OUTPUT_FILENAME_SIZE 64

void usage(byte status)
{
    fprintf(stderr, "Usage: ./lispc <file>\n");
    exit(status);
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
    char *inputFilename = argv[1];
    inputStream = fopen(inputFilename, "r");
    if (inputStream == NULL) {
        fprintf(stderr, "Error: Cannot open \"%s\"\n", inputFilename);
        exit(EXIT_FAILURE);
    }
    char outputFilename[MAX_OUTPUT_FILENAME_SIZE];
    char *ep = strrchr(inputFilename, '.');
    size_t filenameLength = ep == NULL ? strlen(inputFilename) : ep - inputFilename;
    strncpy(outputFilename, inputFilename, filenameLength);
    strcpy(outputFilename + filenameLength, ".s");
    outputStream = fopen(outputFilename, "w");
#endif
    Token *tokens = tokenize(inputStream);
    // Node *node = parse(tokens);

for (Token *cur = tokens; cur; cur = cur->next) {
    dumpToken(stdout, cur);
}

    fclose(inputStream);
    fclose(outputStream);
    return EXIT_SUCCESS;
}

