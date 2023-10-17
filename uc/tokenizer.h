#ifndef UC_TOKENIZER_H
#define UC_TOKENIZER_H

#include <stdio.h>
#include <stdint.h>


#define TOKEN_SIZE_MAX 32

enum TokenType {
  TOK_NAME,
  TOK_INT,
  TOK_SEMICOLON,
  TOK_EQUALS,
  TOK_PLUS,
  TOK_WHITE
};

struct Token
{
  enum TokenType type;
  char cvalue[TOKEN_SIZE_MAX];
  uint32_t ivalue;
  struct Token *next;
};


int tokenize(FILE *inFile, struct Token **outTokens);
void dump_tokens(struct Token* tokens);

#endif