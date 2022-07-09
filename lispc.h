#pragma once

typedef unsigned char byte;
typedef signed char sbyte;

typedef enum GeneratorType GeneratorType;
enum GeneratorType
{
    ARM32,
};

typedef enum TokenType TokenType;
enum TokenType
{
    LeftParen,
    RightParen,
    Quote,
    DoubleQuote,
    Integer,
    Numeric,
    Identifier,
};

typedef struct Token Token;
struct Token
{
    TokenType type;
    const char *text;
    long long integer;
    long double numeric;
    Token *next;
};

Token *tokenize(FILE *);

