#include "parser.h"
#include "tokenizer.h"
#include <string.h>


struct Token *parse(struct Token *t, struct AST *ast)
{
    enum ASTType astTypes[] = {
        AST_ERROR, AST_ASSIGN, AST_PLUS, AST_MINUS, AST_TIMES, AST_DIVIDE, 
        AST_XOR, AST_OR, AST_AND, AST_MOD, AST_INVERT, AST_NAME, AST_CONST
    };

    enum TokenType tokTypes[] = {
        TOK_NONE, TOK_EQUALS, TOK_PLUS, TOK_MINUS, TOK_TIMES, TOK_DIVIDE, 
        TOK_XOR, TOK_OR, TOK_AND, TOK_MOD, TOK_INVERT, TOK_NAME, TOK_CONST
    };

    while (t) {
        if (t->type == TOK_OPENPAREN) {
            ast->type = AST_LIST;
            ast->child = ast_new();
            t = parse(t->next, ast->child);
        } else {
            if (t->type == TOK_CLOSEPAREN) {
                ast->prev->next = NULL;
                free(ast);
                return t->next;
            }
            for (size_t i = 0; i < 13; ++i) {
                if (t->type == tokTypes[i]) {
                    ast->type = astTypes[i];
                    break;
                }
            }
            ast->token = t;
            t = t->next;
        }
        ast->next = ast_new();
        ast->next->prev = ast;
        ast = ast->next;
    }
    ast->prev->next = NULL;
    free(ast);
    return t;
}


struct AST *ast_new()
{
    struct AST *ast = (struct AST*) malloc(sizeof(struct AST));
    memset(ast, 0, sizeof(*ast));
    return ast;
}


struct AST *ast_new_child(struct AST *parent)
{
    parent->child = ast_new();
    return parent->child;
}


struct AST *ast_new_neighbor(struct AST *to)
{
    to->next = ast_new();
    return to->next;
}


void expected(const char *s)
{
    printf("! expected token: %s\n", s);
}


struct Token *tokens_new()
{
    struct Token *t = (struct Token*) malloc(sizeof(struct Token));
    memset(t, 0, sizeof(*t));
    return t;
}


size_t tokens_length(struct Token *list)
{
    size_t i = 0;
    while (list) {
        ++i;
        list = list->next;
    }
    return i;
}


void tokens_append(struct Token *list, struct Token *t)
{
    if (list->type == TOK_NONE) {
        memcpy(list, t, sizeof(*list));
        list->next = NULL;
        return;
    }

    while (list->next) {
        list = list->next;
    }
    list->next = (struct Token*) malloc(sizeof(struct Token));
    memcpy(list->next, t, sizeof(*list->next));
    list->next->next = NULL;
}


void tokens_free(struct Token *list)
{
    while (list) {
        struct Token *next = list->next;
        free(list);
        list = next;
    }
}


void ast_dump(struct AST *ast)
{
    while (ast) {
        switch (ast->type) {
        case AST_LIST:
            printf("(");
            ast_dump(ast->child);
            printf(")");
            break;
        case AST_NAME: 
            printf("%s ", ast->token->cvalue); 
            break;
        case AST_CONST: 
            printf("%d", ast->token->ivalue); 
            break;
        case AST_FN: 
            printf("fn "); 
            break;
        case AST_ASSIGN:
            printf("= ");
            break;
        case AST_PLUS:
            printf("+ ");
            break;
        case AST_ERROR:
            printf("<err>");
            break;
        default: 
            printf("<?>");
            break;
        }
        ast = ast->next;
    }
}
