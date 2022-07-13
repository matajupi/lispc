#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "lispc.h"

#define MAX_CHUNK_SIZE 256

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
        case TK_QUOTE:
            dumpChar = '\'';
            break;
        case TK_DOUBLE_QUOTE:
            dumpChar = '"';
            break;
        case TK_INTEGER:
            printf("%lld\n", token->integer);
            return;
        case TK_NUMERIC:
            printf("%Lf\n", token->numeric);
            return;
        case TK_IDENTIFIER:
            printf("%s\n", token->identifier);
            return;
        case TK_EOF:
            printf("EOF\n");
            return;
    }
    fprintf(stream, "%c\n", dumpChar);
}

void freeToken(Token *token)
{
    free(token->identifier);
    free(token);
}

static bool isReservedChar(char c)
{
    return strchr("()'\";", c) != NULL;
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
    for (char *cur = begin; *cur != '\0'; cur++) {
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
    long long integer;
    long double numeric;
    if (isInteger(begin, end, &integer)) {
        token->type = TK_INTEGER;
        token->integer = integer;
    }
    else if (isNumeric(begin, end, &numeric)) {
        token->type = TK_NUMERIC;
        token->numeric = numeric;
    }
    else {
        token->type = TK_IDENTIFIER;
        token->identifier = malloc(end - begin + 1);
        strncpy(token->identifier, begin, end - begin);
    }
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
        case '\'':
            token->type = TK_QUOTE;
            break;
        case '"':
            token->type = TK_DOUBLE_QUOTE;
            break;
    }
    return token;
}

static void removeEmptyTokens(Token *head)
{
    Token *prev = head;
    Token *cur  = head->next;
    while (cur != NULL) {
        if (cur->type == TK_IDENTIFIER
         && strlen(cur->identifier) == 0) {
            prev->next = cur->next;
            freeToken(cur);
            cur = prev->next;
        }
        else {
            prev = cur;
            cur = prev->next;
        }
    }
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
    char c,
         chunkFirst[MAX_CHUNK_SIZE],
         *chunkLast  = chunkFirst;

    while ((c = fgetc(inputStream)) != EOF) {
        bool skipCharFlag     = false,
             reservedCharFlag = false;
        if ((skipCharFlag = isspace(c))
         || (reservedCharFlag = isReservedChar(c))) {
            Token *token = tokenizeChunk(chunkFirst, chunkLast);
            curToken->next = token;
            curToken = token;
            chunkLast = chunkFirst;

            if (reservedCharFlag) {
                if (c == ';') {
                    skipComment(inputStream);
                }
                else {
                    token = tokenizeReservedChar(c);
                    curToken->next = token;
                    curToken = token;
                }
            }
        }
        else {
            *(chunkLast++) = c;
        }
    }
    Token *token = tokenizeChunk(chunkFirst, chunkLast);
    curToken->next = token;
    curToken = token;
    token = calloc(1, sizeof(Token));
    token->type = TK_EOF;
    curToken->next = token;

    removeEmptyTokens(&headToken);
    return headToken.next;
}

