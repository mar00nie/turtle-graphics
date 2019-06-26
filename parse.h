#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>

#define MAXNUMTOKENS 100 /* Max number of tokens */
#define MAXTOKENSIZE 20 /* Max length of each token */
#define strsame(A, B) (strcmp(A, B) == 0)
#define ERROR(PHRASE) fprintf(stderr, \
  "Fatal Error '%s' occurred in %s, line %d\n", \
  PHRASE, __FILE__, __LINE__ ); exit(2)
#define A 65 /* ASCII code for uppercase A */
#define Z 90 /* ASCII code for uppercase Z */
#define SIZE 256 /* Array size */

struct prog {
  char inst[MAXNUMTOKENS][MAXTOKENSIZE]; /* Token read */
  int crt; /* Current token number */
  int num; /* Total token number */
};
typedef struct prog Program;

struct stackelem {
  char code;
  struct stackelem *prev;
};
typedef struct stackelem Elem;

struct thestack {
  Elem *top;
  int cnt; /* Stack counter */
};
typedef struct thestack Stack;

void MainProg(Program *p, Stack *s);
void Instrctlst(Program *p, Stack *s);
int Instruction(Program *p, Stack *s);
int Forward(Program *p);
int LeftTurn(Program *p);
int RightTurn(Program *p);
int DoLoop(Program *p, Stack *s);
int VarNum(Program *p);
char Var(Program *p);
int Set(Program *p, Stack *s);
int Polish(Program *p, Stack *s);
int Oper(Program *p);

void InitStack(Stack *s);
void Push(Stack *s, char *c);
char Pop(Stack *s);
void FreeStack(Stack *s);
void test_parse();
