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

void illegalNodeTypeError(byte status)
{
    fprintf(errorStream, "Error: Illegal node type.\n");
    exit(status);
}

void unimplementedFeatureError(byte status)
{
    fprintf(errorStream, "Error: This feature has not been implemented yet.\n");
    exit(status);
}

void unboundVariableError(byte status)
{
    fprintf(errorStream, "Error: Unbound variable.\n");
    exit(status);
}

