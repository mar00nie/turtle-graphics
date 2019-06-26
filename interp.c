#include "interp.h"

int main(int argc, char** argv)
{
  int i;
  FILE *fp;
  Program prog;
  SDL_Simplewin w;
  Coords cd;
  Stack s;

  InitStack(&s);
  prog.num = 0;
  prog.crt = 0;
  s.cnt = 0;

  for (i = 0; i < MAXNUMTOKENS; i++) {
    prog.inst[i][0] = '\0';
  }
  if (argc != 2) {
    ERROR("No File Found");
  }
  if (!(fp = fopen(argv[1], "r"))) {
    fprintf(stderr, "Cannot open %s\n", argv[1]);
    exit(2);
  }
  /* Save each instruction from file in an array */
  i = 0;
  while (fscanf(fp, "%s", prog.inst[i++]) == 1 && \
  i < MAXNUMTOKENS) {
    prog.num = prog.num + 1;
  }
  fclose(fp);

  if (prog.num == 0) {
    ERROR("No Words Scanned");
  }
  for (i = 0; i < SIZE; i++) {
    cd.arr[i] = 0;
    cd.values[i] = 0;
  }
  /* Initialise SDL window and set line colour */
  Neill_SDL_Init(&w);
  Neill_SDL_SetDrawColour(&w, WHITE, WHITE, WHITE);
  /* Set initial coordinates */
  cd.x_new = X_INIT;
  cd.y_new = Y_INIT;
  cd.angle = 0;

  SDL_Delay(PAUSE);
  MainProg(&prog, &s, &w, &cd);

  SDL_Delay(PAUSE);
  SDL_Quit();
  atexit(SDL_Quit);
  FreeStack(&s);

  return 0;
}

void MainProg(Program *p, Stack *s, SDL_Simplewin *w, Coords *cd)
{
  /* Check if first instruction is "{" and push onto stack */
  if (!strsame(p->inst[p->crt], "{")) {
    ERROR("No Opening Bracket");
  }
  Push(s, p->inst[p->crt]);
  p->crt = p->crt + 1;
  Instrctlst(p, s, w, cd);
}

void Instrctlst(Program *p, Stack *s, SDL_Simplewin *w, Coords *cd)
{
  /* Check if instruction is "}" to pop last item "{" off the stack */
  if (strsame(p->inst[p->crt], "}")) {
    if (s->top != NULL) {
      if (Pop(s) == '{') {
        /* Check if it's the last "}" in the file */
        if (s->top->prev == NULL) {
          return;
        }
        else {
          p->crt = p->crt + 1;
          return;
        }
      }
    }
  }
  Instruction(p, s, w, cd);
  Instrctlst(p, s, w, cd);
}

int Instruction(Program *p, Stack *s, SDL_Simplewin *w, Coords *cd)
{
    /* Match parsed instruction with one of the following */
  if (strsame(p->inst[p->crt], "FD")) {
    p->crt = p->crt + 1;
    Forward(p, w, cd);
    return 1;
  }
  if (strsame(p->inst[p->crt], "LT")) {
    p->crt = p->crt + 1;
    LeftTurn(p, cd);
    return 1;
  }
  if (strsame(p->inst[p->crt], "RT")) {
    p->crt = p->crt + 1;
    RightTurn(p, cd);
    return 1;
  }
  if (strsame(p->inst[p->crt], "DO")) {
    p->crt = p->crt + 1;
    return DoLoop(p, s, w, cd);
  }
  if (strsame(p->inst[p->crt], "SET")) {
    p->crt = p->crt + 1;
    return Set(p, s, cd);
  }
  if (strsame(p->inst[p->crt], "}")) {
    return 1;
  }
  ERROR("Expecting an Instruction");
}

double Forward(Program *p, SDL_Simplewin *w, Coords *cd)
{
  double num;
  /* If instruction is a number */
  if (VarNum(p)) {
    num = VarDec(p);
    p->crt = p->crt + 1;
  }
  /* If instruction is an uppercase letter */
  else {
    char lett = Var(p);
    int val = lett - A;
    num = cd->arr[val];
    p->crt = p->crt + 1;
  }
  /* Set previous coordinates and calculate new coordinates based on angle */
  cd->x_prev = cd->x_new;
  cd->y_prev = cd->y_new;
  cd->x_new = cd->x_new + num*cos(cd->angle);
  cd->y_new = cd->y_new + num*sin(cd->angle);

  /* Draw line in SDL based on previous and new coordinates */
  SDL_RenderDrawLine(w->renderer, cd->x_prev, cd->y_prev, cd->x_new, cd->y_new);
  Neill_SDL_UpdateScreen(w);
  Neill_SDL_Events(w);
  SDL_Delay(WAIT_TIME);

  return num;
}

