#ifndef ULISP_COMPILER_H
#define ULISP_COMPILER_H

#include <stdio.h>
#include <stdint.h>

#define VAR_NAME_SIZE_MAX 32


struct AST;

struct Variable
{
    char name[VAR_NAME_SIZE_MAX];
    uint32_t address;
    struct Variable *next;
};

struct Program 
{
    struct Variable *vars;
};

int compile(struct AST *ast, struct Program *prog, FILE *outFile);

struct Variable *var_find(struct Variable *vars, const char *name);
struct Variable *var_add(struct Variable **vars, const char *name);

void ast_expected(const char *s);

void program_dump(struct Program *prog);

#endif