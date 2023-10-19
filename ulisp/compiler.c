#include "compiler.h"
#include "tokenizer.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>



static struct Instruction *instruction_new()
{
    struct Instruction *instr = (struct Instruction*) malloc(sizeof(struct Instruction));
    memset(instr, 0, sizeof(*instr));
    return instr;
}


static struct Instruction *instruction_emplace(struct Instruction *list)
{
    struct Instruction *prev;
    while (list) {
        prev = list;
        list = list->next; 
    }
    prev->next = instruction_new();
    return prev->next;
}


static void ast_expected(const char *s)
{
    printf("! Expected AST node: %s\n", s);
}


static struct Instruction *instr_push(struct Instruction *list, const char *regName)
{
    struct Instruction *instr = instruction_emplace(list);
    strncpy(instr->opcode, "push", OPCODE_SIZE_MAX);
    strncpy(instr->args[0], regName, ARG_SIZE_MAX);
    return instr;
}


static struct Instruction *instr_pop(struct Instruction *list, const char *regName)
{
    struct Instruction *instr = instruction_emplace(list);
    strncpy(instr->opcode, "pop", OPCODE_SIZE_MAX);
    strncpy(instr->args[0], regName, ARG_SIZE_MAX);
    return instr;
}


static struct Instruction *instr_store(struct Instruction *list, const char *varName, const char *regName)
{
    struct Instruction *instr = instruction_emplace(list);
    strncpy(instr->opcode, "store", OPCODE_SIZE_MAX);
    strncpy(instr->args[0], varName, ARG_SIZE_MAX);
    strncpy(instr->args[1], regName, ARG_SIZE_MAX);
    return instr;
}


static struct Instruction *instr_ldi(struct Instruction *list, const char *regName, uint32_t value)
{
    struct Instruction *instr = instruction_emplace(list);
    strncpy(instr->opcode, "ldi", OPCODE_SIZE_MAX);
    strncpy(instr->args[0], regName, ARG_SIZE_MAX);

    char buf[ARG_SIZE_MAX+1];
    snprintf(buf, ARG_SIZE_MAX, "%u", value);
    strncpy(instr->args[1], buf, ARG_SIZE_MAX);
    return instr;
}


static struct Instruction *instr_ld(struct Instruction *list, const char *regName, const char *varName)
{
    struct Instruction *instr = instruction_emplace(list);
    strncpy(instr->opcode, "ld", OPCODE_SIZE_MAX);
    strncpy(instr->args[0], regName, ARG_SIZE_MAX);
    strncpy(instr->args[1], varName, ARG_SIZE_MAX);
    return instr;
}


static struct Instruction *instr_add(struct Instruction *list, const char *lhsReg, const char *rhsReg)
{
    struct Instruction *instr = instruction_emplace(list);
    strncpy(instr->opcode, "add", OPCODE_SIZE_MAX);
    strncpy(instr->args[0], lhsReg, ARG_SIZE_MAX);
    strncpy(instr->args[1], rhsReg, ARG_SIZE_MAX);
    return instr;
}


static struct Variable *var_find(struct Variable *vars, const char *name)
{
    while (vars) {
        if (strcmp(vars->name, name) == 0) {
            return vars;
        }
        vars = vars->next;
    }
    return NULL;
}


static struct Variable *var_add(struct Variable **vars, const char *name, size_t size)
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


