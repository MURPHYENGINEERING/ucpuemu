# uasm
Assembly language for a microcoded CPU implemented on FPGA, for EEE 333 with Seth Abraham

### Usage

```
cmake .
cmake --build . --target all
./uasm [input_file] [output_file]
```

If you don't specify `output_file` then it will use the name of the input file and add the extension `.dat`. If you don't specify filenames then it will use `ucpu.asm` and output to `ucpu.dat`.

If the output has the extension `.dat` then the output will be in the EEE333 `loader.dat` file format.

If the output has the extension `.mif` then the output will be in the Intel Memory Initialization File format.

### Instructions
| Example<img width=300/>                       | First argument                  | Second argument                | Third argument               | Description                                                                                                                                         |
|-----------------------------------------------|---------------------------------|--------------------------------|------------------------------|-----------------------------------------------------------------------------------------------------------------------------------------------------|
| `ldi a 0xff`<br/>`ldi b 0b1010`<br/>`ldi c 3` | Destination register            | Source constant                |                              | Load the given constant value into the destination register.                                                                                        |
| `ld a myvar`<br/>`load a 0xda`                | Destination register            | Source label or memory address |                              | Load the contents of memory at the given address into the destination register. The address may be specified literally or referred to with a label. |
| `mov a b`                                     | Desination register             | Source register                |                              | Copy the contents of the source register into the destination register.                                                                             |
| `store myvar a`<br/>`store 0xff a`            | Destination memory address      | Source register                |                              | Store the contents of the source register into memory at the given address. The address can be specified literally or referred to with a label.     |
| `inc a`                                       | Source and destination register |                                |                              | Increment the contents of the given register.                                                                                                       |
| `dec a`                                       | Source and destination register |                                |                              | Decrement the contents of the given register.                                                                                                       |
| `add a b`                                     | Destination register            | RHS register                   |                              | Add the contents of the RHS register to the destination register.                                                                                   |
| `sub a b`                                     | Destination register            | RHS register                   |                              | Subtract the contents of the RHS register from the destination register.                                                                            |
| `mul a b`                                     | Destination register            | RHS register                   |                              | Multiply the contents of the RHS register by the destination register.                                                                              |
| `div a b`                                     | Destination register            | RHS register                   |                              | Divide the contents of the destination register by the RHS register.                                                                                |
| `and a b`                                     | Destination register            | RHS register                   |                              | Bitwise AND the contents of the destination register with the RHS register.                                                                         |
| `or a b`                                      | Destination register            | RHS register                   |                              | Bitwise OR the contents of the destination register with the RHS register.                                                                          |
| `xor a b`                                     | Destination register            | RHS register                   |                              | Bitwise XOR the contents of the destination register with the RHS register.                                                                         |
| `shl a b`                                     | Destination register            | RHS register                   |                              | Shift the contents of the destination register left by the number of bits specified by the contents of the RHS register.                            |
| `shr a b`                                     | Destination register            | RHS register                   |                              | Shift the contents of the destination register right by the number of bits specified by the contents of the RHS register.                            |
| `jmp mylabel`<br/>`jmp 0xff`                  | Destination address or label    |                                |                              | Jump to the given address. The address can be specified literally or referred to with a label.                                                      |
| `je a b mylabel`                              | LHS register                    | RHS register                   | Destination address or label | Jump to the given address if the contents of the two registers are equal.                                                                           |
| `jne a b mylabel`                             | LHS register                    | RHS register                   | Destination address or label | Jump to the given address if the contents of the registers are not equal.                                                                           |
| `jg a b mylabel`                              | LHS register                    | RHS register                   | Destination address or label | Jump to the given address if the contents of the LHS register are greater than the contents of the RHS register.                                    |
| `jz a mylabel`                                | LHS register                    | Destination address or label   |                              | Jump to the given address if the contents of the LHS register are zero.                                                                             |
| `jnz a mylabel`                               | LHS register                    | Destination address            |                              | Jump to the given address if the contents of the LHS register are not zero.                                                                         |
| `call mylabel`                                | Destination address or label    |                                |                              | Push the next instruction address to the stack and jump to the given address.                                                                       |
| `ret`                                         |                                 |                                |                              | Pop an instruction address from the stack and jump to it.                                                                                           |
| `push a`                                      | Source register                 |                                |                              | Push the contents of the source register onto the stack.                                                                                            |
| `pop b`                                       | Destination register            |                                |                              | Pop the top of the stack into the destination register.                                                                                             |


### Example program (`ucpu.asm`)
```
  ldi   a 7       ; literals can be 0x7, 0b111, or 7
  push  a         ; save the contents of register A
  call  fun
  pop   a         ; restore the contents of register A
  ld    b myvar   ; read B from memory at myvar
  add   a b       ; add B into A
  halt            ; result: A = 10, B = 3

fun: 
  ldi   a 3       ; interfere with register A
  store myvar a   ; write A into memory at myvar
  ret

myvar:
  word            ; allocate 8 bits at a named location
```

### Assembler output
```
uASM - by Matt Murphy, for EEE 333 wth Seth Abraham
  Input file:   ucpu.asm
  Output file:  ucpu.dat

Labels

  fun        = 0b on line 9
  myvar      = 10 on line 14

Instructions

  ldi a 7          00  34
                   01  07
  push a           02  84
  call fun         03  80
                   04  0b
  pop a            05  88
  ld b myvar       06  39
                   07  10
  add a b          08  04
                   09  01
  halt             0a  00
  ldi a 3          0b  34
                   0c  03
  store myvar a    0d  2c
                   0e  10
  ret              0f  81
  word             10  00
```

### Resulting `loader.dat` file
```
00  34
01  07
02  84
03  80
04  0b
05  88
06  39
07  10
08  04
09  01
0a  00
0b  34
0c  03
0d  2c
0e  10
0f  81
10  00
```
