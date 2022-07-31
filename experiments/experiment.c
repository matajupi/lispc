#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#define STACK_SIZE 1024

// ================ Interpretation ================
// ================ Declarations ================
typedef struct Primitive Primitive;
typedef struct Environment Environment;
typedef struct Integer Integer;
typedef struct Procedure Procedure;

void assignPrimitive(Primitive *src, Primitive **dest);
void freePrimitive(Primitive *obj);
Environment *createEnvironment(Environment *enclosing);
void freeEnvironment(Environment *env);
Integer *createInteger(int content);
void freeInteger(Integer *integer);
Procedure *createProcedure(unsigned int numParameters, void *lambda, Environment *base);
void freeProcedure(Procedure *proc);
void callProcedure(Procedure *proc, Primitive **retval);

// ================ Primitive ================
struct Primitive
{
    unsigned int referenceCounter;
};

void assignPrimitive(Primitive *src, Primitive **dest)
{
    (src->referenceCounter)++;
    *dest = src;
}

void freePrimitive(Primitive *obj)
{
    (obj->referenceCounter)--;
    if (obj->referenceCounter == 0) {
        switch (obj->type) {
            case TEnvironment:
                freeEnvironment(obj);
                break;
            case TInteger:
                freeInteger(obj);
                break;
            case TProcedure:
                freeProcedure(obj);
                break;
            default:
                fprintf(stderr, "ERROR: Unsupported Type\n");
                exit(EXIT_FAILURE);
        }
    }
}

// ================ Binding ================
struct Binding
{
    Primitive *value;
    Binding *next;
};

Binding *createBinding(Primitive *value)
{
    Binding *binding = calloc(1, sizeof(Binding));
    assignPrimitive(value, &(binding->value));
    return binding;
}

void freeBinding(Binding *binding)
{
    if (binding->next) {
        freeBinding(binding->next);
    }
    freePrimitive(binding->value);
    free(binding);
}

// ================ Environment ================
struct Environment
{
    unsigned int referenceCounter;
    Type type;
    Environment *enclosing;
    Binding *bindings;
};

Environment *createEnvironment(Environment *enclosing)
{
    Environment *env = calloc(1, sizeof(Environment));
    env->referenceCounter = 0;
    env->type = TEnvironment;
    if (enclosing) {
        assignPrimitive(enclosing, &(env->enclosing));
    }
    else {
        env->enclosing = NULL;
    }
    return env;
}

void freeEnvironment(Environment *env)
{
    if (env->enclosing) {
        freePrimitive(env->enclosing);
    }
    if (env->bindings) {
        freeBinding(env->bindings);
    }
    free(env);
}

// ================ Integer ================
struct Integer
{
    unsigned int referenceCounter;
    Type type;
    int content;
};

Integer *createInteger(int content)
{
    Integer *integer = calloc(1, sizeof(Integer));
    integer->referenceCounter = 0;
    integer->type = TInteger;
    integer->content = content;
    return integer;
}

void freeInteger(Integer *integer)
{
    free(integer);
}

// ================ Procedure ================
struct Procedure
{
    unsigned int referenceCounter;
    Type type;
    Environment *base;
    unsigned int numParameters;
    void *lambda;
};

Procedure *createProcedure(unsigned int numParameters, void *lambda, Environment *base)
{
    Procedure *proc = calloc(1, sizeof(Procedure));
    proc->referenceCounter = 0;
    proc->type = TProcedure;
    assignPrimitive(base, &(proc->base));
    proc->numParameters = numParameters;
    proc->lambda = lambda;
    return proc;
}

void freeProcedure(Procedure *proc)
{
    freePrimitive(proc->base);
    free(proc->lambda);
    free(proc);
}

// ================ Machine ================
// ================ Registers ================
size_t sp = STACK_SIZE, fp = STACK_SIZE;
Primitive *r0 = NULL, *r1 = NULL, *r2 = NULL, *r3 = NULL;
Environment *env;

// ================ Stack ================
void *stack[STACK_SIZE];

void push(void *obj) {
    stack[--sp] = obj;
}

void pushPrimitive(Primitive *obj)
{
    assignPrimitive(obj, &stack[--sp]);
}

void *pop()
{
    return stack[sp++];
}

void popPrimitive(Primitive **dest)
{
    assignPrimitive(stack[sp], dest);
    freePrimitive(stack[sp++]);
}

// ================ Prologue ================
void prologue()
{
    push(env);
    push(fp);
    fp = sp;
}

// ================ Epilogue ================
void epilogue()
{
    sp = fp;
    fp = pop();
    env = pop();
}

// ================ Execution ================
void callProcedure(Procedure *proc, Primitive **retval)
{
    // prologue();
    Environment *env;
    assignPrimitive(createEnvironment(proc->base), &env);
    Binding head;
    head.next = NULL;
    Binding *cur = &head;
    // 引数はstackにpushしてある
    for (unsigned int i = 0; proc->numParameters > i; i++) {
        popPrimitive(&r1);
        cur->next = createBinding(r1);
    }
    env->bindings = binding;
    // 先にenvironmentを作って引数を格納し，それをlambdaに渡す
}

// ================ Lambdas ================
// Primitive *Lambda2(Environment *enclosing)
// {
//     Environment *env;
//     assignPrimitive(createEnvironment(0, enclosing), &env);
//     Primitive *retval = env->enclosing->bindings[0];
//     freePrimitive(&env);
//     return retval;
// }
// 
// Primitive *Lambda1(Environment *enclosing, Primitive *op0)
// {
//     Environment *env;
//     assignPrimitive(createEnvironment(1, enclosing), &env);
//     assignPrimitive(op0, &(env->bindings[0]));
//     assignPrimitive(createProcedure(0, Lambda2, env), &retval);
//     freePrimitive(&env);
//     return retval;
// }
// 
// Primitive *Lambda0(Environment *enclosing)
// {
//     Environment *env;
//     assignPrimitive(createEnvironment(1, enclosing), &env);
//     // define resolution.(Use graph.)
//     assignPrimitive(createProcedure(1, Lambda1, env), &(env->bindings[0]));
//     Primitive *retval = callProcedure(env->bindings[0]);
//     freePrimitive(&env);
//     return NULL;
// }
// 
// ================ Main ================
int main()
{
    // (let ()) = ((lambda ())) => generate lambda -> call procedure
    prologue();
    assignPrimitive(createEnvironment(NULL), &env);
    // TODO: Add primitive procedure to env
    // generate arguments
    // generate lambda registration
    pushPrimitive(createProcedure(0, Lambda0, env));
    popPrimitive(&r0);
    // call
    callProcedure(r0, &r0);
    freePrimitive(env);
    epilogue();
    return 0;
}

