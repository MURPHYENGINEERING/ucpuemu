#include "compiler.h"
#include "tokenizer.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>


int compile(struct AST *ast, struct Program *prog, FILE *outFile)
{
    while (ast) {
        if (ast->type == AST_LIST) {
            int result = compile(ast->child, prog, outFile);
            if (result != 0) {
                return result;
            }

        } else if (ast->type == AST_ASSIGN) {
            ast = compile_assignment(ast, prog, outFile);

        } else {
            printf("! Uncompiled AST node: %d\n", ast->type);
        }

        if (ast) {
            ast = ast->next;
        } else {
            return 1;
        }
    }

    return 0;
}


struct AST *compile_assignment(struct AST *ast, struct Program *prog, FILE *outFile)
{
    // Only allow assignment to variables
    ast = ast->next;
    if (ast->type != AST_NAME) {
        ast_expected("name");
        return NULL;
    }

    // Prepare an instruction
    struct Instruction *instr = instruction_add(&prog->instructions, instruction_new());

    // Find the assigned variable address
    struct Variable *lhsVar = var_find(prog->vars, ast->token->cvalue);
    if (lhsVar) {
        printf("Assigning to variable '%s' of size %d\n", lhsVar->name, lhsVar->size);
    } else {
        lhsVar = var_add(&prog->vars, ast->token->cvalue, 1);
        printf("Added variable '%s' of size %d\n", lhsVar->name, lhsVar->size);
    }

    // Write the opcode depending on the type of the assigned value
    // Also write the second argument depending on the type
    ast = ast->next;
    if (ast->type == AST_CONST) {
        strncpy(instr->opcode, "ldi", OPCODE_SIZE_MAX);
        char buf[ARG_SIZE_MAX+1];
        strncpy(instr->args[1], itoa(ast->token->ivalue, buf, 10), ARG_SIZE_MAX);
    } else if (ast->type == AST_NAME) {
        strncpy(instr->opcode, "ld", OPCODE_SIZE_MAX);
        strncpy(instr->args[1], ast->token->cvalue, ARG_SIZE_MAX);
    }

    size_t regIndex = N_REGISTERS;
    int didPush = 0;

    // Find an open register for assignment
    for (size_t i = 0; i < N_REGISTERS; ++i) {
        if (!prog->registers[i].occupied) {
            regIndex = i;
            break;
        }
    }
    
    if (regIndex >= N_REGISTERS) {
        didPush = 1;
        regIndex = 0;
        struct Instruction *stackInstr = instruction_add(&prog->instructions, instruction_new());
        strncpy(stackInstr->opcode, "push", OPCODE_SIZE_MAX);
        strncpy(stackInstr->args[0], prog->registers[regIndex].name, ARG_SIZE_MAX);
        instruction_emit(outFile, stackInstr);
    } else {
        prog->registers[regIndex].occupied = 1;
    }

    strncpy(instr->args[0], prog->registers[regIndex].name, ARG_SIZE_MAX);
    printf("Storing temporary value in register %s\n", prog->registers[regIndex].name);

    instruction_emit(outFile, instr);
    instr->next = instruction_add(&prog->instructions, instruction_new());
    instr = instr->next;
    
    strncpy(instr->opcode, "store", OPCODE_SIZE_MAX);
    strncpy(instr->args[0], lhsVar->name, ARG_SIZE_MAX);
    strncpy(instr->args[1], prog->registers[regIndex].name, ARG_SIZE_MAX);
    instruction_emit(outFile, instr);

    if (didPush) {
        struct Instruction *stackInstr = instruction_add(&prog->instructions, instruction_new());
        strncpy(stackInstr->opcode, "pop", OPCODE_SIZE_MAX);
        strncpy(stackInstr->args[0], prog->registers[regIndex].name, ARG_SIZE_MAX);
        instruction_emit(outFile, stackInstr);
    } else {
        prog->registers[regIndex].occupied = 0;
    }

    return ast;
}

void instruction_emit(FILE *outFile, struct Instruction *instr)
{
    fprintf(outFile, "%s %s %s\n", instr->opcode, instr->args[0], instr->args[1]);
}


struct Instruction *instruction_new()
{
    struct Instruction *instr = (struct Instruction*) malloc(sizeof(struct Instruction));
    memset(instr, 0, sizeof(*instr));
    return instr;
}


struct Instruction *instruction_add(struct Instruction **list, struct Instruction *instr)
{
    if (*list) {
        struct Instruction *prev = NULL;
        while (*list) {
            prev = *list;
            *list = (*list)->next;
        }
        prev->next = instr;
    } else {
        *list = instr;
    }
    return instr;
}


struct Variable *var_find(struct Variable *vars, const char *name)
{
    while (vars) {
        if (strcmp(vars->name, name) == 0) {
            return vars;
        }
        vars = vars->next;
    }
    return NULL;
}


struct Variable *var_add(struct Variable **vars, const char *name, size_t size)
{
    struct Variable *var = *vars;

    if (*vars) {
        struct Variable *prev = NULL;
        while (var) {
            prev = var;
            var = var->next;
        }
        prev->next = (struct Variable*) malloc(sizeof(struct Variable));
        var = prev->next;
        var->size = size;
    } else {
        *vars = (struct Variable*) malloc(sizeof(struct Variable));
        var = *vars;
    }
    strncpy(var->name, name, ARG_SIZE_MAX);
    var->size = size;
    return var;
}


void program_dump(struct Program *prog)
{
    printf("\nProgram\n");
    printf("Variables:\n");
    struct Variable *var = prog->vars;
    while (var) {
        printf("  %s: %d words\n", var->name, var->size);
        var = var->next;
    }
    printf("\n");
}


void ast_expected(const char *s)
{
    printf("! Expected AST node: %s\n", s);
}