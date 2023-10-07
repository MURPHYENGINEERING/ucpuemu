  ldi c 38400
  ldi d 0xFFFF
  ldi a 0x400
clear_screen_loop:
  rstore a d
  inc a
  dec c
  jnz c clear_screen_loop
  halt