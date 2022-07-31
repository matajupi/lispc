#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#include "lispc.h"

// ================ Context ================
static Token *curToken;

static Token *currentToken()
{
    return curToken;
}

static void advanceToken()
{
    curToken = curToken->next;
}

static bool consumeToken(TokenType type)
{
    if (currentToken()->type == type) {
        advanceToken();
        return true;
    }
    return false;
}

static void expectToken(TokenType type)
{
    if (!consumeToken(type)) {
        unexpectedTokenError(EXIT_FAILURE, currentToken());
    }
}

// ================ Parser ================
Node *createNode(NodeType type)
{
    Node *node = calloc(1, sizeof(Node));
    node->type = type;
    return node;
}

Node *cons(Node *carNode, Node *cdrNode)
{
    Node *pair = createNode(ND_PAIR);
    pair->car = carNode;
    pair->cdr = cdrNode;
    return pair;
}

Node *car(Node *pair)
{
    if (pair->type != ND_PAIR) {
        illegalNodeTypeError(EXIT_FAILURE);
    }
    return pair->car;
}

Node *cdr(Node *pair)
{
    if (pair->type != ND_PAIR) {
        illegalNodeTypeError(EXIT_FAILURE);
    }
    return pair->cdr;
}

static bool verifyCurrentTokenType(TokenType type)
{
    return currentToken()->type == type;
}

static bool isPair()
{
    return verifyCurrentTokenType(TK_LEFT_PAREN);
}

static bool isNull()
{
    return verifyCurrentTokenType(TK_IDENTIFIER)
        && strcmp(currentToken()->identifier, "null") == 0;
}

static bool isString()
{
    return verifyCurrentTokenType(TK_STRING);
}

static bool isInteger()
{
    return verifyCurrentTokenType(TK_INTEGER);
}

static bool isNumeric()
{
    return verifyCurrentTokenType(TK_NUMERIC);
}

static bool isIdentifier()
{
    return verifyCurrentTokenType(TK_IDENTIFIER);
}

static Node *parseRecursive();
static Node *parsePair();
static Node *parsePairHelper();

static Node *parsePairHelper()
{
    if (consumeToken(TK_RIGHT_PAREN)) {
        return createNode(ND_NULL);
    }

    // 変数に代入することにより評価順序を明示的に示している．
    Node *carNode = parseRecursive();
    Node *cdrNode = parsePairHelper();
    Node *pair = cons(carNode, cdrNode);
    return pair;
}

static Node *parsePair()
{
    expectToken(TK_LEFT_PAREN);
    return parsePairHelper();
}

static Node *parseNull()
{
    Node *null = createNode(ND_NULL);
    advanceToken();
    return null;
}

static Node *parseString()
{
    Node *str = createNode(ND_STRING);
    str->string = currentToken()->string;
    advanceToken();
    return str;
}

static Node *parseInteger()
{
    Node *integer = createNode(ND_INTEGER);
    integer->integer = currentToken()->integer;
    advanceToken();
    return integer;
}

static Node *parseNumeric()
{
    Node *numeric = createNode(ND_NUMERIC);
    numeric->numeric = currentToken()->numeric;
    advanceToken();
    return numeric;
}

static Node *parseIdentifier()
{
    Node *ident = createNode(ND_IDENTIFIER);
    ident->identifier = currentToken()->identifier;
    advanceToken();
    return ident;
}

static Node *parseRecursive()
{
    Node *node;
    if (isPair()) {
        node = parsePair();
    }
    else if (isNull()) {
        node = parseNull();
    }
    else if (isString()) {
        node = parseString();
    }
    else if (isInteger()) {
        node = parseInteger();
    }
    else if (isNumeric()) {
        node = parseNumeric();
    }
    else if (isIdentifier()) {
        node = parseIdentifier();
    }
    else {
        unexpectedTokenError(EXIT_FAILURE, currentToken());
    }
    return node;
}

Node *parse(Token *tokenLst)
{
    curToken = tokenLst;
    Node *top = parseRecursive();
    expectToken(TK_EOF);
    return top;
}

// ================ Debug ================
static void dumpNodeHelper(FILE *outputStream, Node *node, int depth)
{
    for (int i = 0; depth > i; i++) {
        fprintf(outputStream, "\t");
    }
    switch (node->type) {
        case ND_PAIR:
            fprintf(outputStream, "PAIR:\n");
            dumpNodeHelper(outputStream, node->car, depth + 1);
            dumpNodeHelper(outputStream, node->cdr, depth + 1);
            break;
        case ND_NULL:
            fprintf(outputStream, "NULL()\n");
            break;
        case ND_STRING:
            fprintf(outputStream, "STRING(\"%s\")\n", node->string);
            break;
        case ND_INTEGER:
            fprintf(outputStream, "INTEGER(%lld)\n", node->integer);
            break;
        case ND_NUMERIC:
            fprintf(outputStream, "NUMERIC(%Lf)\n", node->numeric);
            break;
        case ND_IDENTIFIER:
            fprintf(outputStream, "IDENTIFIER(%s)\n", node->identifier);
            break;
    }
}

void dumpNode(FILE *outputStream, Node *node)
{
    dumpNodeHelper(outputStream, node, 0);
}

