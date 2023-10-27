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


static struct Function *function_new()
{
    struct Function *fn = (struct Function*) malloc(sizeof(struct Function));
    memset(fn, 0, sizeof(*fn));
    fn->instructions = (struct Instruction*) malloc(sizeof(struct Instruction));
    return fn;
}


static struct Function *function_emplace(struct Function *list)
{
    struct Function *prev;
    while (list) {
        prev = list;
        list = list->next;
    }
    prev->next = function_new();
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


static struct Instruction *instr_binary(struct Instruction *list, const char *operator, const char *lhsReg, const char *rhsReg)
{
    struct Instruction *instr = instruction_emplace(list);
    strncpy(instr->opcode, operator, OPCODE_SIZE_MAX);
    strncpy(instr->args[0], lhsReg, ARG_SIZE_MAX);
    strncpy(instr->args[1], rhsReg, ARG_SIZE_MAX);
    return instr;
}


static struct Instruction *instr_add(struct Instruction *list, const char *lhsReg, const char *rhsReg)
{
    return instr_binary(list, "add", lhsReg, rhsReg);
}


static struct Instruction *instr_sub(struct Instruction *list, const char *lhsReg, const char *rhsReg)
{
    return instr_binary(list, "sub", lhsReg, rhsReg);
}


static struct Instruction *instr_mul(struct Instruction *list, const char *lhsReg, const char *rhsReg)
{
    return instr_binary(list, "mul", lhsReg, rhsReg);
}


static struct Instruction *instr_div(struct Instruction *list, const char *lhsReg, const char *rhsReg)
{
    return instr_binary(list, "div", lhsReg, rhsReg);
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


static struct Register *register_claim(struct Program *prog)
{
    struct Register *reg = NULL;

    for (size_t i = 0; i < N_REGISTERS; ++i) {
        if (!prog->registers[i].claimed) {
            reg = &prog->registers[i];
            reg->claimed = 1;
            break;
        }
    }
    if (reg == NULL) {
        reg = &prog->registers[0];
        ++reg->nPushes;
        instr_push(prog->instructions, reg->name);
    }
    
    reg->prog = prog;
    return reg;
}


static void register_release(struct Register *reg)
{
    if (reg->nPushes > 0) {
        instr_pop(reg->prog->instructions, reg->name);
        --reg->nPushes;
    } else {
        reg->claimed = 0;
    }
}


static struct AST *compile_node(struct AST *ast, struct Program *prog, struct Context *ctx);


static struct AST *compile_assign(struct AST *ast, struct Program *prog, struct Context *ctx)
{
    // Evaluate the LHS node as a variable name
    ast = ast->next;
    if (ast->type != AST_NAME) {
        printf("! Attempting to assign to a node that doesn't evaluate to a name\n");
        return NULL;
    }

    struct Variable *lhsVar = var_find(prog->vars, ast->token->cvalue);
    if (!lhsVar) {
        lhsVar = var_add(&prog->vars, ast->token->cvalue, 1);
        printf("Defined variable '%s' of size %zu\n", lhsVar->name, lhsVar->size);
    }

    // Evaluate the RHS node to a value
    ast = ast->next;
    compile_node(ast, prog, ctx);

    char buf[FN_NAME_SIZE_MAX+ARG_SIZE_MAX+1];
    snprintf(buf, FN_NAME_SIZE_MAX+ARG_SIZE_MAX, "%s%s", lhsVar->pfx, lhsVar->name);
    instr_store(ctx->instr, buf, ctx->reg->name);

