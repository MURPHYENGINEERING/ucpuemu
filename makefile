all: test.rom

run: test.rom test.vars
	ucpu/build/ucpu build/test.rom build/test.vars

test.vars: test.rom

test.rom: test.asm
	uasm/build/uasm build/test.asm build/test.rom

test.asm: test.ulisp
	ulisp/build/ulisp test.ulisp build/test.asm

clean:
	find build/ -type f -maxdepth 1 -delete