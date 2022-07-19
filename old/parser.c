#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include "lispc.h"

#define MAX_STRING_BUFFER_SIZE 1024

static Token *curToken;

static void dumpNodeHelper(FILE *stream, Node *node, int depth)
{
    for (int i = 0; depth > i; i++) {
        fprintf(stream, "\t");
    }
    switch (node->type) {
        case ND_TOP_LEVEL:
            fprintf(stream, "TOP_LEVEL:\n");
            for (Node *cur = node->next; cur != NULL; cur = cur->next) {
                dumpNodeHelper(stream, cur, depth + 1);
            }
            break;
        case ND_PRIMITIVE:
            fprintf(stream, "PRIMITIVE(");
            dumpToken(stream, node->token);
            fprintf(stream, ")\n");
            break;
        case ND_PAIR:
            fprintf(stream, "PAIR:\n");
            dumpNodeHelper(stream, node->carNode, depth + 1);
            dumpNodeHelper(stream, node->cdrNode, depth + 1);
            break;
        case ND_NULL:
            fprintf(stream, "NULL()\n");
            break;
    }
}

void dumpNode(FILE *stream, Node *node)
{
    dumpNodeHelper(stream, node, 0);
}

void unexpectedTokenError(byte status)
{
    fprintf(stderr, "Error: Unexpected token \"");
    dumpToken(stderr, curToken);
    fprintf(stderr, "\".\n");
    exit(status);
}

static Node *createNode(NodeType type)
{
    Node *node = calloc(1, sizeof(Node));
    node->type = type;
    return node;
}

static void advanceToken()
{
    curToken = curToken->next;
}

static void expectToken(TokenType type)
{
    if (curToken->type == type) {
        advanceToken();
        return;
    }
    unexpectedTokenError(EXIT_FAILURE);
}

static bool consumeToken(TokenType type)
{
    if (curToken->type == type) {
        advanceToken();
        return true;
    }
    return false;
}

static Token *currentToken()
{
    return curToken;
}

static bool atEOF()
{
    return curToken->type == TK_EOF;
}

static Node *parsePairHelper();
static Node *parsePair();
static Node *parsePrimitive();
static Node *parseRecursive();

static Node *parsePairHelper()
{
    if (consumeToken(TK_RIGHT_PAREN)) {
        return createNode(ND_NULL);
    }

    Node *pair = createNode(ND_PAIR);
    pair->carNode = parseRecursive();
    pair->cdrNode = parsePairHelper();
    return pair;
}

static Node *parsePair()
{
    expectToken(TK_LEFT_PAREN);
    return parsePairHelper();
}

static Node *parsePrimitive()
{
    Node *node = createNode(ND_PRIMITIVE);
    node->token = currentToken();
    advanceToken();
    return node;
}

static Node *parseRecursive()
{
    Node *node;
    switch (currentToken()->type) {
        case TK_LEFT_PAREN:
            node = parsePair();
            break;
        case TK_STRING:
        case TK_INTEGER:
        case TK_NUMERIC:
        case TK_IDENTIFIER:
            node = parsePrimitive();
            break;
        default:
            unexpectedTokenError(EXIT_FAILURE);
    }
    return node;
}

Node *parse(Token *token)
{
    curToken = token;
    Node *top = createNode(ND_TOP_LEVEL);
    Node headNode;
    Node *curNode = &headNode;
    while (!atEOF()) {
        curNode->next = parsePair();
        curNode = curNode->next;
    }
    top->next = headNode.next;
    return top;
}

