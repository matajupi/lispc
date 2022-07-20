#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lispc.h"

static bool isInteger(Node *node)
{
    return node->token && node->token->type == TK_INTEGER;
}

// ================ GenerateProcedures ================
static void gen(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(ostream, fmt, ap);
    fprintf(ostream, "\n");
    va_end(ap);
}

static void generateHeader()
{
    gen(".section .text");
    gen(".global main");
    gen("main:");
}

static void generatePrologue()
{
    gen("stmfd sp!, {fp, lr}");
    gen("mov fp, sp");
}

static void generateEpilogue()
{
    gen("mov sp, fp");
    gen("ldmfd sp!, {fp, pc}");
}

static void generateInteger(Node *node)
{
    gen("mov r0, #8");
    gen("bl malloc");

    gen("mov r1, #0");
    gen("str r1, [r0]");

    gen("mov r1, #%d", node->token->integer);
    gen("str, r1, [r0, #4]");

    gen("push {r0}");
}

static void generateRecursive(Node *node, Environment *env)
{
    if (isInteger(node)) {
        generateInteger(node);
    }
    else if (isLet(node)) {
        generateRecursive(letToLambdaCall(node), env);
    }
}

void generateArm32(Node *topNode, FILE *ostream)
{
    ostream = ostream;
    generateHeader();
    generatePrologue();
    generateRecursive(topNode, NULL);
    generateEpilogue();
}

