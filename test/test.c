#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INT2STRING_BUFFER_SIZE 32

#define STACK_SIZE 1024
#define MAX_NUM_ARGUMENTS 64

typedef struct Environment Environment;
typedef struct Procedure Procedure;
typedef struct Integer Integer;
typedef struct String String;

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

void pIsNull()
{
pNot();
}

long long opOr(long long v1, long long v2) { return v1 || v2; }

void pOr()
{
applyIntBinaryOperator(opOr);
}

long long opAnd(long long v1, long long v2) { return v1 && v2; }

void pAnd()
{
applyIntBinaryOperator(opAnd);
}

void Lambda0();
void Lambda1();
void Lambda2();
void Lambda3();
void Lambda4();
void Lambda5();
void Lambda6();
void Lambda7();
void Lambda8();
void Lambda9();
void Lambda10();
void Lambda11();
void Lambda12();
void Lambda13();
int main() {
initMachine();
env = malloc(sizeof(Environment));
env->bindings = malloc(sizeof(void *) * 18);
env->enclosing = benv;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pAdd;
((Procedure *)r0)->base = env;
env->bindings[0] = r0;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pSub;
((Procedure *)r0)->base = env;
env->bindings[1] = r0;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pMul;
((Procedure *)r0)->base = env;
env->bindings[2] = r0;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pDiv;
((Procedure *)r0)->base = env;
env->bindings[3] = r0;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pRemainder;
((Procedure *)r0)->base = env;
env->bindings[4] = r0;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pEqual;
((Procedure *)r0)->base = env;
env->bindings[5] = r0;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pEqual;
((Procedure *)r0)->base = env;
env->bindings[6] = r0;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pGreater;
((Procedure *)r0)->base = env;
env->bindings[7] = r0;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pGreaterEqual;
((Procedure *)r0)->base = env;
env->bindings[8] = r0;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pLess;
((Procedure *)r0)->base = env;
env->bindings[9] = r0;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pLessEqual;
((Procedure *)r0)->base = env;
env->bindings[10] = r0;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pNot;
((Procedure *)r0)->base = env;
env->bindings[11] = r0;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pEqualString;
((Procedure *)r0)->base = env;
env->bindings[12] = r0;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pPrint;
((Procedure *)r0)->base = env;
env->bindings[13] = r0;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pInt2string;
((Procedure *)r0)->base = env;
env->bindings[14] = r0;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pIsNull;
((Procedure *)r0)->base = env;
env->bindings[15] = r0;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pOr;
((Procedure *)r0)->base = env;
env->bindings[16] = r0;
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = pAnd;
((Procedure *)r0)->base = env;
env->bindings[17] = r0;
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
env->bindings = malloc(sizeof(void *) * 12);
env->enclosing = benv;
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = Lambda1;
((Procedure *)r0)->base = env;
push(r0);
env->bindings[0] = pop();
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = Lambda2;
((Procedure *)r0)->base = env;
push(r0);
env->bindings[1] = pop();
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = Lambda3;
((Procedure *)r0)->base = env;
push(r0);
env->bindings[2] = pop();
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = Lambda4;
((Procedure *)r0)->base = env;
push(r0);
env->bindings[3] = pop();
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = Lambda5;
((Procedure *)r0)->base = env;
push(r0);
env->bindings[4] = pop();
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = Lambda6;
((Procedure *)r0)->base = env;
push(r0);
env->bindings[5] = pop();
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = Lambda7;
((Procedure *)r0)->base = env;
push(r0);
env->bindings[6] = pop();
r0 = env;
push(((Environment *)r0)->bindings[1]);
r0 = malloc(sizeof(String));
((String *)r0)->content = "cdrだよ";
push(r0);
r0 = malloc(sizeof(String));
((String *)r0)->content = "carだよ";
push(r0);
args[0] = pop();
args[1] = pop();
proc = pop();
call();
push(ret);
env->bindings[7] = pop();
r0 = env;
push(((Environment *)r0)->bindings[6]);
r0 = env;
push(((Environment *)r0)->bindings[7]);
args[0] = pop();
proc = pop();
call();
push(ret);
r0 = env;
push(((Environment *)r0)->bindings[4]);
r0 = malloc(sizeof(String));
((String *)r0)->content = "再代入したcarだよ";
push(r0);
r0 = env;
push(((Environment *)r0)->bindings[7]);
args[0] = pop();
args[1] = pop();
proc = pop();
call();
push(ret);
r0 = env;
push(((Environment *)r0)->bindings[5]);
r0 = malloc(sizeof(String));
((String *)r0)->content = "再代入したcdrだよ";
push(r0);
r0 = env;
push(((Environment *)r0)->bindings[7]);
args[0] = pop();
args[1] = pop();
proc = pop();
call();
push(ret);
r0 = env;
push(((Environment *)r0)->bindings[6]);
r0 = env;
push(((Environment *)r0)->bindings[7]);
args[0] = pop();
proc = pop();
call();
push(ret);
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = Lambda8;
((Procedure *)r0)->base = env;
push(r0);
env->bindings[8] = pop();
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
env->bindings[9] = pop();
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = Lambda9;
((Procedure *)r0)->base = env;
push(r0);
env->bindings[10] = pop();
r0 = env;
push(((Environment *)r0)->bindings[10]);
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 10;
push(r0);
args[0] = pop();
proc = pop();
call();
push(ret);
env->bindings[9] = pop();
r0 = env;
push(((Environment *)r0)->bindings[8]);
r0 = malloc(sizeof(String));
((String *)r0)->content = "int";
push(r0);
r0 = env;
push(((Environment *)r0)->bindings[9]);
args[0] = pop();
args[1] = pop();
proc = pop();
call();
push(ret);
ret = pop();
ldmfd();
}
void Lambda1() {
stmfd();
env = malloc(sizeof(Environment));
env->bindings = malloc(sizeof(void *) * 2);
env->enclosing = benv;
env->bindings[0] = args[0];
env->bindings[1] = args[1];
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = Lambda10;
((Procedure *)r0)->base = env;
push(r0);
ret = pop();
ldmfd();
}
void Lambda2() {
stmfd();
env = malloc(sizeof(Environment));
env->bindings = malloc(sizeof(void *) * 2);
env->enclosing = benv;
env->bindings[0] = args[0];
env->bindings[1] = args[1];
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[0]);
r0 = env;
push(((Environment *)r0)->bindings[1]);
r0 = env;
push(((Environment *)r0)->bindings[0]);
args[0] = pop();
args[1] = pop();
proc = pop();
call();
push(ret);
ret = pop();
ldmfd();
}
void Lambda3() {
stmfd();
env = malloc(sizeof(Environment));
env->bindings = malloc(sizeof(void *) * 1);
env->enclosing = benv;
env->bindings[0] = args[0];
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
r0 = env;
push(((Environment *)r0)->bindings[0]);
r0 = malloc(sizeof(String));
((String *)r0)->content = "get-car";
push(r0);
args[0] = pop();
proc = pop();
call();
push(ret);
ret = pop();
ldmfd();
}
void Lambda4() {
stmfd();
env = malloc(sizeof(Environment));
env->bindings = malloc(sizeof(void *) * 1);
env->enclosing = benv;
env->bindings[0] = args[0];
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
r0 = env;
push(((Environment *)r0)->bindings[0]);
r0 = malloc(sizeof(String));
((String *)r0)->content = "get-cdr";
push(r0);
args[0] = pop();
proc = pop();
call();
push(ret);
ret = pop();
ldmfd();
}
void Lambda5() {
stmfd();
env = malloc(sizeof(Environment));
env->bindings = malloc(sizeof(void *) * 2);
env->enclosing = benv;
env->bindings[0] = args[0];
env->bindings[1] = args[1];
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
r0 = env;
push(((Environment *)r0)->bindings[0]);
r0 = malloc(sizeof(String));
((String *)r0)->content = "set-car!";
push(r0);
args[0] = pop();
proc = pop();
call();
push(ret);
r0 = env;
push(((Environment *)r0)->bindings[1]);
args[0] = pop();
proc = pop();
call();
push(ret);
ret = pop();
ldmfd();
}
void Lambda6() {
stmfd();
env = malloc(sizeof(Environment));
env->bindings = malloc(sizeof(void *) * 2);
env->enclosing = benv;
env->bindings[0] = args[0];
env->bindings[1] = args[1];
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
r0 = env;
push(((Environment *)r0)->bindings[0]);
r0 = malloc(sizeof(String));
((String *)r0)->content = "set-cdr!";
push(r0);
args[0] = pop();
proc = pop();
call();
push(ret);
r0 = env;
push(((Environment *)r0)->bindings[1]);
args[0] = pop();
proc = pop();
call();
push(ret);
ret = pop();
ldmfd();
}
void Lambda7() {
stmfd();
env = malloc(sizeof(Environment));
env->bindings = malloc(sizeof(void *) * 1);
env->enclosing = benv;
env->bindings[0] = args[0];
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[13]);
r0 = malloc(sizeof(String));
((String *)r0)->content = "car:";
push(r0);
args[0] = pop();
proc = pop();
call();
push(ret);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[13]);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[2]);
r0 = env;
push(((Environment *)r0)->bindings[0]);
args[0] = pop();
proc = pop();
call();
push(ret);
args[0] = pop();
proc = pop();
call();
push(ret);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[13]);
r0 = malloc(sizeof(String));
((String *)r0)->content = "cdr:";
push(r0);
args[0] = pop();
proc = pop();
call();
push(ret);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[13]);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[3]);
r0 = env;
push(((Environment *)r0)->bindings[0]);
args[0] = pop();
proc = pop();
call();
push(ret);
args[0] = pop();
proc = pop();
call();
push(ret);
ret = pop();
ldmfd();
}
void Lambda8() {
stmfd();
env = malloc(sizeof(Environment));
env->bindings = malloc(sizeof(void *) * 3);
env->enclosing = benv;
env->bindings[0] = args[0];
env->bindings[1] = args[1];
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = Lambda11;
((Procedure *)r0)->base = env;
push(r0);
env->bindings[2] = pop();
r0 = env;
push(((Environment *)r0)->bindings[2]);
r0 = env;
push(((Environment *)r0)->bindings[0]);
args[0] = pop();
proc = pop();
call();
push(ret);
ret = pop();
ldmfd();
}
void Lambda9() {
stmfd();
env = malloc(sizeof(Environment));
env->bindings = malloc(sizeof(void *) * 1);
env->enclosing = benv;
env->bindings[0] = args[0];
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[5]);
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
r0 = env;
push(((Environment *)r0)->bindings[0]);
args[0] = pop();
args[1] = pop();
proc = pop();
call();
push(ret);
r0 = pop();
if (((Integer *)r0)->content) {
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
}
else {
r0 = env;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[1]);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[10]);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[1]);
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 1;
push(r0);
r0 = env;
push(((Environment *)r0)->bindings[0]);
args[0] = pop();
args[1] = pop();
proc = pop();
call();
push(ret);
args[0] = pop();
proc = pop();
call();
push(ret);
r0 = env;
push(((Environment *)r0)->bindings[0]);
args[0] = pop();
args[1] = pop();
proc = pop();
call();
push(ret);
}
ret = pop();
ldmfd();
}
void Lambda10() {
stmfd();
env = malloc(sizeof(Environment));
env->bindings = malloc(sizeof(void *) * 1);
env->enclosing = benv;
env->bindings[0] = args[0];
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[12]);
r0 = malloc(sizeof(String));
((String *)r0)->content = "get-car";
push(r0);
r0 = env;
push(((Environment *)r0)->bindings[0]);
args[0] = pop();
args[1] = pop();
proc = pop();
call();
push(ret);
r0 = pop();
if (((Integer *)r0)->content) {
r0 = env;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[0]);
}
else {
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[12]);
r0 = malloc(sizeof(String));
((String *)r0)->content = "get-cdr";
push(r0);
r0 = env;
push(((Environment *)r0)->bindings[0]);
args[0] = pop();
args[1] = pop();
proc = pop();
call();
push(ret);
r0 = pop();
if (((Integer *)r0)->content) {
r0 = env;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[1]);
}
else {
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[12]);
r0 = malloc(sizeof(String));
((String *)r0)->content = "set-car!";
push(r0);
r0 = env;
push(((Environment *)r0)->bindings[0]);
args[0] = pop();
args[1] = pop();
proc = pop();
call();
push(ret);
r0 = pop();
if (((Integer *)r0)->content) {
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = Lambda12;
((Procedure *)r0)->base = env;
push(r0);
}
else {
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[12]);
r0 = malloc(sizeof(String));
((String *)r0)->content = "set-cdr!";
push(r0);
r0 = env;
push(((Environment *)r0)->bindings[0]);
args[0] = pop();
args[1] = pop();
proc = pop();
call();
push(ret);
r0 = pop();
if (((Integer *)r0)->content) {
r0 = malloc(sizeof(Procedure));
((Procedure *)r0)->func = Lambda13;
((Procedure *)r0)->base = env;
push(r0);
}
}
}
}
ret = pop();
ldmfd();
}
void Lambda11() {
stmfd();
env = malloc(sizeof(Environment));
env->bindings = malloc(sizeof(void *) * 1);
env->enclosing = benv;
env->bindings[0] = args[0];
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[15]);
r0 = env;
push(((Environment *)r0)->bindings[0]);
args[0] = pop();
proc = pop();
call();
push(ret);
r0 = pop();
if (((Integer *)r0)->content) {
r0 = malloc(sizeof(String));
((String *)r0)->content = "ok";
push(r0);
}
else {
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[12]);
r0 = malloc(sizeof(String));
((String *)r0)->content = "int";
push(r0);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[1]);
args[0] = pop();
args[1] = pop();
proc = pop();
call();
push(ret);
r0 = pop();
if (((Integer *)r0)->content) {
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[13]);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[14]);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[2]);
r0 = env;
push(((Environment *)r0)->bindings[0]);
args[0] = pop();
proc = pop();
call();
push(ret);
args[0] = pop();
proc = pop();
call();
push(ret);
args[0] = pop();
proc = pop();
call();
push(ret);
}
else {
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[13]);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[2]);
r0 = env;
push(((Environment *)r0)->bindings[0]);
args[0] = pop();
proc = pop();
call();
push(ret);
args[0] = pop();
proc = pop();
call();
push(ret);
}
r0 = env;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[2]);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
push(((Environment *)r0)->bindings[3]);
r0 = env;
push(((Environment *)r0)->bindings[0]);
args[0] = pop();
proc = pop();
call();
push(ret);
args[0] = pop();
proc = pop();
call();
push(ret);
}
ret = pop();
ldmfd();
}
void Lambda12() {
stmfd();
env = malloc(sizeof(Environment));
env->bindings = malloc(sizeof(void *) * 1);
env->enclosing = benv;
env->bindings[0] = args[0];
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
r0 = env;
push(((Environment *)r0)->bindings[0]);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
((Environment *)r0)->bindings[0] = pop();
ret = pop();
ldmfd();
}
void Lambda13() {
stmfd();
env = malloc(sizeof(Environment));
env->bindings = malloc(sizeof(void *) * 1);
env->enclosing = benv;
env->bindings[0] = args[0];
r0 = malloc(sizeof(Integer));
((Integer *)r0)->content = 0;
push(r0);
r0 = env;
push(((Environment *)r0)->bindings[0]);
r0 = env;
r0 = ((Environment *)r0)->enclosing;
r0 = ((Environment *)r0)->enclosing;
((Environment *)r0)->bindings[1] = pop();
ret = pop();
ldmfd();
}
