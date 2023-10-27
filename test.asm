  ; test.ulisp
    
  ldi a 2
  store arg1 a
  ldi a 1
  ldi b 1
  add a b
  store two a
  ldi b 3
  mul a b
  ldi b 6
  ldi c 3
  store three c
  div b c
  add a b
  store eight a
  ld a eight
  ld b two
  add a b
  store ten a
  halt

myfun:
  ld a __myfun_arg1
  ldi b 2
  add a b
  ret  

arg1:
  word 0x0000
__myfun_arg1:
  word 0x0000
eight:
  word 0x0000
two:
  word 0x0000
three:
  word 0x0000
ten:
  word 0x0000
