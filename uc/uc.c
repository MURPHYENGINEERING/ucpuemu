#include "uc.h"
#include "tokenizer.h"
#include "parser.h"
#include <stdio.h>


static int translate_file(FILE *inFile, FILE *outFile)
{
  struct Token *tokens;
  int result = tokenize(inFile, &tokens);
  if (result != 0) {
    printf("! Error tokenizing input file: %d\n", result);
    return result;
  }

  tokens_dump(tokens);

  struct DAG *dag;
  result = parse(tokens, &dag);
  if (result != 0) {
    printf("! Error parsing token stream: %d\n", result);
    return result;
  }

  dag_dump(dag);

  return 0;
}


int main(int argc, char *argv[])
{
  char *inFilename;
  char *outFilename;

  if (argc < 2) {
    //printf("Usage: uc <input> <output>\n");
    //return 1;
    inFilename = "test.uc";
    outFilename = "test.asm";
  } else {
    inFilename = argv[1];
    outFilename = argv[2];
  }

  printf("uC - by Matt Murphy\n");
  printf("  Input file:\t\t%s\n  Output file:\t\t%s\n", inFilename, outFilename);

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

  int result = translate_file(inFile, outFile);

  fclose(inFile);
  fclose(outFile);

  printf("\n");

  return result;
}