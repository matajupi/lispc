#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STACK_SIZE 1024
#define MAX_NUM_ARGUMENTS 64

// ================ Declarations ================
typedef struct Environment Environment;
typedef struct Procedure Procedure;
typedef struct Integer Integer;
typedef struct String String;

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

struct Environment
{
    void *bindings;
    Environment *enclosing;
};

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

struct Integer
{
    long long content;
};

struct String
{
    const char *content;
};

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

// TODO: Add string.h
#define INT2STRING_BUFFER_SIZE 32

void applyIntBinaryOperator(long long (*operator)(long long, long long))
{
    r0 = args[0];
    r1 = args[1];
    ret = malloc(sizeof(Integer));
    ((Integer *)ret)->content = operator(((Integer *)r0)->content, ((Integer *)r1)->content);
}

void applyIntUnaryOperator(long long (*operator)(long long))
{
    r0 = args[0];
    ret = malloc(sizeof(Integer));
    ((Integer *)ret)->content = operator(((Integer *)r0)->content);
}

long long opAdd(long long v1, long long v2) { return v1 + v2; }

void pAdd()
{
    applyIntBinaryOperator(opAdd);
}

long long opSub(long long v1, long long v2) { return v1 - v2; }

void pSub()
{
    applyIntBinaryOperator(opSub);
}

long long opMul(long long v1, long long v2) { return v1 * v2; }

void pMul()
{
    applyIntBinaryOperator(opMul);
}

long long opDiv(long long v1, long long v2) { return v1 / v2; }

void pDiv()
{
    applyIntBinaryOperator(opDiv);
}

long long opRemainder(long long v1, long long v2) { return v1 % v2; }

void pRemainder()
{
    applyIntBinaryOperator(opRemainder);
}

long long opEqual(long long v1, long long v2) { return v1 == v2; }

void pEqual()
{
    applyIntBinaryOperator(opEqual);
}

long long opGreater(long long v1, long long v2) { return v1 > v2; }

void pGreater()
{
    applyIntBinaryOperator(opGreater);
}

long long opGreaterEqual(long long v1, long long v2) { return v1 >= v2; }

void pGreaterEqual()
{
    applyIntBinaryOperator(opGreaterEqual);
}

long long opLess(long long v1, long long v2) { return v1 < v2; }

void pLess()
{
    applyIntBinaryOperator(opLess);
}

long long opLessEqual(long long v1, long long v2) { return v1 <= v2; }

void pLessEqual()
{
    applyIntBinaryOperator(opLessEqual);
}

long long opNot(long long v) { return !v; }

void pNot()
{
    applyIntUnaryOperator(opNot);
}

void pEqualString()
{
    r0 = args[0];
    r1 = args[1];
    ret = malloc(sizeof(Integer));
    ((Integer *)ret)->content = strcmp(((String *)r0)->content, ((String *)r1)->content) == 0;
}

void pPrint()
{
    r0 = args[0];
    printf("%s\n", ((String *)r0)->content);
}

void pInt2string()
{
    r0 = args[0];

    char *buffer = malloc(sizeof(char) * INT2STRING_BUFFER_SIZE);
    sprintf(buffer, "%lld", ((Integer *)r0)->content);

    ret = malloc(sizeof(String));
    ((String *)ret)->content = buffer;
}

int main()
{
    return 0;
}
