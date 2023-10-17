#ifndef UC_PARSER_H
#define UC_PARSER_H

#include <stdlib.h>

#define DAG_NAME_SIZE_MAX 32

struct Token;


struct TokenList
{
  struct Token *value;
  struct TokenList *next;
  struct TokenList *prev;
};


enum DAGType
{
  DAG_BLOCK,
  DAG_EXPR,
  DAG_DEFINITION,
  DAG_ASSIGNMENT,
  DAG_NAME
};


struct DAG
{
  enum DAGType type;
  struct Token *token;
  struct DAG *children;
  struct DAG *next;
};


int parse(struct Token *token, struct DAG **outDag);

void tokens_append(struct TokenList *list, struct Token *token);
size_t tokens_length(struct TokenList *list);
void tokens_free(struct TokenList *list);

void dump_dag(struct DAG *dag);

#endif