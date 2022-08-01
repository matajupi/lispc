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
    env->variables = calloc(numBindings, sizeof(char *));
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

static Node *operator(Node *exp)
{
    return car(exp);
}

static Node *operands(Node *exp)
{
    return cdr(exp);
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

// ================ String ================
static bool isString(Node *exp)
{
    return verifyNodeType(exp, ND_STRING);
}

static void genString(char *str)
{
    gen("r0 = malloc(sizeof(String));");
    gen("((String *)r0)->content = \"%s\";", str);
    gen("push(r0);");
}

// ================ Variable ================
static int lookupVariableAddress(char *var, Environment *env)
{
    int address = -1;
    for (int i = 0; env->numVariables > i; i++) {
        if (strcmp(var, env->variables[i]) == 0) {
            address = i;
        }
    }
    return address;
}

static void genScanVariable(char *var, Environment *env,
                            void (*foundProc)(int), void (*notFoundProc)())
{
    // 環境を外側に向かって探索．束縛が見つかればfoundProcを実行．
    // 見つからなければnotFoundProcを実行．
    gen("r0 = env;");
    for (Environment *cur = env; cur != NULL; cur = cur->enclosing) {
        int address = lookupVariableAddress(var, cur);
        if (address != -1) {
            foundProc(address);
            return;
        }
        gen("r0 = ((Environment *)r0)->enclosing;");
    }
    notFoundProc();
}

static void throwUnboundVariableError()
{
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
    genScanVariable(exp->identifier, env, genGetVariableValue, throwUnboundVariableError);
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
    // "現在の" 環境にすでに同名の束縛が存在すれば代入．
    // そうでなければ束縛を追加．
    genRecursive(definitionValue(exp), env);
    Node *var = definitionVariable(exp);
    int address = lookupVariableAddress(var->identifier, env);
    if (address == -1) {
        env->variables[env->numVariables] = var->identifier;
        gen("env->bindings[%u] = pop();", env->numVariables);
        env->numVariables++;
    }
    else {
        gen("env->bindings[%d] = pop();", address);
    }
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
    genScanVariable(assignmentVariable(exp)->identifier, env,
                    genSetVariableValue, throwUnboundVariableError);
}

// ================ If ================
static bool isIf(Node *exp)
{
    return isTaggedList(exp, "if");
}

static Node *ifPredicate(Node *exp)
{
    return car(cdr(exp));
}

static Node *ifConsequent(Node *exp)
{
    return car(cdr(cdr(exp)));
}

static bool hasIfAlternative(Node *exp)
{
    return isPair(cdr(cdr(cdr(exp))));
}

static Node *ifAlternative(Node *exp)
{
    return car(cdr(cdr(cdr(exp))));
}

static void genIf(Node *exp, Environment *env)
{
    genRecursive(ifPredicate(exp), env);
    gen("r0 = pop();");
    gen("if (((Integer *)r0)->content) {");
    genRecursive(ifConsequent(exp), env);
    gen("}");
    if (hasIfAlternative(exp)) {
        gen("else {");
        genRecursive(ifAlternative(exp), env);
        gen("}");
    }
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

static void genBindParameters(Node *params, Environment *env)
{
    unsigned int i = 0;
    for (Node *cur = params; isPair(cur); cur = cdr(cur)) {
        Node *ident = car(cur);
        if (!isIdentifier(ident)) {
            illegalNodeTypeError(EXIT_FAILURE);
        }
        env->variables[i] = ident->identifier;
        gen("env->bindings[%u] = args[%u];", i, i);
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
    genBindParameters(params, env);

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

// ================ Begin ================
static bool isBegin(Node *exp)
{
    return isTaggedList(exp, "begin");
}

static Node *beginActions(Node *exp)
{
    return cdr(exp);
}

// ================ Application ================
static bool isApplication(Node *exp)
{
    return isPair(exp);
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
    // or, andのshort-circuit-evaluationは今度サポートする
    // 可変引数はとりあえず実装しない
    // cond, define procedure, let, numeric, boolean, symbolなどもとりあえず実装しない
    // Typeはとりあえずサポートしない
    // 引数の数はとりあえずサポートしない
    if (isNull(exp)) {
        genNull();
    }
    else if (isInteger(exp)) {
        genInteger(exp->integer);
    }
    else if (isString(exp)) {
        genString(exp->string);
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
    else if (isIf(exp)) {
        genIf(exp, env);
    }
    else if (isLambda(exp)) {
        genProcedure(exp, env);
    }
    else if (isBegin(exp)) {
        genSequence(beginActions(exp), env);
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

static char *primitiveProcedureVariables[] =
{
    "+",
    "-",
    "*",
    "/",
    "remainder",
    "=",
    "eq?",
    ">",
    ">=",
    "<",
    "<=",
    "not",
    "string=",

    "print",
    "int->string",

    "null?",
    "or", // TODO: short circuit
    "and", // TODO: short circuit
};

static char *primitiveProcedureFunctions[] =
{
    "pAdd",
    "pSub",
    "pMul",
    "pDiv",
    "pRemainder",
    "pEqual",
    "pEqual",
    "pGreater",
    "pGreaterEqual",
    "pLess",
    "pLessEqual",
    "pNot",
    "pEqualString",

    "pPrint",
    "pInt2string",

    "pIsNull",
    "pOr",
    "pAnd",
};

static size_t getNumPrimitiveProcedures()
{
    return sizeof(primitiveProcedureVariables) / sizeof(char *);
}

static void genBindPrimitiveProcedures(Environment *env)
{
    size_t numPrimitiveProcedures = getNumPrimitiveProcedures();
    for (unsigned int i = 0; numPrimitiveProcedures > i; i++) {
        env->variables[i] = primitiveProcedureVariables[i];
        gen("r0 = malloc(sizeof(Procedure));");
        gen("((Procedure *)r0)->func = %s;", primitiveProcedureFunctions[i]);
        gen("((Procedure *)r0)->base = env;");
        gen("env->bindings[%u] = r0;", i);
    }
    env->numVariables = numPrimitiveProcedures;
}

static void genMain(Node *exp)
{
    gen("int main() {");
    gen("initMachine();");
    init();
    size_t numPrimitiveProcedures = getNumPrimitiveProcedures();
    genEnvironment(numPrimitiveProcedures);
    Environment *env = createEnvironment(numPrimitiveProcedures, NULL);
    genBindPrimitiveProcedures(env);
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
    gen("#include <string.h>");
    gen("");
    gen("#define INT2STRING_BUFFER_SIZE 32");
    gen("");
    gen("#define STACK_SIZE 1024");
    gen("#define MAX_NUM_ARGUMENTS 64");
    gen("");
    gen("typedef struct Environment Environment;");
    gen("typedef struct Procedure Procedure;");
    gen("typedef struct Integer Integer;");
    gen("typedef struct String String;");
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
    gen("struct String");
    gen("{");
    gen("const char *content;");
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
    gen("");
    gen("void applyIntBinaryOperator(long long (*operator)(long long, long long))");
    gen("{");
    gen("r0 = args[0];");
    gen("r1 = args[1];");
    gen("ret = malloc(sizeof(Integer));");
    gen("((Integer *)ret)->content = operator(((Integer *)r0)->content, ((Integer *)r1)->content);");
    gen("}");
    gen("");
    gen("void applyIntUnaryOperator(long long (*operator)(long long))");
    gen("{");
    gen("r0 = args[0];");
    gen("ret = malloc(sizeof(Integer));");
    gen("((Integer *)ret)->content = operator(((Integer *)r0)->content);");
    gen("}");
    gen("");
    gen("long long opAdd(long long v1, long long v2) { return v1 + v2; }");
    gen("");
    gen("void pAdd()");
    gen("{");
    gen("applyIntBinaryOperator(opAdd);");
    gen("}");
    gen("");
    gen("long long opSub(long long v1, long long v2) { return v1 - v2; }");
    gen("");
    gen("void pSub()");
    gen("{");
    gen("applyIntBinaryOperator(opSub);");
    gen("}");
    gen("");
    gen("long long opMul(long long v1, long long v2) { return v1 * v2; }");
    gen("");
    gen("void pMul()");
    gen("{");
    gen("applyIntBinaryOperator(opMul);");
    gen("}");
    gen("");
    gen("long long opDiv(long long v1, long long v2) { return v1 / v2; }");
    gen("");
    gen("void pDiv()");
    gen("{");
    gen("applyIntBinaryOperator(opDiv);");
    gen("}");
    gen("");
    gen("long long opRemainder(long long v1, long long v2) { return v1 % v2; }");
    gen("");
    gen("void pRemainder()");
    gen("{");
    gen("applyIntBinaryOperator(opRemainder);");
    gen("}");
    gen("");
    gen("long long opEqual(long long v1, long long v2) { return v1 == v2; }");
    gen("");
    gen("void pEqual()");
    gen("{");
    gen("applyIntBinaryOperator(opEqual);");
    gen("}");
    gen("");
    gen("long long opGreater(long long v1, long long v2) { return v1 > v2; }");
    gen("");
    gen("void pGreater()");
    gen("{");
    gen("applyIntBinaryOperator(opGreater);");
    gen("}");
    gen("");
    gen("long long opGreaterEqual(long long v1, long long v2) { return v1 >= v2; }");
    gen("");
    gen("void pGreaterEqual()");
    gen("{");
    gen("applyIntBinaryOperator(opGreaterEqual);");
    gen("}");
    gen("");
    gen("long long opLess(long long v1, long long v2) { return v1 < v2; }");
    gen("");
    gen("void pLess()");
    gen("{");
    gen("applyIntBinaryOperator(opLess);");
    gen("}");
    gen("");
    gen("long long opLessEqual(long long v1, long long v2) { return v1 <= v2; }");
    gen("");
    gen("void pLessEqual()");
    gen("{");
    gen("applyIntBinaryOperator(opLessEqual);");
    gen("}");
    gen("");
    gen("long long opNot(long long v) { return !v; }");
    gen("");
    gen("void pNot()");
    gen("{");
    gen("applyIntUnaryOperator(opNot);");
    gen("}");
    gen("");
    gen("void pEqualString()");
    gen("{");
    gen("r0 = args[0];");
    gen("r1 = args[1];");
    gen("ret = malloc(sizeof(Integer));");
    gen("((Integer *)ret)->content = strcmp(((String *)r0)->content, ((String *)r1)->content) == 0;");
    gen("}");
    gen("");
    gen("void pPrint()");
    gen("{");
    gen("r0 = args[0];");
    gen("printf(\"%%s\\n\", ((String *)r0)->content);");
    gen("}");
    gen("");
    gen("void pInt2string()");
    gen("{");
    gen("r0 = args[0];");
    gen("");
    gen("char *buffer = malloc(sizeof(char) * INT2STRING_BUFFER_SIZE);");
    gen("sprintf(buffer, \"%%lld\", ((Integer *)r0)->content);");
    gen("");
    gen("ret = malloc(sizeof(String));");
    gen("((String *)ret)->content = buffer;");
    gen("}");
    gen("");
    gen("void pIsNull()");
    gen("{");
    gen("pNot();");
    gen("}");
    gen("");
    gen("long long opOr(long long v1, long long v2) { return v1 || v2; }");
    gen("");
    gen("void pOr()");
    gen("{");
    gen("applyIntBinaryOperator(opOr);");
    gen("}");
    gen("");
    gen("long long opAnd(long long v1, long long v2) { return v1 && v2; }");
    gen("");
    gen("void pAnd()");
    gen("{");
    gen("applyIntBinaryOperator(opAnd);");
    gen("}");
    gen("");
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

