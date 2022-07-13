#include "lispc.h"

static Token *curToken;

static void unexpectedTokenError(byte status)
{
    fprintf(stderr, "Error: Unexpected token \"");
    dumpToken(stderr, token);
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

static TokenType currentToken()
{
    return curToken;
}

static bool atEOF()
{
    return curToken->type == TK_EOF;
}

static Node *parsePairHelper()
{
    if (consumeToken(TK_RIGHT_PAREN)) {
        return createNode(ND_NULL);
    }

    Node *pair = createNode(ND_PAIR);
    pair->car = parseRecursive();

    if (consumeToken(TK_DOT)) {
        pair->cdr = parseRecursive();
        expectToken(TK_RIGHT_PAREN);
        return pair;
    }
    pair->cdr = parsePairHelper();
    return pair;
}

static Node *parsePair()
{
    expectToken(TK_LEFT_PAREN);
    return parsePairHelper();
}

static Node *parseSymbol()
{
    expectToken(TK_QUOTE);
    Node *pair = createNode(ND_PAIR);
    pair->car = createNode(ND_IDENTIFIER);
    pair->car->identifier = "quote";
    pair->cdr = createNode(ND_PAIR);
    pair->cdr->car = parseRecursive();
    pair->cdr->cdr = createNode(ND_NULL);
    return pair;
}

static Node *parseRecursive()
{
    Node *node;
    switch (currentToken()->type) {
        case TK_LEFT_PAREN:
            node = parsePair();
            break;
        case TK_QUOTE:
            node = parseSymbol();
            break;
        case TK_DOUBLE_QUOTE:
            node = parseString();
            break;
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
        cur->next = parsePair();
        cur = cur->next;
    }
    top->next = headNode.next;
    return top;
}
