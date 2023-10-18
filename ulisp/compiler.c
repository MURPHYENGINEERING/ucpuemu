#include "compiler.h"
#include "tokenizer.h"
#include "parser.h"
#include <stdio.h>
#include <string.h>


int compile(struct AST *ast, struct Program *prog, FILE *outFile)
{
    int result = 0;
    struct Variable *existingVar;

    while (ast) {
        switch (ast->type) {
            case AST_LIST:
                result = compile(ast->child, prog, outFile);
                if (result != 0) {
                    return result;
                }
                break;

            case AST_ASSIGN:
                ast = ast->next;
                if (ast->type != AST_NAME) {
                    ast_expected("name");
                    return 1;
                }
                existingVar = var_find(prog->vars, ast->token->cvalue);
                if (existingVar) {
                    printf("Assigning to variable '%s' at address %d\n", existingVar->name, existingVar->address);
                } else {
                    existingVar = var_add(&prog->vars, ast->token->cvalue);
                    printf("Added variable '%s' at address %d\n", existingVar->name, existingVar->address);
                }
            break;

            default:
                printf("! Uncompiled AST node: %d\n", ast->type);
                break;
        }
        ast = ast->next;
    }

    return result;
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


struct Variable *var_add(struct Variable **vars, const char *name)
{
    uint32_t address = 0;
    struct Variable *var = *vars;

    if (*vars) {
        struct Variable *prev = NULL;
        while (var) {
            address = var->address + sizeof(uint32_t);
            prev = var;
            var = var->next;
        }
        prev->next = (struct Variable*) malloc(sizeof(struct Variable));
        var = prev->next;
    } else {
        *vars = (struct Variable*) malloc(sizeof(struct Variable));
        var = *vars;
    }
    strncpy(var->name, name, VAR_NAME_SIZE_MAX);
    var->address = address;
    return var;
}


void program_dump(struct Program *prog)
{
    printf("\nProgram\n");
    printf("Variables:\n");
    struct Variable *var = prog->vars;
    while (var) {
        printf("  %s: %d\n", var->name, var->address);
        var = var->next;
    }
    printf("\n");
}


void ast_expected(const char *s)
{
    printf("! Expected AST node: %s\n", s);
}