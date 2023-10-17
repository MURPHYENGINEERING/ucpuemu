#ifndef UC_TOKENIZER_H
#define UC_TOKENIZER_H

#include <stdio.h>
#include <stdint.h>


#define TOKEN_SIZE_MAX 32

enum TokenType {
  TOK_NONE = 0,
  TOK_NAME,
  TOK_INT,
  TOK_SEMICOLON,
  TOK_DOT,
  TOK_EQUALS,
  TOK_PLUS,
  TOK_MINUS,
  TOK_TIMES,
  TOK_DIVIDE,
  TOK_XOR,
  TOK_OR,
  TOK_AND,
  TOK_MOD,
  TOK_INVERT,
  TOK_DOUBLEQUOTE,
  TOK_SINGLEQUOTE,
  TOK_OPENPAREN,
  TOK_CLOSEPAREN,
  TOK_OPENBRACE,
  TOK_CLOSEBRACE,
  TOK_OPENBRACKET,
  TOK_CLOSEBRACKET,
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
void tokens_dump(struct Token* tokens);

#endif