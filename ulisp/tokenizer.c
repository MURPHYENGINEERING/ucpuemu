#include "tokenizer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


static size_t read_name(FILE *inFile, char* outName, size_t maxLen)
{
  size_t i = 0;
  int c = fgetc(inFile);
  while (c != EOF && isalpha(c) && i < maxLen) {
    outName[i++] = c;
    c = fgetc(inFile);
  }
  // Don't absorb the first non-alpha char
  if (!isalpha(c)) {
    ungetc(c, inFile); 
  }

  return i;
}


static size_t read_int(FILE *inFile, uint32_t *outVal)
{
  char buf[TOKEN_SIZE_MAX];
  memset(buf, 0, TOKEN_SIZE_MAX);

  size_t i = 0;
  int c = fgetc(inFile);
  while (c != EOF && isdigit(c) && i < TOKEN_SIZE_MAX-1) {
    buf[i++] = c;
    c = fgetc(inFile);
  }
  // Don't absorb the first non-digit char
  ungetc(c, inFile);

  *outVal = atoi(buf);
  return i;
}


static void skip_white(FILE *inFile)
{
  int c = fgetc(inFile);
  while (c != EOF && (c == ' ' || c == '\n')) {
    c = fgetc(inFile);
  }
  // Don't absorb the first non-white char
  ungetc(c, inFile);
}


static struct Token *read_token(FILE *inFile)
{
  int c = fgetc(inFile);
  if (c == EOF) {
    return NULL;
  }
  ungetc(c, inFile);

  struct Token *t = (struct Token*) malloc(sizeof(struct Token));
  memset(t, 0, sizeof(*t));

  switch (c) {
  case '=':  
    t->type = TOK_EQUALS;       
    strcpy(t->cvalue, "=");
    break;
  case '+':  
    t->type = TOK_PLUS;         
    strcpy(t->cvalue, "+");
    break;
  case '-':  
    t->type = TOK_MINUS;        
    break;
  case '*':  
    t->type = TOK_TIMES;        
    break;
  case '/':  
    t->type = TOK_DIVIDE;       
    break;
  case '^':  
    t->type = TOK_XOR;          
    break;
  case '|':  
    t->type = TOK_OR;           
    break;
  case '&':  
    t->type = TOK_AND;          
    break;
  case '%':  
    t->type = TOK_MOD;          
    break;
  case '~':  
    t->type = TOK_INVERT;       
    break;
  case '.':  
    t->type = TOK_DOT;          
    break;
  case '(':  
    t->type = TOK_OPENPAREN;    
    break;
  case ')':  
    t->type = TOK_CLOSEPAREN;   
    break;
  case '"':  
    t->type = TOK_DOUBLEQUOTE;  
    break;
  }

  if (t->type != TOK_NONE) {
    // Skip a single char token
    fgetc(inFile);
  } else {
    // Not a single char token
    if (c == ' ' || c == '\n') {
      t->type = TOK_WHITE;
      skip_white(inFile);
    } else if (isalpha(c)) {
      t->type = TOK_NAME;
      read_name(inFile, t->cvalue, TOKEN_SIZE_MAX);
    } else if (isdigit(c)) {
      t->type = TOK_CONST;
      read_int(inFile, &t->ivalue);
    } else {
      printf("! Unknown token: %c\n", c);
      return NULL;
    }
  }

  return t;
}


int tokenize(FILE *inFile, struct Token **outTokens)
{ 
  *outTokens = read_token(inFile);
  struct Token *t = *outTokens;
  while (t) {
    struct Token *newToken = read_token(inFile);
    if (!newToken) {
      break;
    }
    if (newToken->type != TOK_WHITE) {
      t->next = newToken;
      t = t->next;
    }
  }

  return 0;
}


void tokens_dump(struct Token *tokens)
{
  printf("\n");
  while (tokens) {
    switch (tokens->type) {
      case TOK_NAME:
        printf("%s", tokens->cvalue);
        break;
      case TOK_EQUALS:
        printf("=");
        break;
      case TOK_PLUS:
        printf("+");
        break;
      case TOK_CONST:
        printf("%d", tokens->ivalue);
        break;
      case TOK_OPENPAREN:
        printf("(");
        break;
      case TOK_CLOSEPAREN:
        printf(")");
        break;
      case TOK_WHITE:
        break;
      default:
        printf(" <unknown> ");
        break;
    }
    tokens = tokens->next;
    printf(" ");
  }
  printf("\n");
}