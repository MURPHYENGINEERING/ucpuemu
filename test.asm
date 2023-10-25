  ; test.ulisp
    
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
myfun:
  ld a two
  ldi b 2
  add a b
  ret  

two:
  word 0x0000
eight:
  word 0x0000
three:
  word 0x0000
