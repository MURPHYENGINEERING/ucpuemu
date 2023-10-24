#ifndef ULISP_COMPILER_H
#define ULISP_COMPILER_H

#include <stdio.h>
#include <stdint.h>

#define OPCODE_SIZE_MAX 5
#define ARG_SIZE_MAX 32
#define N_ARGS_MAX 2
#define N_REGISTERS 4

struct AST;

struct Instruction
{
    size_t size;
    char opcode[OPCODE_SIZE_MAX+1];
    char args[N_ARGS_MAX][ARG_SIZE_MAX+1];
    struct Instruction *next;
};


struct Variable
{
    char name[ARG_SIZE_MAX+1];
    size_t size;
    struct Variable *next;
};


struct Program;

struct Register
{
    struct Program* prog;
    char *name;
    int claimed;
    size_t nPushes;
};


struct Program 
{
    struct Variable *vars;
    struct Instruction *instructions;
    struct Register registers[N_REGISTERS];
};


struct AST *compile(struct AST *ast, struct Program *prog);

void program_dump(struct Program *prog);

#endif