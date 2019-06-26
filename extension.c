#include "extension.h"

int main(int argc, char** argv)
{
  int i;
  FILE *fp;
  Program prog;
  SDL_Simplewin w;
  Coords cd;
  Node *head;
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
  /* Set initial coordinates */
  cd.x_new = X_INIT;
  cd.y_new = Y_INIT;
  cd.angle = 0;
  /* Set default colour to white */
  for (i = 0; i < RGBCODES; i++) {
    cd.rgb[i] = RGB2;
  }
  /* Initialise head node */
  head = AllocateNode(&cd);
  MainProg(&prog, &s, head, &cd);

  Neill_SDL_Init(&w);
  SDL_Delay(PAUSE);
  /* Draw all lines using SDL after all parsing is done */
  RunCompiler(&w, head);
  SDL_Delay(PAUSE);
  SDL_Quit();
  atexit(SDL_Quit);

  FreeStack(&s);
  FreeNode(&head);

  return 0;
}

void MainProg(Program *p, Stack *s, Node *head, Coords *cd)
{
  if (!strsame(p->inst[p->crt], "{")) {
    ERROR("No Opening Bracket");
  }
  Push(s, p->inst[p->crt]);
  p->crt = p->crt + 1;
  Instrctlst(p, s, head, cd);
}

