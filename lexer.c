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
    }
    fprintf(stream, "%c\n", dumpChar);
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

Token *tokenize(FILE *inputStream)
{
    Token head;
    Token *curToken = &head;
    char chunk[MAX_CHUNK_SIZE];
    while (fscanf(inputStream, "%s", chunk) > 0) {
printf("%s\n", chunk);
        for (char *beg = chunk, *cur = chunk; ; cur++) {
            if (strchr("()'\"", *cur) || *cur == '\0') {
                Token *token = calloc(1, sizeof(Token));
                if (beg == cur) {
                    goto BEGIN_SWITCH;
                }
                long long integer;
                if (isInteger(beg, cur, &integer)) {
                    token->type = TK_INTEGER;
                    token->integer = integer;
                    goto END_IF;
                }
                long double numeric;
                if (isNumeric(beg, cur, &numeric)) {
                    token->type = TK_NUMERIC;
                    token->numeric = numeric;
                    goto END_IF;
                }
                token->type = TK_IDENTIFIER;
                char *identifier = calloc(MAX_CHUNK_SIZE, sizeof(char));
                strncpy(identifier, beg, cur - beg);
                identifier[cur - beg + 1] = '\0';
                token->identifier = identifier;
            END_IF:
                curToken->next = token;
                curToken = token;
                if (*cur == '\0') {
                    break;
                }

                token = calloc(1, sizeof(Token));
            BEGIN_SWITCH:
                switch (*cur) {
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
                curToken->next = token;
                curToken = token;
                beg = cur + 1;
            }
        }
    }
    return head.next;
}

