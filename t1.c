#include <stdio.h>
#include <stdlib.h>

#define STACK_SIZE 1024
#define MAX_NUM_ARGUMENTS 64

typedef struct Environment Environment;
typedef struct Procedure Procedure;
typedef struct Integer Integer;

size_t sp, fp;
void *ret, *r0, *r1, *r2, *r3;
Environment *env, *benv;
Procedure *proc;

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
void **bindings;
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
void Lambda0();
int main() {
initMachine();
env = malloc(sizeof(Environment));
env->bindings = malloc(sizeof(void *) * 0);
env->enclosing = benv;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = Lambda0;
((Procedure *)r0)->base = env;
push(r0);
proc = pop();
call();
push(ret);
ret = pop();
return ((Integer *)ret)->content;
}
void Lambda0() {
stmfd();
env = malloc(sizeof(Environment));
env->bindings = malloc(sizeof(void *) * 3);
env->enclosing = benv;
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 3;
push(r0);
env->bindings[0] = pop();
r0 = env;
push(((Environment *)r0)->bindings[0]);
r0 = pop();
if (((Integer *)r0)->content) {
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 4;
push(r0);
env->bindings[1] = pop();
}
else {
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 5;
push(r0);
env->bindings[1] = pop();
}
r0 = env;
push(((Environment *)r0)->bindings[1]);
ret = pop();
ldmfd();
}
