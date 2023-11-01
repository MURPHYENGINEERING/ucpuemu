#ifndef ULISP_COMPILER_H
#define ULISP_COMPILER_H

#include <stdio.h>
#include <stdint.h>

#define OPCODE_SIZE_MAX 5
#define ARG_SIZE_MAX 32
#define N_ARGS_MAX 2
#define N_REGISTERS 4
#define FN_NAME_SIZE_MAX 256
#define N_FN_ARGS_MAX 32
#define FN_ARG_SIZE_MAX 32

struct AST;
struct Function;
struct Program;

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


struct Register
{
    struct Program* prog;
    char *name;
    int claimed;
    size_t nPushes;
};


struct Function
{
    char *name;
    char *args[N_FN_ARGS_MAX];
    size_t nArgs;
    struct Instruction *instructions;
    struct Variable *vars;
    struct Function *next;
};


struct Program 
{
    struct Variable *vars;
    struct Instruction *instructions;
    struct Function *functions;
    struct Register registers[N_REGISTERS];
};


struct Context
{
    struct Instruction *instr;
    struct Register *reg;
    struct Function *fn;
};


struct AST *compile(struct AST *ast, struct Program *prog);

void program_dump(struct Program *prog);

#endif