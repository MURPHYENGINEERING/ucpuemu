#include "parser.h"
#include "tokenizer.h"
#include <string.h>


int parse(struct Token *t, struct DAG **outDag)
{
  *outDag = (struct DAG*) malloc(sizeof(struct DAG));
  struct DAG *block = *outDag;
  block->type = DAG_BLOCK;

  // Store tokens we see until we get to one that defines an encapsulating node
  struct Token lhsTokens;
  memset(&lhsTokens, 0, sizeof(lhsTokens));

  while (t) {
    if (t->type == TOK_EQUALS) {
      struct DAG *assignment = dag_add_child(block);
      assignment->type = DAG_ASSIGNMENT;
      struct DAG *lhs = dag_add_child(assignment);
      struct DAG *rhs = dag_add_neighbor(lhs);

    } else if (t->type == TOK_SEMICOLON) {

    } else if (t->type == TOK_NAME) {
      // Keyword search: if, for, while, etc.
      if (strcmp(t->cvalue, "if") == 0) {
        t = t->next;
        if (t->type != TOK_OPENPAREN) {
          expect("(");
        }
      }
      // Not a keyword... function call?
    } else {
      tokens_append(&lhsTokens, t);
      printf("LHS: %d\n", tokens_length(&lhsTokens));
    }

    t = t->next;
  }

  return 0;
}


struct DAG *dag_add_child(struct DAG *parent)
{
  parent->child = (struct DAG*) malloc(sizeof(struct DAG));
  memset(parent->child, 0, sizeof(*parent->child));
  return parent->child;
}


struct DAG *dag_add_neighbor(struct DAG *to)
{
  to->next = (struct DAG*) malloc(sizeof(struct DAG));
  memset(to->next, 0, sizeof(*to->next));
  return to->next;
}


void expect(const char *s)
{
  printf("! Expected token: %s\n", s);
}


void dag_dump(struct DAG *dag)
{
  printf("DAG:\n");
  while (dag) {
    switch (dag->type) {
    case DAG_BLOCK:
      printf("{\n");
      break;
    case DAG_ASSIGNMENT:
      printf("= ");
      break;
    case DAG_DEFINITION:
      printf("def: ");
      break;
    default:
    printf("<unknown>\n");
    }
    if (dag->child) {
      dag_dump(dag->child);
    }
    dag = dag->next;
  }
}


size_t tokens_length(struct Token *list)
{
  size_t i = 0;
  while (list) {
    ++i;
    list = list->next;
  }
  return i;
}


void tokens_append(struct Token *list, struct Token *t)
{
  if (list->type == TOK_NONE) {
    memcpy(list, t, sizeof(*list));
  }
  
  while (list->next) {
    list = list->next;
  }
  list->next = (struct Token*) malloc(sizeof(struct Token));
  memcpy(list->next, t, sizeof(*list->next));
  list->next->next = NULL;
}


void tokens_free(struct Token *list)
{
  
}