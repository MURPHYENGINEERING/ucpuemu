  ; test.ulisp
    
  ldi a 2
  ldi b 3
  mul a b
  ldi b 4
  ldi c 2
  store two c
  div b c
  add a b
  store eight a

eight:
  word 0
two:
  word 0
