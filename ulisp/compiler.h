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
struct AST *compile_assignment(struct AST *ast, struct Program *prog, FILE *outFile);

struct Instruction *instruction_new();
struct Instruction *instruction_add(struct Instruction **list, struct Instruction *instr);
void instruction_emit(FILE *outFile, struct Instruction *instr);

struct Variable *var_find(struct Variable *vars, const char *name);
struct Variable *var_add(struct Variable **vars, const char *name, size_t size);

void ast_expected(const char *s);

void program_dump(struct Program *prog);

#endif