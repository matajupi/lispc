#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdbool.h>
#include <string.h>

#include "lispc.h"

#define MAX_NUM_LAMBDA 1024
#define BUFFER_SIZE 1024

// ================ Environment ================
Environment *createEnvironment(size_t numBindings, Environment *enclosing)
{
    Environment *env = calloc(1, sizeof(Environment));
    env->enclosing = enclosing;
    env->variables = calloc(numBindings, sizeof(Node*));
    env->numVariables = 0;
    return env;
}

// ================ Generator-utils ================
static FILE *outputStream;

static void setOutputStream(FILE *ostream)
{
    outputStream = ostream;
}

static void gen(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(outputStream, fmt, ap);
    fprintf(outputStream, "\n");
    va_end(ap);
}

static bool verifyNodeType(Node *node, NodeType type)
{
    return node->type == type;
}

static bool isPair(Node *exp)
{
    return verifyNodeType(exp, ND_PAIR);
}

static bool isIdentifier(Node *exp)
{
    return verifyNodeType(exp, ND_IDENTIFIER);
}

static bool isTaggedList(Node *node, const char *tag)
{
    if (isPair(node)) {
        Node *carNode = car(node);
        return isIdentifier(carNode)
            && strcmp(carNode->identifier, tag) == 0;
    }
    return false;
}

static unsigned int length(Node *exps)
{
    unsigned int n = 0;
    for (Node *cur = exps; isPair(cur); cur = cdr(cur)) {
        n++;
    }
    return n;
}

// ================ Generators ================
static void genRecursive(Node *exp, Environment *env);
static void genSequence(Node *exp, Environment *env);

static void genEnvironment(unsigned int numBindings)
{
    gen("env = malloc(sizeof(Environment));");
    gen("env->bindings = malloc(sizeof(void *) * %u);", numBindings);
    gen("env->enclosing = benv;");
}

// ================ Integer ================
static bool isInteger(Node *exp)
{
    return verifyNodeType(exp, ND_INTEGER);
}

static void genInteger(long long integer)
{
    gen("r0 = malloc(sizeof(Integer));");
    gen("((Integer *)r0)->content = %lld;", integer);
    gen("push(r0);");
}
// ================ Variable ================
static int lookupVariableAddress(char *var, Environment *env)
{
    int address = -1;
    for (int i = 0; env->numVariables > i; i++) {
        if (strcmp(var, env->variables[i]->identifier) == 0) {
            address = i;
        }
    }
    return address;
}

static void genScanVariable(char *var, Environment *env, void (*foundProc)(int))
{
    gen("r0 = env;");
    for (Environment *cur = env; cur != NULL; cur = cur->enclosing) {
        int address = lookupVariableAddress(var, cur);
        if (address != -1) {
            foundProc(address);
            return;
        }
        gen("r0 = ((Environment *)r0)->enclosing;");
    }
    unboundVariableError(EXIT_FAILURE);
}

static void genGetVariableValue(int address)
{
    gen("push(((Environment *)r0)->bindings[%d]);", address);
}

static void genSetVariableValue(int address)
{
    gen("((Environment *)r0)->bindings[%d] = pop();", address);
}

static void genVariable(Node *exp, Environment *env)
{
    genScanVariable(exp->identifier, env, genGetVariableValue);
}

// ================ Null ================
static bool isNull(Node *exp)
{
    return verifyNodeType(exp, ND_NULL)
        || (isIdentifier(exp)
        && strcmp(exp->identifier, "null") == 0);
}

static void genNull()
{
    genInteger(0);
}

// ================ Definition ================
static bool isDefinition(Node *exp)
{
    return isTaggedList(exp, "define");
}

static Node *definitionVariable(Node *exp)
{
    Node *var = car(cdr(exp));
    if (!isIdentifier(var)) {
        illegalNodeTypeError(EXIT_FAILURE);
    }
    return var;
}

static Node *definitionValue(Node *exp)
{
    return car(cdr(cdr(exp)));
}

static void genDefinition(Node *exp, Environment *env)
{
    env->variables[env->numVariables] = definitionVariable(exp);
    genRecursive(definitionValue(exp), env);
    gen("env->bindings[%u] = pop();", env->numVariables);
    env->numVariables++;
}

