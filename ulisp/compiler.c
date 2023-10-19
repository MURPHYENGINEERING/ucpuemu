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


static struct AST *compile_assignment(struct AST *ast, struct Program *prog)
{
    // Only allow assignment to variables
    ast = ast->next;
    if (ast->type != AST_NAME) {
        ast_expected("name");
        return NULL;
    }

    // Prepare an instruction
    struct Instruction *instr = instruction_emplace(prog->instructions);

    // Find the assigned variable address
    struct Variable *lhsVar = var_find(prog->vars, ast->token->cvalue);
    if (lhsVar) {
        printf("Assigning to variable '%s' of size %lu\n", lhsVar->name, lhsVar->size);
    } else {
        lhsVar = var_add(&prog->vars, ast->token->cvalue, 1);
        printf("Added variable '%s' of size %lu\n", lhsVar->name, lhsVar->size);
    }

    // Write the opcode depending on the type of the assigned value
    // Also write the second argument depending on the type
    ast = ast->next;
    if (ast->type == AST_CONST) {
        strncpy(instr->opcode, "ldi", OPCODE_SIZE_MAX);
        char buf[ARG_SIZE_MAX+1];
        snprintf(buf, ARG_SIZE_MAX, "%u", ast->token->ivalue);
        strncpy(instr->args[1], buf, ARG_SIZE_MAX);
    } else if (ast->type == AST_NAME) {
        strncpy(instr->opcode, "ld", OPCODE_SIZE_MAX);
        strncpy(instr->args[1], ast->token->cvalue, ARG_SIZE_MAX);
    }

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
    
    if (!reg) {
        didPush = 1;
        reg = prog->registers[0].name;
        instr = instr_push(instr, reg);
    } else {
        *occupied = 1;
    }

    printf("Storing temporary value in register %s\n", reg);
    strncpy(instr->args[0], reg, ARG_SIZE_MAX);

    instr = instruction_emplace(instr);
    
    strncpy(instr->opcode, "store", OPCODE_SIZE_MAX);
    strncpy(instr->args[0], lhsVar->name, ARG_SIZE_MAX);
    strncpy(instr->args[1], reg, ARG_SIZE_MAX);

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
        printf("AST: %d\n", ast->type);
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