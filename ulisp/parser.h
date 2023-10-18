#ifndef UC_PARSER_H
#define UC_PARSER_H

#include <stdlib.h>

#define AST_NAME_SIZE_MAX 32

struct Token;

enum ASTType
{
    AST_ERROR = 0,
    AST_LIST,
    AST_NAME,
    AST_CONST,
    AST_FN,
    AST_ASSIGN,
    AST_PLUS,
    AST_MINUS,
    AST_TIMES,
    AST_DIVIDE,
    AST_XOR,
    AST_OR,
    AST_AND,
    AST_MOD,
    AST_INVERT
};


struct AST
{
    enum ASTType type;
    struct Token *token;
    struct AST *child;
    struct AST *next;
    struct AST *prev;
};


struct Token *parse(struct Token *token, struct AST *ast);

void tokens_append(struct Token *list, struct Token *t);
size_t tokens_length(struct Token *list);
void tokens_free(struct Token *list);
struct Token *tokens_new();

void expected(const char *s);

struct AST *ast_new();
struct AST *ast_new_child(struct AST *parent);
struct AST *ast_new_neighbor(struct AST *to);

void ast_dump(struct AST *ast);

#endif