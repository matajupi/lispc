#include <stdio.h>
#include <stdlib.h>

#define STACK_SIZE 1024
#define MAX_NUM_ARGUMENTS 64

// ================ Declarations ================
typedef struct Environment Environment;
typedef struct Procedure Procedure;
typedef struct Integer Integer;

// ================ Machine ================
// ================ Registers ================
size_t sp, fp;
void *ret, *r0, *r1, *r2, *r3;
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

void *args[MAX_NUM_ARGUMENTS];

void initMachine()
{
    sp = fp = STACK_SIZE;
    ret = r0 = r1 = r2 = r3 = NULL;
    env = benv = NULL;
    proc = NULL;
}

// ================ Environment ================
struct Environment
{
    void *bindings;
    Environment *enclosing;
};

// ================ Procedure ================
struct Procedure
{
    void (*func)();
    Environment *base;
};

void call()
{
    benv = proc->base;
    proc->func();
}

// ================ Integer ================
struct Integer
{
    long long content;
};

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

// ================ Lambda Declarations ================
void Lambda0();

// ================ Main ================
// とりあえずメモリ管理は行わない．（やるならGCになるだろう．）
// Typeの管理も行わない．（コンパイラは責任を追わない．）
// そのためProcedureのArgumentの数の管理も行われない．

int main()
{
    initMachine();
    env = createEnvironment(0, benv);
    // TODO: Add primitive procedures
    // global = (let ()) = ((lambda ())) => create procedure object, push, call procedure, generate lambda
    push(createProcedure(Lambda0, env)); // function pointer, enclosing environment
    proc = pop();
    // push arguments
    call();
    push(ret);
    ret = pop();
    return ((Integer *)ret)->content;
}

void Lambda0()
{
    // prologue
    stmfd();
    env = createEnvironment(0, benv); // enclosing environment, num_bindings(definiton + parameter)
    // pop and regist arguments
    // push dummy ret value
    push(createInteger(0));
    // body
    // ....
    // epilogue
    ret = pop();
    ldmfd();
}

