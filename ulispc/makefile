CC=C:\msys64\mingw64\bin\clang.exe

all: ulispc

ulispc: ulispc.c compiler.c parser.c tokenizer.c stringutil.c
	$(CC) -o build/ulispc.exe ulisp.c compiler.c parser.c tokenizer.c stringutil.c

run: ulispc
	build/ulispc.exe ../test.ulisp ../build/test.asm