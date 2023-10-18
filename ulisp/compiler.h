#ifndef ULISP_COMPILER_H
#define ULISP_COMPILER_H

#include <stdio.h>
#include <stdint.h>

#define VAR_NAME_SIZE_MAX 32
#define OPCODE_SIZE_MAX 4
#define N_ARGS_MAX 2
#define N_REGISTERS 4

struct AST;

struct Instruction
{
    size_t size;
    char opcode[OPCODE_SIZE_MAX+1];
    char args[N_ARGS_MAX][VAR_NAME_SIZE_MAX+1];
    struct Instruction *next;
};


struct Variable
{
    char name[VAR_NAME_SIZE_MAX+1];
    size_t size;
    struct Variable *next;
};


struct Register
{
    char *name;
    int occupied;
};


struct Program 
{
    struct Variable *vars;
    struct Instruction *instructions;
    struct Register registers[N_REGISTERS];
};

int compile(struct AST *ast, struct Program *prog, FILE *outFile);

struct Instruction *instruction_new();
struct Instruction *instruction_add(struct Instruction *list, struct Instruction *instr);

struct Variable *var_find(struct Variable *vars, const char *name);
struct Variable *var_add(struct Variable **vars, const char *name, size_t size);

void ast_expected(const char *s);

void program_dump(struct Program *prog);

#endif