    return ast;
}


static struct AST *compile_binary_operator(
    struct AST *ast, 
    struct Program *prog, 
    struct Context *ctx, 
    struct Instruction *(*operator)(struct Instruction*, const char*, const char*)
)
{
    // Evaluate LHS node to a value
    ast = ast->next;
    compile_node(ast, prog, ctx);

    struct Context rhsCtx;
    memcpy(&rhsCtx, ctx, sizeof(rhsCtx));
    rhsCtx.reg = register_claim(prog);

    // Evaluate RHS node to a value
    ast = ast->next;
    compile_node(ast, prog, &rhsCtx);

    operator(ctx->instr, ctx->reg->name, rhsCtx.reg->name);
    
    register_release(rhsCtx.reg);

    return ast;
}


static struct AST *compile_fn(struct AST *ast, struct Program *prog, struct Context *ctx)
{
    ast = ast->next;
    if (ast->type != AST_NAME) {
        printf("! Expected function name\n");
        return NULL;
    }

    struct Function *fn = function_emplace(prog->functions);
    fn->name = ast->token->cvalue;

    ast = ast->next;

    struct Context fnCtx;
    memcpy(&fnCtx, ctx, sizeof(fnCtx));
    fnCtx.instr = fn->instructions;

    while (ast->type == AST_NAME) { 
        fn->args[fn->nArgs++] = ast->token->cvalue;
        struct Variable *var = var_find(prog->vars, ast->token->cvalue);
        if (!var) {
            var = var_add(&prog->vars, ast->token->cvalue, 1);
            snprintf(var->pfx, FN_NAME_SIZE_MAX, "__%s_", fn->name);
        }
        ast = ast->next;
    }

    if (ast->type != AST_LIST) {
        printf("! Expected function body\n");
        return NULL;
    }

    ast = compile_node(ast, prog, &fnCtx);

    struct Instruction *instr = instruction_emplace(fn->instructions);
    strncpy(instr->opcode, "ret", OPCODE_SIZE_MAX);

    return ast;
}


static struct AST *compile_node(struct AST *ast, struct Program *prog, struct Context *ctx)
{
    if (!ast) {
        return NULL;
    }

    int didClaimReg = 0;
    if (ctx->reg == NULL) {
        ctx->reg = register_claim(prog);
        didClaimReg = 1;
    }

    if (ast->type == AST_LIST) {
        struct AST *list = ast->child;
        while (list) {
            list = compile_node(list, prog, ctx);
        }

    } else if (ast->type == AST_ASSIGN) {
        ast = compile_assign(ast, prog, ctx);

    } else if (ast->type == AST_PLUS) {
        ast = compile_binary_operator(ast, prog, ctx, &instr_add);

    } else if (ast->type == AST_MINUS) {
        ast = compile_binary_operator(ast, prog, ctx, &instr_sub);
    
    } else if (ast->type == AST_TIMES) {
        ast = compile_binary_operator(ast, prog, ctx, &instr_mul);

    } else if (ast->type == AST_DIVIDE) {
        ast = compile_binary_operator(ast, prog, ctx, &instr_div);
    
    } else if (ast->type == AST_NAME) {
        if (strcmp(ast->token->cvalue, "fn") == 0) {
            ast = compile_fn(ast, prog, ctx);
        } else {
            struct Variable *var = var_find(prog->vars, ast->token->cvalue);
            if  (var) {
                char buf[FN_NAME_SIZE_MAX+ARG_SIZE_MAX+1];
                snprintf(buf, FN_NAME_SIZE_MAX+ARG_SIZE_MAX, "%s%s", 
                        var->pfx, var->name);
                instr_ld(ctx->instr, ctx->reg->name, buf);
            } else {
                instr_ld(ctx->instr, ctx->reg->name, ast->token->cvalue);
            }
        }

    } else if (ast->type == AST_CONST) {
        instr_ldi(ctx->instr, ctx->reg->name, ast->token->ivalue);
    }

    if (didClaimReg) {
        register_release(ctx->reg);
    }
    
    return ast ? ast->next : NULL;
}


struct AST *compile(struct AST *ast, struct Program *prog)
{
    struct Context ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.instr = prog->instructions;
    ast = compile_node(ast, prog, &ctx);
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
        printf("  %s %s %s\n", instr->opcode, instr->args[0], instr->args[1]);
        instr = instr->next;
    }
    printf("\n");
}