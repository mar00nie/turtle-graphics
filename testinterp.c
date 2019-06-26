#include "interp.h"

struct example {
  char* array[SIZE];
  int cnt;
};
typedef struct example Example;

int test_forward(Program *p, SDL_Simplewin *w, Coords *cd, char* c, char* d);
int test_leftturn(Program *p, Coords *cd, char* c, char* d);
int test_rightturn(Program *p, Coords *cd, char* c, char* d);
int test_doloop(Program *p, Stack *s, SDL_Simplewin *w, Coords *cd, Example *e);
int test_leftbrace(Program *p, char* c);
int test_rightbrace(Program *p, char* c);
int test_varnum(Program *p, char* c);
int test_vardec(Program *p, char* c);
char test_var(Program *p, char* c);
int test_set(Program *p, Stack *s, Coords *cd, Example *e);
int test_polish(Program *p, Stack *s, Coords *cd, Example *e);
int test_oper(Program *p, char *c);
int test_calc(Stack *s, Coords *cd, Example *e, int op);
int test_convert(Stack *s, Coords *cd, char* c);
int check_zero(double num);

void test_interp()
{
  Program prog;
  SDL_Simplewin w;
  Stack s;
  Coords cd;
  Example e;
  int i;

  prog.crt = 0;
  e.cnt = 0;

  for (i = 0; i < SIZE; i++) {
    cd.arr[i] = 0;
    cd.values[i] = 0;
    e.array[i] = '\0';
  }

  Neill_SDL_Init(&w);
  Neill_SDL_SetDrawColour(&w, WHITE, WHITE, WHITE);

  /* Check that the stack pointer is not NULL */
  InitStack(&s);
  assert(&s != NULL);
  assert(s.top != NULL);

  /* Check that function Push() will push a string
  containing a character or number (first digit only) onto the stack */
  /* Check that function Pop() will pop the last
  element (only the first digit for numbers) off the stack */
  Push(&s, "D");
  assert(s.top->prev->code == 'D');
  assert(Pop(&s)=='D');

  Push(&s, "7.52");
  assert(s.top->prev->code == '7');
  assert(Pop(&s)=='7');

  Push(&s, "81");
  assert(s.top->prev->code == '8');
  assert(Pop(&s)=='8');

  cd.angle = 90;
  cd.x_new = 400;
  cd.y_new = 300;
  /* Check that function Forward() with the correct
  grammar will correctly set the new x and y coordinates */
  assert(test_forward(&prog, &w, &cd, "FD", "45")==1);
  assert(test_forward(&prog, &w, &cd, "T", "120")==0);

  cd.angle = 125;
  cd.x_new = 280;
  cd.y_new = 150;
  cd.arr['F'-A] = 25;
  assert(test_forward(&prog, &w, &cd, "FD", "F")==1);
  assert(test_forward(&prog, &w, &cd, "33", "7")==0);

  /* Check that function LeftTurn() with the correct
  grammar will correctly set the new angle */
  cd.angle = 0;
  assert(test_leftturn(&prog, &cd, "LT", "90")==1);
  cd.angle = 20;
  cd.arr['Q'-A] = 67;
  assert(test_leftturn(&prog, &cd, "LT", "Q")==1);
  cd.angle = 30;
  assert(test_leftturn(&prog, &cd, "WT", "20")==0);

  /* Check that function RightTurn() with the correct
  grammar will correctly set the new angle */
  cd.angle = 33;
  assert(test_rightturn(&prog, &cd, "RT", "45")==1);
  cd.angle = 85;
  cd.arr['J'-A] = 10;
  assert(test_rightturn(&prog, &cd, "RT", "J")==1);
  cd.angle = 53;
  assert(test_rightturn(&prog, &cd, "3", "2")==0);

  /* Check that function DoLoop() parses the
  instructions stored in e.array as correct grammar */
  e.array[0] = "DO";
  e.array[1] = "S";
  e.array[2] = "FROM";
  e.array[3] = "A";
  e.array[4] = "TO";
  e.array[5] = "18";
  e.array[6] = "{";
  e.array[7] = "}";
  e.cnt = 8;
  assert(test_doloop(&prog, &s, &w, &cd, &e)==1);

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
  /* Check that function VarDec() converts a string
  containing a number into a number of type double */
  assert(test_vardec(&prog, "4.5")==1);
  assert(test_vardec(&prog, "60")==1);
  /* Check that function Var() converts a string
  containing an uppercase letter into a character */
  assert(test_var(&prog, "J")=='J');

  /* Check that function Set() with the correct
  grammar returns a calculation result */
  e.array[0] = "SET";
  e.array[1] = "K";
  e.array[2] = ":=";
  e.array[3] = "34";
  e.array[4] = "5";
  e.array[5] = "-";
  e.array[6] = ";";
  e.cnt = 7;
  assert(test_set(&prog, &s, &cd, &e)==1);

  /* Check that function Polish() with the correct
  grammar returns a calculation result */
  e.array[0] = "C";
  e.array[1] = "2";
  e.array[2] = "+";
  e.array[3] = ";";
  e.cnt = 4;
  assert(test_polish(&prog, &s, &cd, &e)==1);

  /* Check that function Oper() returns the correct
  integer based on the operation */
  assert(test_oper(&prog, "*")==3);
  assert(test_oper(&prog, "F")==0);
  assert(test_oper(&prog, "-")==2);
    assert(test_oper(&prog, "$")==0);

  /* Check that function Calc() returns the correct
  result of an arithmetic operation using the last
  two items from the stack as operands */
  s.cnt = 0;
  e.array[0] = "R";
  cd.arr['R'-A] = 22;
  e.array[1] = "7.5";
  cd.values[1] = 7.5;
  assert(test_calc(&s, &cd, &e, 2)==1);

  /* Check that function Convert() correctly
  converts the last element popped off the stack
  into a number or uppercase letter */
  s.cnt = 0;
  cd.values[0] = 3;
  assert(test_convert(&s, &cd, "3")==1);
  cd.arr['W'-A] = 26;
  assert(test_convert(&s, &cd, "W")==1);

  /* Check that stack memory has been freed */
  FreeStack(&s);
  assert(s.top == NULL);

  SDL_Quit();
  atexit(SDL_Quit);
}