// ================ Assignment ================
static bool isAssignment(Node *exp)
{
    return isTaggedList(exp, "set!");
}

static Node *assignmentVariable(Node *exp)
{
    Node *var = car(cdr(exp));
    if (!isIdentifier(var)) {
        illegalNodeTypeError(EXIT_FAILURE);
    }
    return var;
}

static Node *assignmentValue(Node *exp)
{
    return car(cdr(cdr(exp)));
}

static void genAssignment(Node *exp, Environment *env)
{
    genRecursive(assignmentValue(exp), env);
    genScanVariable(assignmentVariable(exp)->identifier, env, genSetVariableValue);
}

// ================ Lambda ================
static unsigned int numLambdas = 0;
static Node *lambdaExps[MAX_NUM_LAMBDA];
static Environment *lambdaEnvs[MAX_NUM_LAMBDA];

static bool isLambda(Node *exp)
{
    return isTaggedList(exp, "lambda");
}

static Node *lambdaParameters(Node *exp)
{
    Node *params = car(cdr(exp));
    if (!(isPair(params) || isNull(params))) {
        illegalNodeTypeError(EXIT_FAILURE);
    }
    return params;
}

static Node *lambdaBody(Node *exp)
{
    return cdr(cdr(exp));
}

static unsigned int countDefinitions(Node *exp)
{
    unsigned int numDefinitions;
    if (isLambda(exp)) {
        numDefinitions = 0;
    }
    else if (isDefinition(exp)) {
        numDefinitions = 1;
    }
    else if (isPair(exp)) {
        numDefinitions = countDefinitions(car(exp)) + countDefinitions(cdr(exp));
    }
    else {
        numDefinitions = 0;
    }
    return numDefinitions;
}

static void bindParameters(Node *params, Environment *env)
{
    unsigned int i = 0;
    for (Node *cur = params; isPair(cur); cur = cdr(cur)) {
        Node *ident = car(cur);
        if (!isIdentifier(ident)) {
            illegalNodeTypeError(EXIT_FAILURE);
        }
        env->variables[i] = ident;
        i++;
    }
    env->numVariables = i;
}

static void registLambda(Node *exp, Environment *env)
{
    lambdaExps[numLambdas] = exp;
    lambdaEnvs[numLambdas] = env;
    numLambdas++;
}

static void genProcedure(Node *exp, Environment *env)
{
    gen("r0 = malloc(sizeof(Procedure));");
    gen("((Procedure *)r0)->func = Lambda%u;", numLambdas);
    gen("((Procedure *)r0)->base = env;");
    gen("push(r0);");
    registLambda(exp, env);
}

static void genLambda(unsigned int id)
{
    Node *exp = lambdaExps[id];
    Environment *benv = lambdaEnvs[id];
    gen("void Lambda%u() {", id);
    gen("stmfd();");

    Node *params = lambdaParameters(exp);
    unsigned int numParams = length(params);
    Node *body = lambdaBody(exp);
    unsigned int numDefinitions = countDefinitions(body);
    unsigned int numBindings = numParams + numDefinitions;

    genEnvironment(numBindings);
    Environment *env = createEnvironment(numBindings, benv);
    bindParameters(params, env);
    for (unsigned int i = 0; numParams > i; i++) {
        gen("env->bindings[%u] = args[%u];", i, i);
    }

    genNull();
    genSequence(body, env);

    gen("ret = pop();");
    gen("ldmfd();");
    gen("}");
}

static void genLambdaDeclarations()
{
    for (unsigned int i = 0; numLambdas > i; i++) {
        gen("void Lambda%u();", i);
    }
}

// ================ Application ================
static bool isApplication(Node *exp)
{
    return isPair(exp);
}

static Node *operator(Node *exp)
{
    return car(exp);
}

static Node *operands(Node *exp)
{
    return cdr(exp);
}

static void genListOfValuesReverseOrder(Node *exps, Environment *env)
{
    if (isNull(exps)) {
        return;
    }
    genListOfValuesReverseOrder(cdr(exps), env);
    genRecursive(car(exps), env);
}

static void genApplication(Node *exp, Environment *env)
{
    genRecursive(operator(exp), env);
    Node *args = operands(exp);
    genListOfValuesReverseOrder(args, env);
    unsigned int numArgs = length(args);
    for (unsigned int i = 0; numArgs > i; i++) {
        gen("args[%u] = pop();", i);
    }
    gen("proc = pop();");
    gen("call();");
    gen("push(ret);");
}

