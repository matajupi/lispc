#include "lispc.h"

static GeneratorType genType;

static void generatePair(Node *node, FILE *outputStream)
{
    if (isInteger(node)) {
        generateInteger(node, outputStream);
    }
    else if (isNumeric(node)) {
        generateNumeric(node, outputStream);
    }
    else if (isString(node)) {
        generateString(node, outputStream);
    }
}

static void generateRecursive(Node *node, FILE *outputStream)
{
    switch (node->type) {
        case ND_TOP_LEVEL:
            for (Node *cur = node; cur != NULL; cur = cur->next) {
                generatePair(cur, outputStream);
            }
            break;
        case ND_PAIR:
            generatePair(node, outputStream);
            break;
        case ND_PRIMITIVE:
            generatePrimitive(node, outputStream);
            break;
        case ND_NULL:
            generateNull(node, outputStream);
            break;
    }
}

void generate(Node *node, FILE *outputStream, GeneratorType type)
{
    genType = type;
    generateRecursive(node, outputStream);
}