int test_forward(Program *p, SDL_Simplewin *w, Coords *cd, char* c, char* d)
{
  double num, diff_x, diff_y;
  p->crt = 0;
  strcpy(p->inst[p->crt], c);
  strcpy(p->inst[p->crt+1], d);
  if (strsame(p->inst[p->crt], "FD")) {
    p->crt = p->crt + 1;
    num = Forward(p, w, cd);
    diff_x = cd->x_new - (cd->x_prev + num*cos(cd->angle));
    diff_y = cd->y_new - (cd->y_prev + num*sin(cd->angle));
    if ((check_zero(diff_x)==1) && (check_zero(diff_y)==1)) {
      return 1;
    }
  }
  return 0;
}

int test_leftturn(Program *p, Coords *cd, char* c, char* d)
{
  double num, diff;
  double angle = cd->angle;
  p->crt = 0;
  strcpy(p->inst[p->crt], c);
  strcpy(p->inst[p->crt+1], d);
  if (strsame(p->inst[p->crt], "LT")) {
    p->crt = p->crt + 1;
    num = LeftTurn(p, cd);
    diff = cd->angle - (angle-num*PI/DEG);
    if (check_zero(diff)==1) {
      return 1;
    }
  }
  return 0;
}

int test_rightturn(Program *p, Coords *cd, char* c, char* d)
{
  double num, diff;
  double angle = cd->angle;
  p->crt = 0;
  strcpy(p->inst[p->crt], c);
  strcpy(p->inst[p->crt+1], d);
  if (strsame(p->inst[p->crt], "RT")) {
    p->crt = p->crt + 1;
    num = RightTurn(p, cd);
    diff = cd->angle - (angle+num*PI/DEG);
    if (check_zero(diff)==1) {
      return 1;
    }
  }
  return 0;
}

