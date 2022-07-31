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
    ND_NULL,
    ND_STRING,
    ND_INTEGER,
    ND_NUMERIC,
    ND_IDENTIFIER,
};

typedef struct Node Node;
struct Node
{
    NodeType type;
    Node *car;
    Node *cdr;

    long double numeric;
    long long integer;
    char *identifier;
    char *string;
};

Node *createNode(NodeType type);
Node *parse(Token *tokenLst);

Node *cons(Node *carNode, Node *cdrNode);
Node *car(Node *pair);
Node *cdr(Node *pair);

void dumpNode(FILE *outputStream, Node *node);

// ================ Formatter ================
Node *format(Node *node);

// ================ Generator ================
typedef enum GeneratorType GeneratorType;
enum GeneratorType
{
    GEN_C,
};

typedef struct Environment Environment;
struct Environment
{
    Environment *enclosing;
    Node **variables;
    unsigned int numVariables;
};

Environment *createEnvironment(size_t numBindings, Environment *enclosing);
void generate(Node *exp, FILE *ostream, GeneratorType type);

// ================ C-Generator ================
void generateC(Node *exp, FILE *ostream);

// ================ Error ================
void setErrorStream(FILE *estream);
void unexpectedTokenError(byte status, Token *token);
void illegalStringError(byte status);
void illegalNodeTypeError(byte status);
void unimplementedFeatureError(byte status);
void unboundVariableError(byte status);

