#include "parser.h"
#include "tokenizer.h"
#include <string.h>


int parse(struct Token *token, struct DAG **outDag)
{
  *outDag = (struct DAG*) malloc(sizeof(struct DAG));
  struct DAG *dag = *outDag;
  dag->type = DAG_BLOCK;

  dag->children = (struct DAG*) malloc(sizeof(struct DAG));

  // Store tokens we see until we get to one that defines an encapsulating node
  struct TokenList lhs;
  memset(&lhs, 0, sizeof(lhs));

  while (token) {
    if (token->type == TOK_WHITE) {
      // Ignore whitespace
    } else if (token->type == TOK_EQUALS) {
      dag->children->type = DAG_ASSIGNMENT;
      printf("Tokens on LHS: %u\n", tokens_length(&lhs));

      if (tokens_length(&lhs) == 2) {
        dag->children->children = (struct DAG*) malloc(sizeof(struct DAG));
        struct DAG *def = dag->children->children;
        def->type = DAG_DEFINITION;
        def->children = (struct DAG*) malloc(sizeof(struct DAG));
        def->children->type = DAG_NAME;
        def->children->token = lhs.value;
        def->children->next = (struct DAG*) malloc(sizeof(struct DAG));
        def->children->next->type = DAG_NAME;
        def->children->next->token = lhs.next->value;
      }
    } else if (token->type == TOK_SEMICOLON) {
      tokens_free(&lhs);
      memset(&lhs, 0, sizeof(lhs));
    } else {
      tokens_append(&lhs, token);
    }

    token = token->next;
  }

  return 0;
}


void dump_dag(struct DAG *dag)
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
    if (dag->children) {
      dump_dag(dag->children);
    }
    dag = dag->next;
  }
}


size_t tokens_length(struct TokenList *list)
{
  size_t i = 0;
  while (list) {
    ++i;
    list = list->next;
  }
  return i;
}


void tokens_append(struct TokenList *list, struct Token *token)
{
  if (list->value == NULL) {
    list->value = token;
    return;
  }
  
  while (list->next) {
    list = list->next;
  }
  list->next = (struct TokenList*) malloc(sizeof(struct TokenList));
  list->next->value = token;
  list->next->next = NULL;
  list->next->prev = list;
}


void tokens_free(struct TokenList *list)
{
  while (list->next) {
    list = list->next;
  }

  while (list->prev) {
    struct TokenList *prev = list->prev;
    free(list);
    list = prev;
  }
}