#include "parse.h"

struct example {
  char* array[SIZE];
  int cnt;
};
typedef struct example Example;

int test_forward(Program *p, char* c, char* d);
int test_leftturn(Program *p, char* c, char* d);
int test_rightturn(Program *p, char* c, char* d);
int test_doloop(Program *p, Stack *s, Example *e);
int test_leftbrace(Program *p, char* c);
int test_rightbrace(Program *p, char* c);
int test_varnum(Program *p, char* c);
char test_var(Program *p, char* c);
int test_set(Program *p, Stack *s, Example *e);
int test_polish(Program *p, Stack *s, Example *e);
int test_oper(Program *p, char* c);

void test_parse()
{
  Program prog;
  Stack s;
  Example e;
  int i;

  prog.crt = 0;
  e.cnt = 0;
  for (i = 0; i < SIZE; i++) {
    e.array[i] = '\0';
  }
  /* Check that the stack pointer is not NULL */
  InitStack(&s);
  assert(&s != NULL);
  assert(s.top != NULL);

  /* Check that function Push() will push a string containing a
  character or number (first digit only) onto the stack */
  /* Check that function Pop() will pop the last element
  (only the first digit for numbers) off the stack */
  Push(&s, "B");
  assert(s.top->prev->code == 'B');
  assert(Pop(&s)=='B');

  Push(&s, "6");
  assert(s.top->prev->code == '6');
  assert(Pop(&s)=='6');

  Push(&s, "23.9");
  assert(s.top->prev->code == '2');
  assert(Pop(&s)=='2');

  /* Check that function Forward() only recognises "FD"
  instruction followed by <VARNUM> as correct grammar */
  assert(test_forward(&prog, "FD", "5")==1);
  assert(test_forward(&prog, "FT", "A")==0);
  /* Check that function LeftTurn() only recognises "LT"
  instruction followed by <VARNUM> as correct grammar */
  assert(test_leftturn(&prog, "LT", "23")==1);
  assert(test_leftturn(&prog, "ST", "72")==0);
  /* Check that function RightTurn() only recognises "RT"
  instruction followed by <VARNUM> as correct grammar */
  assert(test_rightturn(&prog, "RT", "H")==1);
  assert(test_rightturn(&prog, "4", "N")==0);

  /* Check that function DoLoop() parses the
  instructions stored in e.array as correct grammar */
  e.array[0] = "DO";
  e.array[1] = "E";
  e.array[2] = "FROM";
  e.array[3] = "3";
  e.array[4] = "TO";
  e.array[5] = "12";
  e.array[6] = "{";
  e.cnt = 7;
  assert(test_doloop(&prog, &s, &e)==1);

  /* Check that strings containing left and right
  curly brackets are recognised as such */
  assert(test_leftbrace(&prog, "{")==1);
  assert(test_leftbrace(&prog, "?")==0);
  assert(test_rightbrace(&prog, "}")==1);
  assert(test_rightbrace(&prog, "!")==0);

  /* Check that function VarNum() can distinguish
  between a string containing a number and a string
  containing an uppercase letter */
  assert(test_varnum(&prog, "3")==1);
  assert(test_varnum(&prog, "2.5")==1);
  assert(test_varnum(&prog, "A")==0);
  /* Check that function Var() converts a string
  containing an uppercase letter into a character */
  assert(test_var(&prog, "M")=='M');
  assert(test_var(&prog, "S")=='S');

  /* Check that function Set() parses the
  instructions stored in e.array as correct grammar */
  e.array[0] = "SET";
  e.array[1] = "L";
  e.array[2] = ":=";
  e.array[3] = ";";
  e.cnt = 4;
  assert(test_set(&prog, &s, &e)==1);

  /* Check that function Polish() parses the
  instructions stored in e.array as correct grammar */
  e.array[0] = "4";
  e.array[1] = "12";
  e.array[2] = "/";
  e.array[3] = ";";
  e.cnt = 4;
  assert(test_polish(&prog, &s, &e)==1);

  /* Check that function Oper() recognises the validity
  and type of operation */
  assert(test_oper(&prog, "*")==1);
  assert(test_oper(&prog, "F")==0);
  assert(test_oper(&prog, "-")==1);
  assert(test_oper(&prog, "3")==0);

  /* Check that stack memory has been freed */
  FreeStack(&s);
  assert(s.top == NULL);
}

int test_forward(Program *p, char* c, char* d)
{
  p->crt = 0;
  strcpy(p->inst[p->crt], c);
  strcpy(p->inst[p->crt+1], d);
  if (strsame(p->inst[p->crt], "FD")) {
    p->crt = p->crt + 1;
    return Forward(p);
  }
  return 0;
}

int test_leftturn(Program *p, char* c, char* d)
{
  p->crt = 0;
  strcpy(p->inst[p->crt], c);
  strcpy(p->inst[p->crt+1], d);
  if (strsame(p->inst[p->crt], "LT")) {
    p->crt = p->crt + 1;
    return LeftTurn(p);
  }
  return 0;
}

int test_rightturn(Program *p, char* c, char* d)
{
  p->crt = 0;
  strcpy(p->inst[p->crt], c);
  strcpy(p->inst[p->crt+1], d);
  if (strsame(p->inst[p->crt], "RT")) {
    p->crt = p->crt + 1;
    return RightTurn(p);
  }
  return 0;
}

int test_doloop(Program *p, Stack *s, Example *e)
{
  int i = 0;
  p->crt = 0;
  for (i = 0; i < e->cnt; i++) {
    strcpy(p->inst[p->crt+i], e->array[i]);
  }
  if (strsame(p->inst[p->crt], "DO")) {
    p->crt = p->crt + 1;
    return DoLoop(p, s);
  }
  return 0;
}

int test_leftbrace(Program *p, char* c)
{
  strcpy(p->inst[p->crt], c);
  if (strsame(p->inst[p->crt], "{")) {
    return 1;
  }
  return 0;
}

int test_rightbrace(Program *p, char* c)
{
  strcpy(p->inst[p->crt], c);
  if (strsame(p->inst[p->crt], "}")) {
    return 1;
  }
  return 0;
}

int test_varnum(Program *p, char* c)
{
  strcpy(p->inst[p->crt], c);
  return VarNum(p);
}

char test_var(Program *p, char* c)
{
  strcpy(p->inst[p->crt], c);
  return Var(p);
}

int test_set(Program *p, Stack *s, Example *e)
{
  int i;
  p->crt = 0;
  for (i = 0; i < e->cnt; i++) {
    strcpy(p->inst[p->crt+i], e->array[i]);
  }
  if (strsame(p->inst[p->crt], "SET")) {
    p->crt = p->crt + 1;
    return Set(p, s);
  }
  return 0;
}

int test_polish(Program *p, Stack *s, Example *e)
{
  int i;
  p->crt = 0;
  for (i = 0; i < e->cnt; i++) {
    strcpy(p->inst[p->crt+i], e->array[i]);
  }
  return Polish(p, s);
}

int test_oper(Program *p, char* c)
{
  strcpy(p->inst[p->crt], c);
  return Oper(p);
}
