#include "lispc.h"

static bool taggedList(Node *node, char *tag)
{
    return node->type == ND_PAIR
        && node->carNode->type == ND_IDENTIFIER
        && strcmp(node->carNode->token->identifier, tag) == 0;
}

// ================ Assignment ================
static bool isAssignment(Node *node)
{
    return taggedList(node, "set!");
}

static void generateAssignment(Node *node)
{
    // TODO
}

// ================ Definition ================
static bool isDefinition(Node *node)
{
    return taggedList(node, "define");
}

static void generateDefinition(Node *node)
{
    // TODO
}

static void generateRecursive(Node *, FILE *);
static void generatePair(Node *, FILE *);
static void generatePrimitive(Node *, FILE *);

static void generatePair(Node *node, FILE *outputStream)
{
    if (isAssignment(node)) {
        generateAssignment(node, outputStream);
    }
    else if (isDefinition(node)) {
        generateDefinition(node, outputStream);
    }
    else if (isIf(node)) {
        generateIf(node, outputStream);
    }
    else if (isLambda(node)) {
        generateProcedure(lambdaParameters(node), lambdaBody(node), outputStream);
    }
    else if (isLet(node)) {
        generateRecursivea(let2lambda(node), outputStream);
    }
    else if (isBegin(node)) {
        generateSequence(beginActions(node), outputStream);
    }
    else if (isCond(node)) {
        generateRecursive(cond2if(node), outputStream);
    }
    else if (isOr(node)) {
        generateOr(node, outputStream);
    }
    else if (isAnd(node)) {
        generateAnd(node, outputStream);
    }
    else if (isApplication(node)) {
        generateApplication(node, outputStream);
    }
}

static void generatePrimitive(Node *node, FILE *outputStream)
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
    else if (isVariable(node)) {
        generateVariable(node, outputStream);
    }
}

// TODO: Environment
// TODO: genreatePair + generatePrimitive is here
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
        case ND_NULL:
            generatePrimitive(node, outputStream);
            break;
    }
}

void generateArm32(Node *node, FILE *outputStream)
{
    // TODO: Create Environment
    // TODO: beginではdefineは最初にAddress領域をとっておく
    // TODO: TopLevel -> begin
}