int test_doloop(Program *p, Stack *s, SDL_Simplewin *w, Coords *cd, Example *e)
{
  int i;
  p->crt = 0;
  for (i = 0; i < e->cnt; i++) {
    strcpy(p->inst[p->crt+i], e->array[i]);
  }
  if (strsame(p->inst[p->crt], "DO")) {
    p->crt = p->crt + 1;
    return DoLoop(p, s, w, cd);
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

int test_vardec(Program *p, char* c)
{
  double diff;
  strcpy(p->inst[p->crt], c);
  diff = VarDec(p)-atof(c);
  if (check_zero(diff)==1) {
    return 1;
  }
  return 0;
}

char test_var(Program *p, char* c)
{
  strcpy(p->inst[p->crt], c);
  return Var(p);
}

int test_set(Program *p, Stack *s, Coords *cd, Example *e)
{
  int i;
  p->crt = 0;
  for (i = 0; i < e->cnt; i++) {
    strcpy(p->inst[p->crt+i], e->array[i]);
  }
  if (strsame(p->inst[p->crt], "SET")) {
    p->crt = p->crt + 1;
    return Set(p, s, cd);
  }
  return 0;
}

int test_polish(Program *p, Stack *s, Coords *cd, Example *e)
{
  int i;
  double calc;
  p->crt = 0;
  for (i = 0; i < e->cnt; i++) {
    strcpy(p->inst[p->crt+i], e->array[i]);
  }
  calc = Polish(p, s, cd);
  return !check_zero(calc);
}

int test_oper(Program *p, char *c)
{
  strcpy(p->inst[p->crt], c);
  return Oper(p);
}

int test_calc(Stack *s, Coords *cd, Example *e, int op)
{
  int val;
  double result, diff, num1, num2;
  double value = 0;
  Push(s, e->array[0]);
  s->cnt = s->cnt + 1;
  Push(s, e->array[1]);
  s->cnt = s->cnt + 1;

  result = Calc(s, cd, op);

  if (atoi(e->array[0])) {
    num1 = cd->values[s->cnt];
  }
  else if ((e->array[0][0] >= A) && (e->array[0][0] <= Z)) {
    val = e->array[0][0] - A;
    num1 = cd->arr[val];
  }
  else {
    num1 = 0;
  }
  s->cnt = s->cnt + 1;

  if (atoi(e->array[1])) {
    num2 = cd->values[s->cnt];
  }
  else if ((e->array[1][0] >= A) && (e->array[1][0] <= Z)) {
    val = e->array[1][0] - A;
    num2 = cd->arr[val];
  }
  else {
    num2 = 0;
  }

  switch (op) {
    case 1:
      value = num1 + num2;
      break;
    case 2:
      value = num1 - num2;
      break;
    case 3:
      value = num1 * num2;
      break;
    case 4:
      value = num2 / num2;
      break;
  }

  diff = result - value;
  return check_zero(diff);
}

int test_convert(Stack *s, Coords *cd, char* c)
{
  int val;
  double number, diff;
  double value = 0;
  Push(s, c);
  s->cnt = s->cnt + 1;

  number = Convert(s, cd);

  if (atoi(c)) {
     value = cd->values[s->cnt];
  }
  else if ((c[0] >= A) && (c[0] <= Z)) {
    val = c[0] - A;
    value = cd->arr[val];
  }
  diff = number - value;
  return check_zero(diff);
}

int check_zero(double num)
{
  if (abs(num) < FLT_EPSILON) {
    return 1;
  }
  return 0;
}
