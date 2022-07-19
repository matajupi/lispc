#include <stdio.h>
#include <stdlib.h>

#include "lispc.h"

static FILE *errorStream;

void setErrorStream(FILE *estream)
{
    errorStream = estream;
}

void unexpectedTokenError(byte status, Token *token)
{
    fprintf(errorStream, "Error: Unexpected token \"");
    dumpToken(errorStream, token);
    fprintf(errorStream, "\".\n");
    exit(status);
}

void illegalStringError(byte status)
{
    fprintf(errorStream, "Error: Illegal string.\n");
    exit(status);
}

