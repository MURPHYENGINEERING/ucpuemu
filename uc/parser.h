#ifndef UC_PARSER_H
#define UC_PARSER_H

struct Token;

struct DAG
{
};


int parse(struct Token* tokens, struct DAG** outDag);

#endif