#pragma once

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
    TK_SEMICOLON,
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