// ================ Other ================
static void genRecursive(Node *exp, Environment *env)
{
    // TODO: string, begin, if, or, and
    if (isNull(exp)) {
        genNull();
    }
    else if (isInteger(exp)) {
        genInteger(exp->integer);
    }
    else if (isIdentifier(exp)) {
        genVariable(exp, env);
    }
    else if (isDefinition(exp)) {
        genDefinition(exp, env);
    }
    else if (isAssignment(exp)) {
        genAssignment(exp, env);
    }
    else if (isLambda(exp)) {
        genProcedure(exp, env);
    }
    else if (isApplication(exp)) {
        genApplication(exp, env);
    }
    else {
        unimplementedFeatureError(EXIT_FAILURE);
    }
}

static void genSequence(Node *exps, Environment *env)
{
    for (Node *cur = exps; isPair(cur); cur = cdr(cur)) {
        genRecursive(car(cur), env);
    }
}

static void init()
{
    numLambdas = 0;
}

static void genMain(Node *exp)
{
    gen("int main() {");
    gen("initMachine();");
    init();
    genEnvironment(0);
    Environment *env = createEnvironment(0, NULL);
    // TODO: add primitive procedures
    genRecursive(exp, env);
    gen("ret = pop();");
    gen("return ((Integer *)ret)->content;");
    gen("}");

    for (unsigned int i = 0; numLambdas > i; i++) {
        genLambda(i);
    }
}

static void genHeader()
{
    gen("#include <stdio.h>");
    gen("#include <stdlib.h>");
    gen("");
    gen("#define STACK_SIZE 1024");
    gen("#define MAX_NUM_ARGUMENTS 64");
    gen("");
    gen("typedef struct Environment Environment;");
    gen("typedef struct Procedure Procedure;");
    gen("typedef struct Integer Integer;");
    gen("");
    gen("size_t sp, fp;");
    gen("void *ret, *r0, *r1, *r2, *r3;");
    gen("Environment *env, *benv;");
    gen("Procedure *proc;");
    gen("");
    gen("void *stack[STACK_SIZE];");
    gen("");
    gen("void push(void *obj)");
    gen("{");
    gen("stack[--sp] = obj;");
    gen("}");
    gen("");
    gen("void *pop()");
    gen("{");
    gen("return stack[sp++];");
    gen("}");
    gen("");
    gen("void *args[MAX_NUM_ARGUMENTS];");
    gen("");
    gen("void initMachine()");
    gen("{");
    gen("sp = fp = STACK_SIZE;");
    gen("ret = r0 = r1 = r2 = r3 = NULL;");
    gen("env = benv = NULL;");
    gen("proc = NULL;");
    gen("}");
    gen("");
    gen("struct Environment");
    gen("{");
    gen("void **bindings;");
    gen("Environment *enclosing;");
    gen("};");
    gen("");
    gen("struct Procedure");
    gen("{");
    gen("void (*func)();");
    gen("Environment *base;");
    gen("};");
    gen("");
    gen("void call()");
    gen("{");
    gen("benv = proc->base;");
    gen("proc->func();");
    gen("}");
    gen("");
    gen("struct Integer");
    gen("{");
    gen("long long content;");
    gen("};");
    gen("");
    gen("void stmfd()");
    gen("{");
    gen("push(env);");
    gen("push(fp);");
    gen("fp = sp;");
    gen("}");
    gen("");
    gen("void ldmfd()");
    gen("{");
    gen("sp = fp;");
    gen("fp = pop();");
    gen("env = pop();");
    gen("}");
}

static char buffer[BUFFER_SIZE];

static void appendStream(FILE *dest, FILE *src)
{
    fseek(src, 0, SEEK_SET);
    while (fgets(buffer, BUFFER_SIZE, src) != NULL) {
        fputs(buffer, dest);
    }
}

void generateC(Node *exp, FILE *ostream)
{
    setOutputStream(ostream);
    genHeader();
    FILE *tstream = tmpfile();
    setOutputStream(tstream);
    genMain(exp);
    setOutputStream(ostream);
    genLambdaDeclarations();
    appendStream(ostream, tstream);
}

