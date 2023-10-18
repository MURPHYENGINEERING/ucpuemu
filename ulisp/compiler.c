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
            // Only allow assignment to variables
            ast = ast->next;
            if (ast->type != AST_NAME) {
                ast_expected("name");
                return 1;
            }

            // Prepare an instruction
            struct Instruction *instr = instruction_add(&prog->instructions, instruction_new());

            // Find the assigned variable address
            struct Variable *lhsVar = var_find(prog->vars, ast->token->cvalue);
            if (lhsVar) {
                printf("Assigning to variable '%s' of size %d\n", lhsVar->name, lhsVar->size);
            } else {
                lhsVar = var_add(&prog->vars, ast->token->cvalue);
                printf("Added variable '%s' of size %d\n", lhsVar->name, lhsVar->size);
            }

            // Write the opcode depending on the type of the assigned value
            // Also write the second argument depending on the type
            ast = ast->next;
            if (ast->type == AST_CONST) {
                strncpy(instr->opcode, "ldi", OPCODE_SIZE_MAX);
                strncpy(instr->args[1], itoa(ast->token->ivalue), VAR_NAME_SIZE_MAX);
            } else if (ast->type == AST_NAME) {
                strncpy(instr->opcode, "ld", OPCODE_SIZE_MAX);
                strncpy(instr->args[1], ast->token->cvalue, VAR_NAME_SIZE_MAX);
            }

            // Find an open register for assignment
            for (size_t i = 0; i < N_REGISTERS; ++i) {
                if (prog->registers[i].occupied == 0) {
                    strncpy(instr->args[0], prog->registers[i].name, VAR_NAME_SIZE_MAX);
                    prog->registers[i].occupied = 1;
                }
            }




        } else {
            printf("! Uncompiled AST node: %d\n", ast->type);
        }
        ast = ast->next;
    }

    return 0;
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
        while (list) {
            prev = list;
            list = list->next;
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
    strncpy(var->name, name, VAR_NAME_SIZE_MAX);
    var->size = size;
    return var;
}


void program_dump(struct Program *prog)
{
    printf("\nProgram\n");
    printf("Variables:\n");
    struct Variable *var = prog->vars;
    while (var) {
        printf("  %s\n", var->name);
        var = var->next;
    }
    printf("\n");
}


void ast_expected(const char *s)
{
    printf("! Expected AST node: %s\n", s);
}