#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lispc.h"

#define MAX_NUM_LAMBDA 1024

static FILE *ostream;

static bool isTaggedList(Node *node, const char *tag)
{
    if (node->type == ND_PAIR) {
        Node *carNode = car(node);
        return carNode->type == ND_IDENTIFIER
            && strcmp(carNode->identifier, tag) == 0;
    }
    return false;
}

static bool verifyNodeType(Node *node, NodeType type)
{
    return node->type == type;
}

static void gen(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vprintf(ostream, fmt, ap);
    fprintf(ostream, "\n");
    va_end(ap);
}

static void generateRecursive(Node *node, Environment *env);

// ================ Header, Prologue, Epilogue ================
static void generateHeader()
{
    gen("
#include <stdio.h>
#include <stdlib.h>

#define STACK_SIZE 1024
#define ARGUMENT_LIST_SIZE 64

// ================ Declarations ================
typedef struct Environment Environment;
typedef struct Procedure Procedure;
typedef struct Integer Integer;

// ================ Machine ================
// ================ Registers ================
size_t sp, fp;
void *ret, *t0, *t1, *t2, *t3;
Environment *env, *benv;
Procedure *proc;

// ================ Stack ================
void *stack[STACK_SIZE];

void push(void *obj)
{
    stack[--sp] = obj;
}

void *pop()
{
    return stack[sp++];
}

void *argumentList[ARGUMENT_LIST_SIZE];

void initMachine()
{
    sp = fp = STACK_SIZE;
    ret = t0 = t1 = t2 = t3 = NULL;
    env = benv = NULL;
    proc = NULL;
}

// ================ Environment ================
struct Environment
{
    void *bindings;
    Environment *enclosing;
};

Environment *createEnvironment(size_t numBindings, Environment *enclosing)
{
    Environment *env = calloc(1, sizeof(Environment));
    env->bindings = calloc(numBindings, sizeof(void *));
    env->enclosing = enclosing;
    return env;
}

// ================ Procedure ================
struct Procedure
{
    void (*func)();
    Environment *base;
};

Procedure *createProcedure(void (*func)(), Environment *base)
{
    Procedure *proc = calloc(1, sizeof(Procedure));
    proc->func = func;
    proc->base = base;
    return proc;
}

void callProcedure()
{
    benv = proc->base;
    proc->func();
}

// ================ Integer ================
struct Integer
{
    int content;
};

Integer *createInteger(int content)
{
    Integer *integer = calloc(1, sizeof(Integer));
    integer->content = content;
    return integer;
}

// ================ stmfd, ldmfd ================
void stmfd()
{
    push(env);
    push(fp);
    fp = sp;
}

void ldmfd()
{
    sp = fp;
    fp = pop();
    env = pop();
}
    ");
}

// ================ Integer ================
static bool isInteger(Node *node)
{
    return verifyNodeType(node, ND_INTEGER);
}

static void generateInteger(Node *node)
{
    gen("push(createInteger(%lld));", node->integer);
}

// ================ Lambda ================
static Node *isLambda(Node *node)
{
    return isTaggedList(node, "lambda");
}

static Node genLambdaNodes[MAX_NUM_LAMBDA];
static Environment genLambdaEnvs[MAX_NUM_LAMBDA];
static size_t numLambda = 0;

static void generateProcedure(Node *node, Environment *env)
{
    genLambdaNodes[numLambda] = node;
    genLambdaEnvs[numLambda] = env;
    gen("push(createProcedure(Lambda%d, env));", numLambda);
    genLambda++;
}

static void generateLambda(Node *node, Environment *env, int id)
{
    gen("void Lambda%d() {", id);
    gen("stmfd();");
    int numDefinitions = countDefinitions(lambdaBody(node)); // TODO: cd, lb
    gen("env = createEnvironment(%d, benv);", numDefinition);
    int numParameters = listLength(lambdaParameters(node));
}

static void generateLambdas()
{
    for (int i = 0; numLambda > i; i++) {
        Node *node = genLambdaNodes[i];
        Environment *env = genLambdaEnvs[i];
        generateLambda(node, env, i);
    }
}

// ================ Let ================
static bool isLet(Node *node)
{
    return isTaggedList(node, "let");
}

// ================ Application ================
static bool isApplication(Node *node)
{
    return verifyNodeType(node, ND_PAIR);
}

static Node *operator(Node *app)
{
    return car(app);
}

static Node *arguments(Node *app)
{
    return cdr(app);
}

static int generateListOfValues(Node *node, Environment *env)
{
    if (verifyNodeType(node, ND_NULL)) {
        return 0;
    }
    int numArguments = generateListOfValues(cdr(node), env) + 1;
    generateRecursive(car(node), env);
    return numArguments;
}

static void generateApplication(int numArguments)
{
    for (int i = 0; numArguments > i; i++) {
        gen("argumentList[%d] = pop();", i);
    }
    gen("proc = pop();");
    gen("callProcedure();");
    gen("push(ret);");
}

// ================ Generator ================
static void generateRecursive(Node *node, Environment *env)
{
    // support
    // type: procdure, integer, string, null
    // syntax: lambda, if, begin, define, set!, (or, and), application
    if (isInteger(node)) {
        generateInteger(node);
    }
    else if (isLambda(node)) {
        generateProcedure(node, env);
    }
    else if (isApplication(node)) {
        generateRecursive(operator(node), env);
        int numArguments = generateListOfValues(arguments(node), env);
        generateApplication(numArguments);
    }
    // TODO: if, begin, define, set!, or, and, variable, string, null
}

static void generateMain(Node *node)
{
    gen("int main() {");
    gen("initMachine();");
    Environment *env = createEnvironment(0, NULL);
    gen("env = createEnvironment(0, benv);");
    // TODO:add primitive procedure
    generateRecursive(node, env);
    gen("ret = pop();");
    gen("return ((Integer *)ret)->content;");
    gen("}");
}

void generateC(Node *topNode, FILE *outputStream)
{
    FILE *tmpStream = tmpfile();
    ostream = outputStream;
    generateHeader();
    ostream = tmpStream;
    // TODO:Env <- +, -, primitive procedure, true, false, etc...
    generateMain(topNode);
    generateLambdas();
    ostream = outputStream;
    generateLambdaDeclarations();
    appendStream(outputStream, tmpStream); // TODO
}

