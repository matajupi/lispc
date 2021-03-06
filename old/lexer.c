#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "lispc.h"

#define MAX_CHUNK_SIZE 256
#define MAX_STRING_BUFFER_SIZE 1024

void dumpToken(FILE *stream, Token *token)
{
    char dumpChar;
    switch (token->type) {
        case TK_LEFT_PAREN:
            dumpChar = '(';
            break;
        case TK_RIGHT_PAREN:
            dumpChar = ')';
            break;
        case TK_STRING:
            fprintf(stream, "\"%s\"", token->text);
            return;
        case TK_INTEGER:
            fprintf(stream, "%lld", token->integer);
            return;
        case TK_NUMERIC:
            fprintf(stream, "%Lf", token->numeric);
            return;
        case TK_IDENTIFIER:
            fprintf(stream, "%s", token->identifier);
            return;
        case TK_EOF:
            // fprintf(stream, "EOF");
            return;
    }
    fprintf(stream, "%c", dumpChar);
}

void freeToken(Token *token)
{
    free(token->identifier);
    free(token);
}

static bool isReservedChar(char c)
{
    return strchr("()\";", c) != NULL;
}

static bool isInteger(char *begin, char *end, long long *integer)
{
    int factor = 1;

    if (*begin == '+') {
        begin++;
    }
    else if (*begin == '-') {
        factor = -1;
        begin++;
    }

    if (begin == end) {
        return false;
    }

    for (char *cur = begin; cur != end; cur++) {
        if (isdigit(*cur)) {
            *integer += (*cur - '0') * powl(10, end - cur - 1);
        }
        else {
            return false;
        }
    }
    *integer *= factor;
    return true;
}

static bool isNumeric(char *begin, char *end, long double *numeric)
{
    int factor = 1;

    if (*begin == '+') {
        begin++;
    }
    else if (*begin == '-') {
        factor = -1;
        begin++;
    }

    if (begin == end) {
        return false;
    }

    char *point = NULL;
    for (char *cur = begin; cur != end; cur++) {
        if (*cur == '.') {
            if (point) {
                return false;
            }
            point = cur;
        }
    }
    if (point == NULL) {
        return false;
    }

    for (char *cur = begin; cur != end; cur++) {
        if (*cur == '.') {
            continue;
        }
        else if (isdigit(*cur)) {
            *numeric += (*cur - '0') * powl(10,
                        cur < point ? point - cur - 1 : point - cur);
        }
        else {
            return false;
        }
    }
    *numeric *= factor;
    return true;
}

static Token *tokenizeChunk(char *begin, char *end)
{
    Token *token = calloc(1, sizeof(Token));
    long double numeric = 0.0;
    long long integer = 0;
    if (isNumeric(begin, end, &numeric)) {
        token->type = TK_NUMERIC;
        token->numeric = numeric;
    }
    else if (isInteger(begin, end, &integer)) {
        token->type = TK_INTEGER;
        token->integer = integer;
    }
    else {
        token->type = TK_IDENTIFIER;
        token->identifier = malloc(sizeof(char) * (end - begin + 1));
        strncpy(token->identifier, begin, end - begin);
    }
    return token;
}

static Token *tokenizeString(FILE *inputStream)
{
    char c,
         bufferBegin[MAX_STRING_BUFFER_SIZE],
         *bufferEnd = bufferBegin;
    while ((c = fgetc(inputStream)) != '"') {
        if (c == EOF) {
            unexpectedTokenError(EXIT_FAILURE);
        }
        *(bufferEnd++) = c;
    }
    *(bufferEnd++) = '\0';

    Token *token = calloc(1, sizeof(Token));
    token->type = TK_STRING;
    token->text = malloc(sizeof(char) * (bufferEnd - bufferBegin + 1));
    strcpy(token->text, bufferBegin);
    return token;
}

static Token *tokenizeReservedChar(char c)
{
    Token *token = calloc(1, sizeof(Token));
    switch (c) {
        case '(':
            token->type = TK_LEFT_PAREN;
            break;
        case ')':
            token->type = TK_RIGHT_PAREN;
            break;
    }
    return token;
}

static void skipComment(FILE *inputStream)
{
    char c;
    while ((c = fgetc(inputStream)) != EOF && c != '\n');
}

Token *tokenize(FILE *inputStream)
{
    Token headToken;
    Token *curToken = &headToken;
    Token *token;
    char c;
    char chunkBegin[MAX_CHUNK_SIZE];
    char *chunkEnd  = chunkBegin;

    while ((c = fgetc(inputStream)) != EOF) {
        bool skipCharFlag     = false;
        bool reservedCharFlag = false;
        if ((skipCharFlag = isspace(c))
         || (reservedCharFlag = isReservedChar(c))) {
            if (chunkBegin < chunkEnd) {
                token = tokenizeChunk(chunkBegin, chunkEnd);
                curToken->next = token;
                curToken = token;
                chunkEnd = chunkBegin;
            }

            if (reservedCharFlag) {
                if (c == ';') {
                    skipComment(inputStream);
                    continue;
                }

                if (c == '"') {
                    token = tokenizeString(inputStream);
                }
                else {
                    token = tokenizeReservedChar(c);
                }
                curToken->next = token;
                curToken = token;
            }
        }
        else {
            *(chunkEnd++) = c;
        }
    }
    if (chunkBegin < chunkEnd) {
        token = tokenizeChunk(chunkBegin, chunkEnd);
        curToken->next = token;
        curToken = token;
    }
    token = calloc(1, sizeof(Token));
    token->type = TK_EOF;
    curToken->next = token;

    return headToken.next;
}

