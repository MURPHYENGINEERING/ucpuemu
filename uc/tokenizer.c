#include "tokenizer.h"
#include <stdlib.h>


int tokenize(FILE *inFile, struct Token **outTokens)
{
  *outTokens = (struct Token*) malloc(sizeof(**outTokens));
  (*outTokens)->type = TOK_NAME;

  return 0;
}

void dump_tokens(struct Token* tokens)
{
  while (tokens) {

  }
}