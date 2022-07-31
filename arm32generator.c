#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "lispc.h"

#define MAX_NUM_LAMBDA 1024

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
    gen("ldr r0, [sp]");
    gen("mov sp, fp");
    gen("ldmfd sp!, {fp, pc}");
}

// ================ Integer ================
static bool isInteger(Node *node)
{
    return verifyNodeType(node, ND_INTEGER);
}

static void generateInteger(Node *node)
{
    gen("mov r0, #8");
    gen("bl malloc");

    gen("mov r1, #0");
    gen("str r1, [r0]");

    gen("mov r1, #%d", node->integer);
    gen("str, r1, [r0, #4]");

    gen("push {r0}");
}

// ================ Lambda ================
static Node *isLambda(Node *node)
{
    return isTaggedList(node, "lambda");
}

static Node *createLambda(Node *params, Node *body)
{
    Node *lambdaTag = createNode(ND_IDENTIFIER);
    lambdaTag->identifier = "lambda";
    return cons(lambdaTag, cons(params, body));
}

static Node *genLambdaNodes[MAX_NUM_LAMBDA];
static Environment *genLambdaEnvs[MAX_NUM_LAMBDA];
static size_t numLambda = 0;

static int countDefine(Node *node)
{
    // TODO
    // define, lambda, letの中には立ち入らない
}

static Node *lambdaParameters(Node *node)
{
    return car(cdr(node));
}

static int countParamters(Node *node)
{
    int counter = 0;
    for (; verifyNodeType(node, ND_PAIR); node = cdr(node)) {
        counter++;
    }
    return counter;
}

static void generateLambda()
{
    for (size_t i = 0; numLambda > i; i++) {
        Node *node = genLambdaNodes[i];
        Environment *env = genLambdaNodes[i];

        gen(".Lambda{%d}:", i);
        generatePrologue();

        // count define
        int numDefine = countDefine(node);
        // parameter list
        Node *parameters = lambdaParameters(node);
        int numParameters = countParameters(node);
        // 引数退避
        for (int i = 0; numParameters > i; i++) {
            gen("push {r%d}", i);
        }
        // malloc
        // 引数格納
        // create environment -> r0
        // 簡単のためアセンブラのenvironmen側にも
        // 前のenvironmentのアドレスをもたせる
        // environment用レジスタを作って実行中はそれを使い，
        // 関数に入るときはfpとかと一緒にpushしておく
        // NOOOOOOOOOOOOOOO
        // Lambdaはprocedureのアドレスをpushするのではなく
        // procedure objectを作る <= base environment付き
        // ただ，environmentにenclosing-environmentを持たせるのは必要
        // 一番外にあるdefine解決
    }
    // TODO: Lambdaは最後に出力する．
    // TODO: word sectionも
    // .L{labelCounter}:
    // stmfd sp!, {fp, lr}
    // mov fp, sp
    // label create environment -> r4
    // <label inc>
    // ref counter ++
    // <end label>
    // push r4
    // <Procedure>
    // pop r0
    // <label dec>
    // ref counter --
    // if ref counter == 0 then free environment
    // <end label>
    // ldmfd sp!, {fp, pc}
    // lambdaNodeCurIdx = 0;
    // numLambda = 0;
}

static void registerLambda(Node *node, Environment *env)
{
    gen("ldr r0, .LambdaWord+%d", numLambda * 4);
    gen("push {r0}");

    genLambdaNodes[numLambda] = node;
    genLambdaEnvs[numLambda] = env;

    numLambda++;
}

// ================ Let ================
static bool isLet(Node *node)
{
    return isTaggedList(node, "let");
}

static Node *letParameterArgumentList(Node *let)
{
    return car(cdr(let));
}

static Node *letParametersHelper(Node *lst)
{
    if (verifyNodeType(lst, ND_NULL)) {
        return lst;
    }
    return cons(car(car(lst)), letParametersHelper(cdr(lst)));
}

static Node *letParameters(Node *let)
{
    Node *lst = letParameterArgumentList(let);
    return letParametersHelper(lst);
}

static Node *letBody(Node *let)
{
    return cdr(cdr(let));
}

static Node *letArgumentsHelper(Node *lst)
{
    if (verifyNodeType(ND_NULL)) {
        return lst;
    }
    return cons(car(cdr(car(lst))), letArgumentsHelper(cdr(lst)));
}

static Node *letArguments(Node *let)
{
    Node *lst = letParameterArgumentList(let);
    return letArgumentsHelper(lst);
}

static Node *letToLambdaCall(Node *let)
{
    return = cons(createLambda(letParameters(let), letBody(let)),
                  letArguments(let));
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
    // cf: ARM32 ABI
    if (numArguments > 4) {
        tooManyArgumentsError(EXIT_FAILURE);
    }
    // Arguments
    for (int i = 0; numArguments < i; i++) {
        gen("pop {r%d}", i);
    }

    // Function pointer
    gen("pop {r4}");

    // Call
    gen("blx r4");

    // Push return value
    gen("push {r0}");
}

// ================ Generator ================
static void generateRecursive(Node *node, Environment *env)
{
    if (isInteger(node)) {
        generateInteger(node);
    }
    else if (isLambda(node)) {
        registerLambda(node, env);
    }
    else if (isLet(node)) {
        generateRecursive(letToLambdaCall(node), env);
    }
    else if (isApplication(node)) {
        generateRecursive(operator(node), env);
        int numArguments = generateListOfValues(arguments(node), env);
        generateApplication(numArguments);
    }
}

void generateArm32(Node *topNode, FILE *ostream)
{
    ostream = ostream;
    generateHeader();
    generatePrologue();
    // TODO:Env <- +, -, primitive procedure, true, false, etc...
    generateRecursive(topNode, NULL);
    generateLambda();
    generateEpilogue();
}

