#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <assert.h>
#include "neillsdl2.h"

#define MAXNUMTOKENS 100 /* Max number of tokens */
#define MAXTOKENSIZE 20 /* Max length of each token */
#define strsame(A, B) (strcmp(A, B) == 0)
#define ERROR(PHRASE) fprintf(stderr, \
  "Fatal Error '%s' occurred in %s, line %d\n", \
  PHRASE, __FILE__, __LINE__ ); exit(2)
#define A 65 /* ASCII code for uppercase A */
#define Z 90 /* ASCII code for uppercase Z */
#define PI 3.14159265358979323846 /* Pi to 20 decimal places */
#define DEG 180 /* 180 degrees */
#define SIZE 256 /* Array size */
#define X_INIT 400 /* Initial value of x */
#define Y_INIT 300 /* Initial value of y */
#define WAIT_TIME 200 /* Pause between lines drawn */
#define PAUSE 1000 /* Pause at beginning and end of graphics */
#define RGBCODES 3 /* Total number of RGB values */
#define RGB0 0 /* Min RGB value */
#define RGB1 128 /* Mid RGB value */
#define RGB2 255 /* Max RGB value */

struct prog {
  char inst[MAXNUMTOKENS][MAXTOKENSIZE]; /* Token read */
  int crt; /* Current token number */
  int num; /* Total token number */
};
typedef struct prog Program;

struct coords {
  double x_prev;
  double y_prev;
  double x_new;
  double y_new;
  double angle;
  double arr[SIZE];
  double values[SIZE];
  int rgb[RGBCODES];
};
typedef struct coords Coords;

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

struct node {
  double x;
  double y;
  int rgb[RGBCODES];
  struct node *next; /* Link to next node */
};
typedef struct node Node;

void MainProg(Program *p, Stack *s, Node *head, Coords *cd);
void Instrctlst(Program *p, Stack *s, Node *head, Coords *cd);
int Instruction(Program *p, Stack *s, Node *head, Coords *cd);
double Forward(Program *p, Node *n, Coords *cd);
double LeftTurn(Program *p, Coords *cd);
double RightTurn(Program *p, Coords *cd);
int DoLoop(Program *p, Stack *s, Node *head, Coords *cd);
int VarNum(Program *p);
double VarDec(Program *p);
char Var(Program *p);
int Set(Program *p, Stack *s, Coords *cd);
double Polish(Program *p, Stack *s, Coords *cd);
int Oper(Program *p);
double Calc(Stack *s, Coords *cd, int op);
double Convert(Stack *s, Coords *cd);

void InitStack(Stack *s);
void Push(Stack *s, char *c);
char Pop(Stack *s);
void FreeStack(Stack *s);

int Colour(Program *p, Node *n, Coords *cd);
void RunCompiler(SDL_Simplewin *w, Node *n);
Node* AllocateNode(Coords *cd);
void FreeNode(Node **n);
