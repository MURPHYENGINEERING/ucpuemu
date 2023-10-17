#ifndef UC_TOKENIZER_H
#define UC_TOKENIZER_H

#include <stdio.h>


enum TokenType {
  TOK_NAME,
  TOK_INT,
  TOK_SEMICOLON,
  TOK_EQUALS
};

struct Token
{
  enum TokenType type;
  struct Token *next;
};


int tokenize(FILE *inFile, struct Token **outTokens);
void dump_tokens(struct Token* tokens);

#endif