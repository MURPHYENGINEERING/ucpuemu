# ucpuemu

ucpuemu emulates a microcoded CPU architecture I developed for FPGA. 

The CPU supports 29 instructions including `call/ret`, `push/pop`, arithmetic, and jumps, as well as named variables.

---

Build a program ROM using the assembler, uasm:

`uasm/build/uasm prog.asm prog.rom`

Run the CPU emulator:

`ucpu/build/ucpu`

Result: `0xA` in `R0`

---

Example `prog.asm`:

```
  ldi   a 7       ; literals can be 0x7, 0b111, or 7
  push  a         ; save the contents of register A
  call  fun
  pop   a         ; restore the contents of register A
  ld    b myvar   ; read B from memory at myvar
  add   a b       ; add B into A
  halt            ; result: A = 10, B = 3

fun: 
  ldi   a 3       ; interfere with register A to demonstrate push/pop after returning
  store myvar a   ; write A into memory at myvar
  ret

myvar:
  word 0xDEADBEEF ; allocate 32 bits at a named location
```
