CC=C:\msys64\mingw64\bin\clang.exe

all: ulisp

ulisp: ulisp.c compiler.c parser.c tokenizer.c stringutil.c
	$(CC) -o build/ulisp.exe ulisp.c compiler.c parser.c tokenizer.c stringutil.c

run: ulisp
	build/ulisp.exe ../test.ulisp ../build/test.asm