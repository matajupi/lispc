#pragma once

#include <stdio.h>

typedef unsigned char byte;
typedef signed char sbyte;

typedef enum GeneratorType GeneratorType;
enum GeneratorType
{
    GEN_ARM32,
};

typedef enum TokenType TokenType;
enum TokenType
{
    TK_LEFT_PAREN,
    TK_RIGHT_PAREN,
    TK_QUOTE,
    TK_DOUBLE_QUOTE,
    TK_INTEGER,
    TK_NUMERIC,
    TK_IDENTIFIER,
};

typedef struct Token Token;
struct Token
{
    TokenType type;
    char *identifier;
    long long integer;
    long double numeric;
    Token *next;
};

void dumpToken(FILE *, Token *);
Token *tokenize(FILE *);
void freeToken(Token *token);

typedef enum NodeType NodeType;
enum NodeType
{
    ND_PAIR,
    ND_INTEGER,
    ND_NUMERIC,
    ND_SYMBOL,
    ND_STRING,
    ND_NULL,
    ND_TRUE,
    ND_FALSE,
};

typedef struct Node Node;
struct Node
{
    NodeType type;
    Node *carNode;
    Node *cdrNode;
};

