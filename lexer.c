#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "lispc.h"

#define CHUNK_BUFFER_SIZE 1024

static char reservedChars[] = "()\";";
static FILE *inputStream;

// ================ Token list ================
static Token *curToken;

static void appendToken(Token *token)
{
    curToken->next = token;
    curToken = token;
}

// ================ Chunk buffer ================
static char chunkBuffer[CHUNK_BUFFER_SIZE];
static char *chunkEnd = chunkBuffer; // TODO: ?

static void appendCharToChunkBuffer(char c)
{
    *(chunkEnd++) = c;
}

static void resetChunkBuffer()
{
    chunkEnd = chunkBuffer;
}

// ================ Lexer ================
Token *createToken(TokenType type)
{
    Token *token = calloc(1, sizeof(Token));
    token->type = type;
    return token;
}

static Token *createNumeric(long double numeric)
{
    Token *token = createToken(TK_NUMERIC);
    token->numeric = numeric;
    return token;
}

static Token *createInteger(long long integer)
{
    Token *token = createToken(TK_INTEGER);
    token->integer = integer;
    return token;
}

static Token *createIdentifier()
{
    Token *token = createToken(TK_IDENTIFIER);
    token->identifier = malloc(sizeof(char)
                               * (chunkEnd - chunkBuffer + 1));
    strncpy(token->identifier, chunkBuffer, chunkEnd - chunkBuffer);
    return token;
}

static bool isSkipChar(char c)
{
    return isspace(c);
}

static bool isReservedChar(char c)
{
    return strchr(reservedChars, c) != NULL || c == EOF;
}

static bool isNumeric(long double *numeric)
{
    char *begin = chunkBuffer;

    int factor = 1;
    if (*begin == '+') {
        begin++;
    }
    else if (*begin == '-') {
        factor = -1;
        begin++;
    }

    if (begin == chunkEnd) {
        return false;
    }

    char *point = NULL;
    for (char *cur = begin; cur != chunkEnd; cur++) {
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

    for (char *cur = begin; cur != chunkEnd; cur++) {
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

static bool isInteger(long long *integer)
{
    char *begin = chunkBuffer;

    int factor = 1;
    if (*begin == '+') {
        begin++;
    }
    else if (*begin == '-') {
        factor = -1;
        begin++;
    }

    if (begin == chunkEnd) {
        return false;
    }

    for (char *cur = begin; cur != chunkEnd; cur++) {
        if (isdigit(*cur)) {
            *integer += (*cur - '0') * powl(10, chunkEnd - cur - 1);
        }
        else {
            return false;
        }
    }
    *integer *= factor;
    return true;
}

static void skipComment()
{
    char c;
    while ((c = fgetc(inputStream)) != EOF && c != '\n');
}

static void tokenizeString()
{
    char c;
    while ((c = fgetc(inputStream)) != '"') {
        if (c == EOF) {
            illegalStringError(EXIT_FAILURE);
        }
        appendCharToChunkBuffer(c);
    }

    Token *token = createToken(TK_STRING);
    token->string = malloc(sizeof(char) * (chunkEnd - chunkBuffer + 1));
    strncpy(token->string, chunkBuffer, chunkEnd - chunkBuffer);
    appendToken(token);
    resetChunkBuffer();
}

static void tokenizeChunk()
{
    if (chunkBuffer == chunkEnd) {
        return;
    }

    Token *token;
    long double numeric = 0;
    long long integer = 0.0;
    if (isNumeric(&numeric)) {
        token = createNumeric(numeric);
    }
    else if (isInteger(&integer)) {
        token = createInteger(integer);
    }
    else {
        token = createIdentifier();
    }
    appendToken(token);
    resetChunkBuffer();
}

static void tokenizeReservedChar(char c)
{
    if (c == ';') {
        skipComment();
    }
    else if (c == '"') {
        tokenizeString();
    }
    else if (c == EOF) {
        Token *token = createToken(TK_EOF);
        appendToken(token);
    }
    else {
        TokenType type = (TokenType)(strchr(reservedChars, c)
                                     - reservedChars);
        Token *token = createToken(type);
        appendToken(token);
    }
}

Token *tokenize(FILE *istream)
{
    inputStream = istream;

    // Initialize token list
    Token headToken;
    curToken = &headToken;

    while (true) {
        char c = fgetc(inputStream);

        if (isSkipChar(c)) {
            tokenizeChunk();
        }
        else if (isReservedChar(c)) {
            tokenizeChunk();
            tokenizeReservedChar(c);
        }
        else {
            appendCharToChunkBuffer(c);
        }

        if (c == EOF) {
            break;
        }
    }
    return headToken.next;
}

// ================ Debug ================
void dumpToken(FILE *outputStream, Token *token)
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
            fprintf(outputStream, "\"%s\"", token->string);
            return;
        case TK_NUMERIC:
            fprintf(outputStream, "%Lf", token->numeric);
            return;
        case TK_INTEGER:
            fprintf(outputStream, "%lld", token->integer);
            return;
        case TK_IDENTIFIER:
            fprintf(outputStream, "%s", token->identifier);
            return;
        case TK_EOF:
            return;
    }
    fprintf(outputStream, "%c", dumpChar);
}

