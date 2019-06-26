#include "parse.h"

int main(int argc, char** argv)
{
  int i;
  FILE *fp;
  Program prog;
  Stack s;

  /* Run test file */
  test_parse();

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

  MainProg(&prog, &s);
  FreeStack(&s);

  return 0;
}

void MainProg(Program *p, Stack *s)
{
  /* Check if first instruction is "{" and push onto stack */
  if (!strsame(p->inst[p->crt], "{")) {
    ERROR("No Opening Bracket");
  }
  Push(s, p->inst[p->crt]);
  p->crt = p->crt + 1;
  Instrctlst(p, s);
}

void Instrctlst(Program *p, Stack *s)
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
        }
      }
    }
  }
  Instruction(p, s);
  Instrctlst(p, s);
}

int Instruction(Program *p, Stack *s)
{
  /* Match parsed instruction with one of the following */
  if (strsame(p->inst[p->crt], "FD")) {
    p->crt = p->crt + 1;
    return Forward(p);
  }
  if (strsame(p->inst[p->crt], "LT")) {
    p->crt = p->crt + 1;
    return LeftTurn(p);
  }
  if (strsame(p->inst[p->crt], "RT")) {
    p->crt = p->crt + 1;
    return RightTurn(p);
  }
  if (strsame(p->inst[p->crt], "DO")) {
    p->crt = p->crt + 1;
    return DoLoop(p, s);
  }
  if (strsame(p->inst[p->crt], "SET")) {
    p->crt = p->crt + 1;
    Set(p, s);
    return 1;
  }
  if (strsame(p->inst[p->crt], "}")) {
    return 1;
  }
  ERROR("Expecting an Instruction");
}

int Forward(Program *p)
{
  /* If instruction is a number */
  if (VarNum(p)) {
    p->crt = p->crt + 1;
    return 1;
  }
  /* If instruction is an uppercase letter */
  if (Var(p)) {
    p->crt = p->crt + 1;
    return 1;
  }
  return 0;
}

int LeftTurn(Program *p)
{
  /* If instruction is a number */
  if (VarNum(p)) {
    p->crt = p->crt + 1;
    return 1;
  }
  /* If instruction is an uppercase letter */
  if (Var(p)) {
    p->crt = p->crt + 1;
    return 1;
  }
  return 0;
}

int RightTurn(Program *p)
{
  /* If instruction is a number */
  if (VarNum(p)) {
    p->crt = p->crt + 1;
    return 1;
  }
  /* If instruction is an uppercase letter */
  if (Var(p)) {
    p->crt = p->crt + 1;
    return 1;
  }
  return 0;
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

char Var(Program *p)
{
  /* Convert string to character type */
  char lett = *p->inst[p->crt];
  if ((lett >= A) && (lett <= Z)) {
    return lett;
  }
  ERROR("Expecting a Letter");
}

int Set(Program *p, Stack *s)
{
  /* Check if SET instruction is valid */
  p->crt = p->crt + 1;
  if (strsame(p->inst[p->crt], ":=")) {
    p->crt = p->crt + 1;
    return Polish(p, s);
  }
  ERROR("Expecting := Symbol");
}

int Polish(Program *p, Stack *s)
{
  /* Check if <POLISH> expressions are valid */
  if (strsame(p->inst[p->crt], ";")) {
    p->crt = p->crt + 1;
    return 1;
  }
  if (Oper(p)) {
    p->crt = p->crt + 1;
    return Polish(p, s);
  }
  if (!VarNum(p)) {
    p->crt = p->crt + 1;
    return Polish(p, s);
  }
  if (VarNum(p)) {
    p->crt = p->crt + 1;
    return Polish(p, s);
  }
  return 0;
}

int Oper(Program *p)
{
  /* Check if <OPER> instruction is valid */
  if (strsame(p->inst[p->crt], "+")) {
    return 1;
  }
  if (strsame(p->inst[p->crt], "-")) {
    return 1;
  }
  if (strsame(p->inst[p->crt], "*")) {
    return 1;
  }
  if (strsame(p->inst[p->crt], "/")) {
    return 1;
  }
  return 0;
}

int DoLoop(Program *p, Stack *s)
{
  /* Check if DO instruction is valid */
  p->crt = p->crt + 1;
  if (strsame(p->inst[p->crt], "FROM")) {
    p->crt = p->crt + 1;
    if (VarNum(p) || Var(p)) {
      p->crt = p->crt + 1;
    }
    if (strsame(p->inst[p->crt], "TO")) {
      p->crt = p->crt + 1;
      if (VarNum(p) || Var(p)) {
        p->crt = p->crt + 1;
      }
      if (strsame(p->inst[p->crt], "{")) {
        Push(s, p->inst[p->crt]);
        p->crt = p->crt + 1;
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

void Push(Stack *s, char *c)
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
