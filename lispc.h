#pragma once

#include <stdio.h>

typedef unsigned char byte;
typedef signed char sbyte;

typedef enum TokenType TokenType;
enum TokenType
{
    TK_LEFT_PAREN,
    TK_RIGHT_PAREN,
    TK_STRING,
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
    char *text;
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
    ND_PRIMITIVE,
    ND_NULL,
};

typedef struct Node Node;
struct Node
{
    NodeType type;
    Node *carNode;
    Node *cdrNode;
    Node *next;

    Token *token;
};

void dumpNode(FILE *, Node *);
void unexpectedTokenError(byte);
Node *parse(Token *);

typedef enum GeneratorType GeneratorType;
enum GeneratorType
{
    GEN_ARM32,
};

void generate(Node *, FILE *, GeneratorType);

