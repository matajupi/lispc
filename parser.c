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
static Node *createNode(NodeType type)
{
    Node *node = calloc(1, sizeof(Node));
    node->type = type;
    return node;
}

static bool isPair()
{
    return currentToken()->type == TK_LEFT_PAREN;
}

static bool isNull()
{
    Token *token = currentToken();
    return token->type == TK_IDENTIFIER
        && strcmp(token->identifier, "null") == 0;
}

static bool isPrimitive()
{
    TokenType type = currentToken()->type;
    return type == TK_STRING || type == TK_NUMERIC
        || type == TK_INTEGER || type == TK_IDENTIFIER;
}

static Node *parseRecursive();
static Node *parsePair();
static Node *parsePairHelper();

static Node *parsePairHelper()
{
    if (consumeToken(TK_RIGHT_PAREN)) {
        return createNode(ND_NULL);
    }

    Node *pair = createNode(ND_PAIR);
    pair->car = parseRecursive();
    pair->cdr = parsePairHelper();
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

static Node *parsePrimitive()
{
    Node *primitive = createNode(ND_PRIMITIVE);
    primitive->token = currentToken();
    advanceToken();
    return primitive;
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
    else if (isPrimitive()) {
        node = parsePrimitive();
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
        case ND_PRIMITIVE:
            fprintf(outputStream, "PRIMITIVE(");
            dumpToken(outputStream, node->token);
            fprintf(outputStream, ")\n");
            break;
        case ND_NULL:
            fprintf(outputStream, "NULL()\n");
            break;
    }
}

void dumpNode(FILE *outputStream, Node *node)
{
    dumpNodeHelper(outputStream, node, 0);
}