void Instrctlst(Program *p, Stack *s, Node *head, Coords *cd)
{
  if (strsame(p->inst[p->crt], "}")) {
    if (s->top != NULL) {
      if (Pop(s) == '{') {
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
  Instruction(p, s, head, cd);
  Instrctlst(p, s, head, cd);
}

int Instruction(Program *p, Stack *s, Node *head, Coords *cd)
{
  if (strsame(p->inst[p->crt], "FD")) {
    p->crt = p->crt + 1;
    Forward(p, head, cd);
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
    return DoLoop(p, s, head, cd);
  }
  if (strsame(p->inst[p->crt], "SET")) {
    p->crt = p->crt + 1;
    return Set(p, s, cd);
  }
  /* If current instruction is "COLOUR", run Colour() function for
  the next instruction */
  if (strsame(p->inst[p->crt], "COLOUR")) {
    p->crt = p->crt + 1;
    return Colour(p, head, cd);
  }
  if (strsame(p->inst[p->crt], "}")) {
    return 1;
  }
  ERROR("Expecting an Instruction");
}

double Forward(Program *p, Node *head, Coords *cd)
{
  Node *current = head;
  double num;

  if (VarNum(p)) {
    num = VarDec(p);
    p->crt = p->crt + 1;
  }
  else {
    char lett = Var(p);
    int val = lett - A;
    num = cd->arr[val];
    p->crt = p->crt + 1;
  }
  cd->x_prev = cd->x_new;
  cd->y_prev = cd->y_new;
  cd->x_new = cd->x_new + num*cos(cd->angle);
  cd->y_new = cd->y_new + num*sin(cd->angle);

  /* Save current x and y coordinates in the next
  node linked to the current node  */
  while (current->next != NULL) {
    current = current->next;
  }
  current->next = AllocateNode(cd);

  return num;
}

double LeftTurn(Program *p, Coords *cd)
{
  double num;
  if (VarNum(p)) {
    num = VarDec(p);
  }
  else {
    char lett = Var(p);
    int val = lett - A;
    num = cd->arr[val];
  }
  cd->angle = cd->angle - num * PI/DEG;
  p->crt = p->crt + 1;
  return num;
}

double RightTurn(Program *p, Coords *cd)
{
  double num;
  if (VarNum(p)) {
    num = VarDec(p);
  }
  else {
    char lett = Var(p);
    int val = lett - A;
    num = cd->arr[val];
  }
  cd->angle = cd->angle + num * PI/DEG;
  p->crt = p->crt + 1;
  return num;
}

int VarNum(Program *p)
{
  float num;
  char* str = p->inst[p->crt];
  if (sscanf(str, "%f", &num)==1) {
    return 1;
  }
  if (isupper(str[0]) && strlen(str)==1) {
    return 0;
  }
  ERROR("Expecting a Number or Letter");
}

double VarDec(Program *p)
{
  double num = atof(p->inst[p->crt]);
  return num;
}

char Var(Program *p)
{
  char lett = *p->inst[p->crt];
  if ((lett >= A) && (lett <= Z)) {
    return lett;
  }
  ERROR("Expecting a Letter");
}

int Set(Program *p, Stack *s, Coords *cd)
{
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

  if (Oper(p)) {
    int op;
    double calc;
    char buffer[SIZE];
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

  if (!VarNum(p)) {
    Push(s, p->inst[p->crt]);
    s->cnt = s->cnt + 1;
    p->crt = p->crt + 1;
    return Polish(p, s, cd);
  }

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

int DoLoop(Program *p, Stack *s, Node *head, Coords *cd)
{
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

        while (cd->arr[val] <= max) {
          Instrctlst(p, s, head, cd);
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

int Colour(Program *p, Node *n, Coords *cd)
{
  int i;
  /* Set a random colour for the node */
  if (strsame(p->inst[p->crt], "RDM")) {
    for (i = 0; i < RGBCODES; i++) {
      n->rgb[i] = cd->rgb[i] = rand()%SDL_8BITCOLOUR;
    }
    p->crt = p->crt + 1;
  }
  else {
    /* Set the RGB values for the specified colour */
    for (i = 0; i < RGBCODES; i++) {
      if (strsame(p->inst[p->crt], "0")) {
        n->rgb[i] = cd->rgb[i] = RGB0;
      }
      else if (strsame(p->inst[p->crt], "1")) {
        n->rgb[i] = cd->rgb[i] = RGB1;
      }
      else if (strsame(p->inst[p->crt], "2")) {
        n->rgb[i] = cd->rgb[i] = RGB2;
      }
      p->crt = p->crt + 1;
    }
  }

  return 1;
}

void InitStack(Stack *s)
{
  s->top = (Elem *) calloc(1, sizeof(Elem));
  if (s->top == NULL) {
    ERROR("Creation of Stack Failed");
  }
  s->top->prev = NULL;
}

void Push(Stack *s, char* c)
{
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
  free(s->top);
  s->top = s->top->prev;
  if (s->top != NULL) {
    return s->top->code;
  }
  return '\0';
}

void FreeStack(Stack *s)
{
  if (s != NULL) {
    while (s->top != NULL) {
      free(s->top);
      s->top = s->top->prev;
    }
  }
}

void RunCompiler(SDL_Simplewin *w, Node *head)
{
  /* Iterate through each node starting from the head node
  and draw a line based on the coordinates of the current
  node and the coordinates of the next node */
  Node *current = head;

  while (current->next != NULL) {
    /* Draw line in colour specified by current node. */
    Neill_SDL_SetDrawColour(w, current->rgb[0],
        current->rgb[1], current->rgb[2]);
    SDL_RenderDrawLine(w->renderer, current->x, \
      current->y, current->next->x, current->next->y);
    Neill_SDL_UpdateScreen(w);
    Neill_SDL_Events(w);
    SDL_Delay(WAIT_TIME);

    current = current->next;
  }
}

Node* AllocateNode(Coords *cd)
{
  int i;
  Node *n;
  /* Initialise node and set x and y values */
  n = (Node *) malloc(sizeof(Node));

  if (n == NULL) {
    ERROR("Cannot Allocate Node");
  }
  n->x = cd->x_new;
  n->y = cd->y_new;

  /* Set RGB values for node based on specified colour */
  for (i = 0; i < RGBCODES; i++) {
    n->rgb[i] = cd->rgb[i];
  }
  n->next = NULL;

  return n;
}

void FreeNode(Node **n)
{
  /* Free all memory of linked list */
  if (n != NULL) {
    Node * p = *n;
    if (p->next != NULL) {
      FreeNode(&p->next);
    }
    free(p);
    *n = NULL;
  }
}