double LeftTurn(Program *p, Coords *cd)
{
  double num;
  /* If instruction is a number */
  if (VarNum(p)) {
    num = VarDec(p);
  }
  /* If instruction is an uppercase letter */
  else {
    char lett = Var(p);
    int val = lett - A;
    num = cd->arr[val];
  }
  /* Set new angle based on old angle */
  cd->angle = cd->angle - num * PI/DEG;
  p->crt = p->crt + 1;
  return num;
}

double RightTurn(Program *p, Coords *cd)
{
  double num;
  /* If instruction is a number */
  if (VarNum(p)) {
    num = VarDec(p);
  }
  /* If instruction is an uppercase letter */
  else {
    char lett = Var(p);
    int val = lett - A;
    num = cd->arr[val];
  }
  /* Set new angle based on old angle */
  cd->angle = cd->angle + num * PI/DEG;
  p->crt = p->crt + 1;
  return num;
}

int VarNum(Program *p)
{
  float num;
  char* str = p->inst[p->crt];
  /* If string contains a number */
  if (sscanf(str, "%f", &num)==1) {
    return 1;
  }
  /* If string contains an uppercase letter */
  if (isupper(str[0]) && strlen(str)==1) {
    return 0;
  }
  ERROR("Expecting a Number or Letter");
}

double VarDec(Program *p)
{
  /* Convert string to double type */
  double num = atof(p->inst[p->crt]);
  return num;
}

char Var(Program *p)
{
  /* Convert string to character type */
  char lett = *p->inst[p->crt];
  if ((lett >= A) && (lett <= Z)) {
    return lett;
  }
  ERROR("Expecting a Letter");
}

int Set(Program *p, Stack *s, Coords *cd)
{
  /* Check if SET instruction is valid and calculate value from <POLISH> expressions */
  char lett = Var(p);
  int val = lett - A;
  double calc;
  p->crt = p->crt + 1;
  if (strsame(p->inst[p->crt], ":=")) {
    p->crt = p->crt + 1;
    calc = Polish(p, s, cd);
    cd->arr[val] = calc;
    return 1;
  }
  ERROR("Expecting := Symbol");
}

double Polish(Program *p, Stack *s, Coords *cd)
{
  /* Return <POLISH> expression if string contains ";" */
  if (strsame(p->inst[p->crt], ";")) {
    double calc;
    if (s->cnt == 0) {
      ERROR("Expecting a Value");
    }
    Pop(s);
    s->cnt = s->cnt - 1;
    calc = cd->values[s->cnt];
    while (s->cnt > 0) {
      Pop(s);
      s->cnt = s->cnt - 1;
    }
    p->crt = p->crt + 1;
    return calc;
  }
  /* Calculate <POLISH> expression using values saved
  and referred to by the stack if string contains <OP> */
  if (Oper(p)) {
    int op;
    double calc;
    char buffer[SIZE];
    /* Check there are at least two values on the stack that
    refer to valid operands*/
    if (s->cnt < 2) {
      ERROR("Expecting Two Operands");
    }
    op = Oper(p);
    calc = Calc(s, cd, op);
    cd->values[s->cnt] = calc;
    sprintf(buffer, "%f", calc);
    Push(s, buffer);
    s->cnt = s->cnt + 1;
    p->crt = p->crt + 1;
    return Polish(p, s, cd);
  }
  /* If string contains a character, push onto stack and keep parsing */
  if (!VarNum(p)) {
    Push(s, p->inst[p->crt]);
    s->cnt = s->cnt + 1;
    p->crt = p->crt + 1;
    return Polish(p, s, cd);
  }
  /* If string contains a number, push onto stack and keep parsing */
  if (VarNum(p)) {
    double num = VarDec(p);
    cd->values[s->cnt] = num;
    Push(s, p->inst[p->crt]);
    s->cnt = s->cnt + 1;
    p->crt = p->crt + 1;
    return Polish(p, s, cd);
  }

  return 0;
}

