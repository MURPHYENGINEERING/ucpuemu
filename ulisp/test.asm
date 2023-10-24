  ; test.ulisp
    
  ldi a 2
  ldi b 3
  mul a b
  ldi b 6
  ldi c 3
  store three c
  div b c
  add a b
  store eight a

eight:
  word 0x0000
three:
  word 0x0000
