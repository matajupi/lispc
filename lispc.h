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
    TK_DOT,
    TK_INTEGER,
    TK_NUMERIC,
    TK_IDENTIFIER,
    TK_EOF,
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
void freeToken(Token *);

typedef enum NodeType NodeType;
enum NodeType
{
    ND_TOP_LEVEL,
    ND_PAIR,
    ND_INTEGER,
    ND_NUMERIC,
    ND_IDENTIFIER,
    ND_SYMBOL,
    ND_STRING,
    ND_TRUE,
    ND_FALSE,
    ND_NULL,
};

typedef struct Node Node;
struct Node
{
    NodeType type;
    Node *carNode;
    Node *cdrNode;
    Node *next;

    long long integer;
    long double numeric;
    char *identifier;
    char *text;
};

Node *parse(Token *);

