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
  halt

myfun:
  ld a __myfun_arg1
  ld b __myfun_arg2
  add a b
  store __myfun_two a
  ret  
otherfun:
  ld a __otherfun_arg1
  ld b __otherfun_arg2
  add a b
  ret  

eight:
  word 0x0000
two:
  word 0x0000
three:
  word 0x0000
__myfun_arg1:
  word 0x0000
__myfun_arg2:
  word 0x0000
__myfun_two:
  word 0x0000
__otherfun_arg1:
  word 0x0000
__otherfun_arg2:
  word 0x0000
