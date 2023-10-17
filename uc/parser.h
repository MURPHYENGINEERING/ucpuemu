#ifndef UC_PARSER_H
#define UC_PARSER_H

#include <stdlib.h>

#define DAG_NAME_SIZE_MAX 32

struct Token;

enum DAGType
{
  DAG_BLOCK,
  DAG_EXPR,
  DAG_DEFINITION,
  DAG_ASSIGNMENT,
  DAG_SUM,
  DAG_NAME
};


struct DAG
{
  enum DAGType type;
  struct Token *token;
  struct DAG *child;
  struct DAG *next;
};


int parse(struct Token *token, struct DAG **outDag);

void tokens_append(struct Token *list, struct Token *t);
size_t tokens_length(struct Token *list);
void tokens_free(struct Token *list);

void expect(const char *s);

struct DAG *dag_add_child(struct DAG *parent);
struct DAG *dag_add_neighbor(struct DAG *to);

void dag_dump(struct DAG *dag);

#endif