static void compile_subexpression(struct AST *ast, struct Program *prog, const char *resultReg)
{
    ast = ast->child;
    struct Instruction *instr;

    if (ast->type == AST_ASSIGN) {
        // Look at the LHS token
        ast = ast->next;
        
        if (ast->type != AST_NAME) {
            expected("name");
            return;
        }

        struct Variable *lhsVar = var_find(prog->vars, ast->token->cvalue);
        if (!lhsVar) {
            lhsVar = var_add(&prog->vars, ast->token->cvalue, 1);
            printf("Defined variable '%s' of size %d\n", lhsVar->name, lhsVar->size);
        }

        // Look at the RHS token
        ast = ast->next;

        if (ast->type == AST_CONST) {
            instr = instr_ldi(prog->instructions, resultReg, ast->token->ivalue);
        } else if (ast->type == AST_NAME) {
            instr = instr_ld(prog->instructions, resultReg, ast->token->cvalue);
        } else if (ast->type == AST_LIST) {
            compile_subexpression(ast, prog, resultReg);
        }

        instr = instr_store(instr, lhsVar->name, resultReg);

    } else if (ast->type == AST_PLUS) {
        // Look at the LHS token
        ast = ast->next;

        if (ast->type == AST_NAME) {
            instr = instr_ld(prog->instructions, resultReg, ast->token->cvalue);

        } else if (ast->type == AST_CONST) {
            instr = instr_ldi(prog->instructions, resultReg, ast->token->ivalue);
        
        } else if (ast->type == AST_LIST) {
            compile_subexpression(ast, prog, resultReg);

        } else {
            printf("Invalid expression as LHS to +\n");
            return;
        }

        // Find a register for the RHS
        size_t rhsRegIndex = N_REGISTERS;
        for (size_t i = 0; i  < N_REGISTERS; ++i) {
            if (!prog->registers[i].occupied) {
                rhsRegIndex = i;
                break;
            }
        }

        size_t pushedRhsReg = N_REGISTERS;
        if (rhsRegIndex == N_REGISTERS) {
            pushedRhsReg = 1;
            rhsRegIndex = pushedRhsReg;
            instr = instr_push(prog->instructions, prog->registers[pushedRhsReg].name);
        } else {
            prog->registers[rhsRegIndex].occupied = 1;
        }

        // Look at the RHS token
        ast = ast->next;

        if (ast->type == AST_NAME) {
            instr = instr_ld(prog->instructions, prog->registers[rhsRegIndex].name, ast->token->cvalue);

        } else if (ast->type == AST_CONST) {
            instr = instr_ldi(prog->instructions, prog->registers[rhsRegIndex].name, ast->token->ivalue);
            
        } else if (ast->type == AST_LIST) {
            compile_subexpression(ast, prog, prog->registers[rhsRegIndex].name);

        } else {
            printf("Invalid expression as RHS to +\n");
            return;
        }

        instr = instr_add(instr, resultReg, prog->registers[rhsRegIndex].name);

        if (pushedRhsReg < N_REGISTERS) {
            instr = instr_pop(instr, prog->registers[pushedRhsReg].name); 
        } else {
            prog->registers[rhsRegIndex].occupied = 0;
        }
    }
}


static struct AST *compile_assignment(struct AST *ast, struct Program *prog)
{
    // Only allow assignment to variables
    ast = ast->next;
    if (ast->type != AST_NAME) {
        ast_expected("name");
        return NULL;
    }

    // Find the assigned variable
    struct Variable *lhsVar = var_find(prog->vars, ast->token->cvalue);
    if (!lhsVar) {
        lhsVar = var_add(&prog->vars, ast->token->cvalue, 1);
        printf("Defined variable '%s' of size %d\n", lhsVar->name, lhsVar->size);
    }

    // Find a register for the temporary value to be stored
    char *reg = NULL;
    int *occupied = NULL;
    int didPush = 0;

    // Find an open register for assignment
    for (size_t i = 0; i < N_REGISTERS; ++i) {
        if (!prog->registers[i].occupied) {
            reg = prog->registers[i].name;
            occupied = &prog->registers[i].occupied;
            break;
        }
    }
    
    struct Instruction *instr;

    if (!reg) {
        didPush = 1;
        reg = prog->registers[0].name;
        instr = instr_push(instr, reg);
    } else {
        *occupied = 1;
    }

    // Look at the assigned value
    ast = ast->next;

    if (ast->type == AST_CONST) {
        instr = instr_ldi(prog->instructions, reg, ast->token->ivalue);
    } else if (ast->type == AST_NAME) {
        instr = instr_ld(prog->instructions, reg, ast->token->cvalue);
    } else if (ast->type == AST_LIST) {
        compile_subexpression(ast, prog, reg);
    }

    instr = instr_store(instr, lhsVar->name, reg);

    if (didPush) {
        instr = instr_pop(instr, reg);
    } else {
        //*occupied = 0;
    }

    // Skip the assignand
    return ast->next;
}

static struct AST *compile_plus(struct AST *ast, struct Program *prog)
{

    return ast;
}


struct AST *compile(struct AST *ast, struct Program *prog, FILE *outFile)
{
    while (ast) {
        if (ast->type == AST_LIST) {
            compile(ast->child, prog, outFile);
            ast = ast->next;
        } else if (ast->type == AST_ASSIGN) {
            ast = compile_assignment(ast, prog);
        } else if (ast->type == AST_PLUS) {
            ast = compile_plus(ast, prog);
        } else {
            printf("! Uncompiled AST node: %d\n", ast->type);
            ast = ast->next;
        }
    }

    return ast;
}



void program_dump(struct Program *prog)
{
    printf("\nProgram\n");
    printf("Variables:\n");
    struct Variable *var = prog->vars;
    while (var) {
        printf("  %s: %lu words\n", var->name, var->size);
        var = var->next;
    }
    printf("\n");
}