#include "tokenizer.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>


static size_t read_name(FILE *inFile, char* outName, size_t maxLen)
{
    size_t i = 0;
    int c = fgetc(inFile);
    while (c != EOF && isalpha(c) && i < maxLen) {
        outName[i++] = c;
        c = fgetc(inFile);
    }
    // Don't absorb the first non-alpha char
    if (!isalpha(c)) {
        ungetc(c, inFile); 
    }

    return i;
}


static size_t read_int(FILE *inFile, uint32_t *outVal)
{
    char buf[TOKEN_SIZE_MAX];
    memset(buf, 0, TOKEN_SIZE_MAX);

    size_t i = 0;
    int c = fgetc(inFile);
    while (c != EOF && isdigit(c) && i < TOKEN_SIZE_MAX-1) {
        buf[i++] = c;
        c = fgetc(inFile);
    }
    // Don't absorb the first non-digit char
    ungetc(c, inFile);

    *outVal = atoi(buf);
    return i;
}


static void skip_white(FILE *inFile)
{
    int c = fgetc(inFile);
    while (c != EOF && (c == ' ' || c == '\n')) {
        c = fgetc(inFile);
    }
    // Don't absorb the first non-white char
    ungetc(c, inFile);
}


static struct Token *read_token(FILE *inFile)
{
    // Peek the character so we can decide what to do, but don't read it
    // so if it's alpha or integral then we can read the whole token in one shot
    int c = fgetc(inFile);
    if (c == EOF) {
        return NULL;
    }
    ungetc(c, inFile);

    struct Token *t = (struct Token*) malloc(sizeof(struct Token));
    memset(t, 0, sizeof(*t));

    char symbols[] = {
        '=', '+', '-', '*', '/', '^', '|', '&', '%', '~', '.', '(', ')', '"'
    };

    enum TokenType tokenTypes[] = {
        TOK_ASSIGN, TOK_PLUS, TOK_MINUS, TOK_TIMES, TOK_DIVIDE, TOK_XOR,
        TOK_OR, TOK_AND, TOK_MOD, TOK_INVERT, TOK_DOT, TOK_OPENPAREN,
        TOK_CLOSEPAREN, TOK_DOUBLEQUOTE
    };

    // Map chars to tokens
    for (size_t i = 0; i < sizeof(symbols)/sizeof(symbols[0]); ++i) {
        if (c == symbols[i]) {
            t->type = tokenTypes[i];
            break;
        }
    }

    if (t->type != TOK_NONE) {
        // It was a single char token, so skip it--it's read.
        fgetc(inFile);

        // Is it a double char token?
        c = fgetc(inFile);
        if (c != EOF) {
            switch (c) {
                case '=':
                    if (t->type == TOK_ASSIGN) {
                        t->type = TOK_EQUALS;
                    } else {
                        ungetc(c, inFile);
                    }
                    break;
                case '+':
                    if (t->type == TOK_PLUS) {
                        t->type = TOK_INCREMENT;
                    } else {
                        ungetc(c, inFile);
                    }
                    break;
                case '-':
                    if (t->type == TOK_MINUS) {
                        t->type = TOK_DECREMENT;
                    } else {
                        ungetc(c, inFile);
                    }
                    break;
                default:
                    // Not a double char token, put it back to continue reading
                    ungetc(c, inFile);
                    break;
            }
        }
    } else {
        // Not a single char token
        if (c == ' ' || c == '\n') {
            t->type = TOK_WHITE;
            skip_white(inFile);
        } else if (isalpha(c)) {
            t->type = TOK_NAME;
            read_name(inFile, t->cvalue, TOKEN_SIZE_MAX);
        } else if (isdigit(c)) {
            t->type = TOK_CONST;
            read_int(inFile, &t->ivalue);
        } else {
            printf("! Unknown token: %c\n", c);
            return NULL;
        }
    }

    return t;
}


int tokenize(FILE *inFile, struct Token **outTokens)
{ 
    *outTokens = read_token(inFile);
    struct Token *t = *outTokens;
    while (t) {
        struct Token *newToken = read_token(inFile);
        if (!newToken) {
            break;
        }
        if (newToken->type != TOK_WHITE) {
            t->next = newToken;
            t = t->next;
        }
    }

    return 0;
}


void tokens_dump(struct Token *tokens)
{
    printf("\n");
    while (tokens) {
        switch (tokens->type) {
            case TOK_NAME:
                printf("%s", tokens->cvalue);
                break;
            case TOK_ASSIGN:
                printf("=");
                break;
            case TOK_EQUALS:
                printf("==");
                break;
            case TOK_PLUS:
                printf("+");
                break;
            case TOK_INCREMENT:
                printf("++");
                break;
            case TOK_DECREMENT:
                printf("--");
                break;
            case TOK_CONST:
                printf("%d", tokens->ivalue);
                break;
            case TOK_OPENPAREN:
                printf("(");
                break;
            case TOK_CLOSEPAREN:
                printf(")");
                break;
            case TOK_WHITE:
                break;
            default:
                printf(" <unknown> ");
                break;
        }
        tokens = tokens->next;
        printf(" ");
    }
    printf("\n");
}