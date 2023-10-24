#include "ulisp.h"
#include "tokenizer.h"
#include "parser.h"
#include "compiler.h"
#include <stdio.h>
#include <string.h>


static int translate_file(FILE *inFile, FILE *outFile)
{
    struct Token *tokens;
    int result = tokenize(inFile, &tokens);
    if (result != 0) {
        printf("! Error tokenizing input file: %d\n", result);
        return result;
    }

    tokens_dump(tokens);

    struct AST ast;
    memset(&ast, 0, sizeof(ast));
    ast.type = AST_LIST;
    ast.child = ast_new();
    tokens = parse(tokens, ast.child);
    if (tokens != NULL) {
        printf("! Token stream not completely parsed!\n");
        return result;
    }

    ast_dump(&ast, 0);
    printf("\n");

    struct Program prog;
    memset(&prog, 0, sizeof(prog));
    prog.instructions = (struct Instruction*) malloc(sizeof(struct Instruction));
    memset(prog.instructions, 0, sizeof(*prog.instructions));
    prog.registers[0].name = "a";
    prog.registers[1].name = "b";
    prog.registers[2].name = "c";
    prog.registers[3].name = "d";

    struct AST *remaining = compile(&ast, &prog);
    if (remaining != NULL) {
        printf("! AST remains uncompiled\n");
        return 1;
    }

    program_dump(&prog);
    struct Instruction *instr = prog.instructions;
    while (instr) {
        fprintf(outFile, "  %s %s %s\n", instr->opcode, instr->args[0], instr->args[1]);
        instr = instr->next;
    }
    fprintf(outFile, "\n");
    struct Variable *var = prog.vars;
    while (var) {
        fprintf(outFile, "%s:\n", var->name);
        for (size_t i = 0; i < var->size; ++i) {
            fprintf(outFile, "  word 0\n");
        }
        var = var->next;
    }

    return 0;
}


int main(int argc, char *argv[])
{
    char *inFilename;
    char *outFilename;

    if (argc < 2) {
        //printf("Usage: ulisp <input> <output>\n");
        //return 1;
        inFilename = "test.ulisp";
        outFilename = "test.asm";
    } else {
        inFilename = argv[1];
        outFilename = argv[2];
    }

    printf("uLisp - by Matt Murphy\n");
    printf("    Input file:\t\t%s\n    Output file:\t\t%s\n", inFilename, outFilename);

    FILE *inFile = fopen(inFilename, "r");
    if (!inFile) {
        printf("\b! Input file does not exist\n\n");
        return 1;
    }

    FILE *outFile = fopen(outFilename, "w");
    if (!outFile) {
        printf("\n! Couldn't open output file for writing\n\n");
        fclose(inFile);
        return 1;
    }

    fprintf(outFile, "  ; %s\n", inFilename);
    int result = translate_file(inFile, outFile);

    fclose(inFile);
    fclose(outFile);

    printf("\n");

    return result;
}