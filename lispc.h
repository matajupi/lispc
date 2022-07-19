#pragma once

#include <stdio.h>

typedef unsigned char byte;
typedef signed   char sbyte;

// ================ Lexer ================
typedef enum TokenType TokenType;
enum TokenType
{
    TK_LEFT_PAREN,
    TK_RIGHT_PAREN,
    TK_STRING,
    TK_NUMERIC,
    TK_INTEGER,
    TK_IDENTIFIER,
    TK_EOF,
};

typedef struct Token Token;
struct Token
{
    TokenType type;

    long double numeric;
    long long integer;
    char *identifier;
    char *string;

    Token *next;
};

Token *tokenize(FILE *istream);
Token *createToken(TokenType type);
void dumpToken(FILE *outputStream, Token *token);

// ================ Preprocessor ================
Token *preprocess(Token *tokenLst);

// ================ Parser ================
typedef enum NodeType NodeType;
enum NodeType
{
    ND_PAIR,
    ND_PRIMITIVE,
    ND_NULL,
};

typedef struct Node Node;
struct Node
{
    NodeType type;
    Node *car;
    Node *cdr;
    Token *token;
};

Node *parse(Token *tokenLst);
void dumpNode(FILE *outputStream, Node *node);

// ================ Generator ================
typedef enum GeneratorType GeneratorType;
enum GeneratorType
{
    GEN_ARM32,
};

typedef struct Environment Environment;
struct Environment
{

};

void generate(Node *topNode, FILE *ostream, GeneratorType type);

// ================ Error ================
void setErrorStream(FILE *estream);
void unexpectedTokenError(byte status, Token *token);
void illegalStringError(byte status);

