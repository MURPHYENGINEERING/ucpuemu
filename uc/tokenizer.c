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
  // Don't absorb a nonalphanumeric char if we saw one
  if (c != EOF && !isalpha(c)) {
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
  // Don't absorb the EOF if we saw it
  if (c != EOF) {
    ungetc(c, inFile);
  }
  *outVal = atoi(buf);
  return i;
}


static void skip_white(FILE *inFile)
{
  int c = fgetc(inFile);
  while (c != EOF && (c == ' ' || c == '\n')) {
    c = fgetc(inFile);
  }
  // Don't absorb the EOF if we saw it
  if (c != EOF) {
    ungetc(c, inFile);
  }
}


static struct Token *read_token(FILE *inFile)
{
  int c = fgetc(inFile);
  if (c == EOF) {
    return 0;
  }
  ungetc(c, inFile);

  struct Token *t = (struct Token*) malloc(sizeof(struct Token));
  memset(t, 0, sizeof(*t));

  if (c == ' ' || c == '\n') {
    t->type = TOK_WHITE;
    skip_white(inFile);
  } else if (c == '=') {
    t->type = TOK_EQUALS;
    fgetc(inFile);
  } else if (c == '+') {
    t->type = TOK_PLUS;
    fgetc(inFile);
  } else if (c == ';') {
    t->type = TOK_SEMICOLON;
    fgetc(inFile);
  } else if (isalpha(c)) {
    t->type = TOK_NAME;
    read_name(inFile, t->cvalue, TOKEN_SIZE_MAX);
  } else if (isdigit(c)) {
    t->type = TOK_INT;
    read_int(inFile, &t->ivalue);
  } else {
    printf("! Unknown token: %c\n", c);
    return 0;
  }

  return t;
}


int tokenize(FILE *inFile, struct Token **outTokens)
{ 
  *outTokens = read_token(inFile);
  struct Token *t = *outTokens;
  while (t) {
    t->next = read_token(inFile);
    t = t->next;
  }

  return 0;
}


void dump_tokens(struct Token *tokens)
{
  printf("\n");
  while (tokens) {
    switch (tokens->type) {
      case TOK_NAME:
        printf("%s ", tokens->cvalue);
        break;
      case TOK_EQUALS:
        printf("= ");
        break;
      case TOK_PLUS:
        printf("+ ");
        break;
      case TOK_INT:
        printf("%d ", tokens->ivalue);
        break;
      case TOK_SEMICOLON:
        printf(";\n");
        break;
      case TOK_WHITE:
        break;
      default:
        printf(" <unknown> ");
        break;
    }
    tokens = tokens->next;
  }
  printf("\n");
}