int Oper(Program *p)
{
  /* Return a different integer based on <OP> type */
  if (strsame(p->inst[p->crt], "+")) {
    return 1;
  }
  if (strsame(p->inst[p->crt], "-")) {
    return 2;
  }
  if (strsame(p->inst[p->crt], "*")) {
    return 3;
  }
  if (strsame(p->inst[p->crt], "/")) {
    return 4;
  }
  return 0;
}

double Calc(Stack *s, Coords *cd, int op)
{
  /* Arithmetic calculation of two operands and an operator */
  double result;
  double num2 = Convert(s, cd);
  double num1 = Convert(s, cd);

  switch (op) {
    case 1:
      result = num1 + num2;
      return result;
    case 2:
      result = num1 - num2;
      return result;
    case 3:
      result = num1 * num2;
      return result;
    case 4:
      result = num1 / num2;
      return result;
  }
  return 0;
}

double Convert(Stack *s, Coords *cd)
{
  /* Retrieve the value saved by popping off its reference from the stack */
  char lett[SIZE];
  lett[0] = Pop(s);
  lett[1] = '\0';
  s->cnt = s->cnt - 1;

  if (atoi(lett)) {
    double num = cd->values[s->cnt];
    return num;
  }
  if ((lett[0] >= A) && (lett[0] <= Z)) {
    int val = lett[0] - A;
    double num = cd->arr[val];
    return num;
  }
  return 0;
}

int DoLoop(Program *p, Stack *s, SDL_Simplewin *w, Coords *cd)
{
  /* Check if DO instruction is valid */
  int row;
  double num1, num2, min, max;
  char lett = Var(p);
  int val = lett - A;
  char letter;
  int value;
  p->crt = p->crt + 1;

  if (strsame(p->inst[p->crt], "FROM")) {
    p->crt = p->crt + 1;
    if (VarNum(p)) {
      num1 = VarDec(p);
      min = num1;
      cd->arr[val] = min;
    }
    else {
      letter = Var(p);
      value = letter - A;
      min = cd->arr[value];
      cd->arr[val] = min;
    }

    p->crt = p->crt + 1;
    if (strsame(p->inst[p->crt], "TO")) {
      p->crt = p->crt + 1;
      if (VarNum(p)) {
        num2 = VarDec(p);
        max = num2;
      }
      else {
        letter = Var(p);
        value = letter - A;
        max = cd->arr[value];
      }

      p->crt = p->crt + 1;
      if (strsame(p->inst[p->crt], "{")) {
        Push(s, p->inst[p->crt]);
        p->crt = p->crt + 1;
        row = p->crt;

        /* If FROM value < TO value, iterate through the contents after "{" and
        increase the FROM value by 1 after a "}" is encountered each time */
        while (cd->arr[val] <= max) {
          Instrctlst(p, s, w, cd);
          if (strsame(p->inst[p->crt], "}")) {
            cd->arr[val] = cd->arr[val] + 1;
            if (cd->arr[val] <= max) {
              p->crt = row;
              Push(s, "{");
            }
          }
        }
        return 1;
      }
      ERROR("Expecting an Opening Bracket");
    }
    ERROR("Expecting a TO");
  }
  ERROR("Expecting a FROM");
}

void InitStack(Stack *s)
{
  /* Initialise stack */
  s->top = (Elem *) calloc(1, sizeof(Elem));
  if (s->top == NULL) {
    ERROR("Creation of Stack Failed");
  }
  s->top->prev = NULL;
}

void Push(Stack *s, char* c)
{
  /* Push a string onto the stack */
  Elem *e;
  e = (Elem *) calloc(1, sizeof(Elem));
  if (e == NULL) {
    ERROR("Creation of Stack Element Failed");
  }
  e->prev = s->top;
  s->top->code = *c;
  s->top = e;
}

char Pop(Stack *s)
{
  /* Pop the last character off the stack */
  free(s->top);
  s->top = s->top->prev;
  if (s->top != NULL) {
    return s->top->code;
  }
  return '\0';
}

void FreeStack(Stack *s)
{
  /* Free all stack memory */
  if (s != NULL) {
    while (s->top != NULL) {
      free(s->top);
      s->top = s->top->prev;
    }
  }
}
