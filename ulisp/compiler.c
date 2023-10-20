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


static const char *pick_register(struct Program *prog, size_t *regIndex, int *didPush)
{
    char *resultReg = NULL;

    for (size_t i = 0; i < N_REGISTERS; ++i) {
        if (!prog->registers[i].occupied) {
            *regIndex = i;
            resultReg = prog->registers[i].name;
            prog->registers[i].occupied = 1;
            break;
        }
    }
    if (resultReg == NULL) {
        *regIndex = 0;
        *didPush = 1;
        resultReg = prog->registers[*regIndex].name;
        instr_push(prog->instructions, resultReg);
    }

    return resultReg;
}


static struct AST *compile_node(struct AST *ast, struct Program *prog, const char *resultReg);

static struct AST *compile_assign(struct AST *ast, struct Program *prog, const char *resultReg)
{
    size_t resultRegIndex = N_REGISTERS;
    int didPushReg = 0;

    if (resultReg == NULL) {
        resultReg = pick_register(prog, &resultRegIndex, &didPushReg);
    }

    // Evaluate the LHS node as a variable name
    ast = ast->next;
    if (ast->type != AST_NAME) {
        printf("! Attempting to assign to a node that doesn't evaluate to a name\n");
        return NULL;
    }
    struct Variable *lhsVar = var_find(prog->vars, ast->token->cvalue);
    if (!lhsVar) {
        lhsVar = var_add(&prog->vars, ast->token->cvalue, 1);
        printf("Defined variable '%s' of size %d\n", lhsVar->name, lhsVar->size);
    }

    // Evaluate the RHS node to a value
    ast = ast->next;
    compile_node(ast, prog, resultReg);

    instr_store(prog->instructions, lhsVar->name, resultReg);

    if (didPushReg) {
        instr_pop(prog->instructions, resultReg);
    } else {
        prog->registers[resultRegIndex].occupied = 0;
    }

    return ast;
}


static struct AST *compile_plus(struct AST *ast, struct Program *prog, const char *resultReg)
{
    size_t resultRegIndex = N_REGISTERS;
    int didPushReg = 0;

    if (resultReg == NULL) {
        resultReg = pick_register(prog, &resultRegIndex, &didPushReg);
    }

    // Evaluate LHS node to a value
    ast = ast->next;
    compile_node(ast, prog, resultReg);

    size_t rhsResultRegIndex = N_REGISTERS;
    int didPushRhsReg = 0;
    const char *rhsResultReg = pick_register(prog, &rhsResultRegIndex, &didPushRhsReg);

    ast = ast->next;
    compile_node(ast, prog, rhsResultReg);

    instr_add(prog->instructions, resultReg, rhsResultReg);
    
    if (didPushRhsReg) {
        instr_pop(prog->instructions, rhsResultReg);
    } else {
        prog->registers[rhsResultRegIndex].occupied = 0;
    }

    if (didPushReg) {
        instr_pop(prog->instructions, resultReg);
    } else {
        prog->registers[resultRegIndex].occupied = 0;
    }

    return ast;
}


static struct AST *compile_node(struct AST *ast, struct Program *prog, const char *resultReg)
{
    size_t resultRegIndex = N_REGISTERS;
    int didPushReg = 0;

    if (resultReg == NULL) {
        resultReg = pick_register(prog, &resultRegIndex, &didPushReg);
    }

    if (ast->type == AST_LIST) {
        while (ast) {
            compile_node(ast->child, prog, resultReg);
            ast = ast->next;
        }
    } else if (ast->type == AST_ASSIGN) {
        ast = compile_assign(ast, prog, resultReg);

    } else if (ast->type == AST_PLUS) {
        ast = compile_plus(ast, prog, resultReg);

    } else if (ast->type == AST_NAME) {
        instr_ld(prog->instructions, resultReg, ast->token->cvalue);

    } else if (ast->type == AST_CONST) {
        instr_ldi(prog->instructions, resultReg, ast->token->ivalue);
    }

    if (didPushReg) {
        instr_pop(prog->instructions, resultReg);
    } else {
        prog->registers[resultRegIndex].occupied = 0;
    }
    
    return ast;
}


struct AST *compile(struct AST *ast, struct Program *prog, FILE *outFile)
{
    while (ast) {
        ast = compile_node(ast, prog, NULL);
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
    struct Instruction *instr = prog->instructions;
    while (instr) {
        printf("%s %s %s\n", instr->opcode, instr->args[0], instr->args[1]);
        instr = instr->next;
    }
    printf("\n");
}