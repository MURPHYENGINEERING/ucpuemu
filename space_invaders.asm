main:

check_move_right:
  ld d 0xffb 
  jz d check_move_left

  ld d ship_position
  inc d
  store ship_position d
  
check_move_left:
  ld d 0xffc
  jz d check_shoot

  ld d ship_position
  dec d
  store ship_position d

check_shoot:
  ld d 0xffd
  jz d check_shoot_released

  ; If we already consumed this shot then wait until the key is released
  ld d can_shoot
  jz d draw

check_shoot_do_shot:
  ; Consume the shot until the key is released
  ldi d 0
  store can_shoot d

  ; Make sure there aren't too many shots on screen
  ld d num_shots
  ld c max_shots         
  je c d check_shoot_max_exceeded
  
  ; Index into the shot array to set the initial X position
  ldi a shot_xs
  add a d

  ld b ship_position
  rstore a b

  ; And set the initial Y position, relative to the ship and going upward
  ldi a shot_ys
  add a d

  ldi b 0
  rstore a b

  ld d num_shots
  inc d
  store num_shots d

check_shoot_max_exceeded:
  jmp draw

check_shoot_released:
  ; The key is released -- allow a new shot to be fired
  ldi d 1
  store can_shoot d

draw:
  call clear_screen
  call draw_ship

  call move_enemies
  call draw_enemies

  call move_shots
  call check_shots
  call draw_shots

  ldi c 100
draw_loop:
  dec c
  jnz c draw_loop

  jmp main

  halt


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
clear_screen:
  ldi c 38400       ; Number of framebuffer words to clear
  ldi d 0           ; Clear color
  ldi a 0x400       ; Start address
clear_screen_loop:
  rstore a d        ; Write the clear color into the framebuffer
  inc a             ; Go to the next framebuffer word
  dec c             ; Count down number of words to clear
  jnz c clear_screen_loop
  ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Calling convention: 
; - glyph address is in pointer A
; - destination cursor is in pointer B
print_glyph:
  ; A font glyph comprises 8 words (7..0)
  ldi c 8           
print_glyph_loop:
  rld d a           ; Load the glyph data at pointer A into Data 
  rstore b d        ; Write the glyph word to the cursor position in B

  ; Advance the cursor to the next line
  ldi d 80
  add b d

  inc a             ; Go to the next glyph word
  dec c             ; count down glyph words remaining
  jnz c print_glyph_loop

print_glyph_done:
  ret ; print_glyph


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
draw_ship:
  ld  b ship_position_start
  ld  d ship_position
  add b d

  ldi a image_ship
  call print_glyph

  ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
draw_shots:
  ldi c 0
draw_shot:
  ld d num_shots
  je c d draw_shots_done

  ; Index into the shot array by the shot counter C
  ldi b shot_xs
  add b c
  rld a b

  ; Start the cursor at the ship's line
  ld b ship_position_start
  ; Add the shot's X position to the ship's line start
  add b a

  ; Load the Y position
  ; B holds the destination cursor
  ldi a shot_ys
  add a c
  rld d a

  push c    ; Save the count

  ; Multiply the Y position by the character width of the screen to wrap around
  ldi c 80
  mul d c
  ; And subtract that Y line position from the cursor to pull it toward the top
  sub b d

  ldi a image_shot
  call print_glyph
  
  ; Restore and increment the count
  pop c   
  inc c

  jmp draw_shot

draw_shots_done:
  ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Calling convention: shot index is in C
remove_shot:
  ; if we've reached the end of the list then skip to shortening it
  mov b c
  inc b
  ld d num_shots
  je b d remove_shot_decrement

  ; Get the next shot and replace the current one with it
  ldi a shot_ys
  add a b
  rld d a     ; next shot position in D
  ldi a shot_ys
  add a c
  rstore a d  ; write next shot position into current shot

  ldi a shot_xs
  add a b
  rld d a     ; next shot position in D
  ldi a shot_xs
  add a c
  rstore a d ; write next shot position into current shot

  ; Go to next shot
  inc c
  jmp remove_shot

remove_shot_decrement:
  ld d num_shots
  dec d
  store num_shots d

remove_shot_done:
  ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
check_shots:
  ldi c 0
check_shot:
  ld d num_shots
  je c d check_shots_done

  ; Index into the shot array by the shot counter
  ldi b shot_ys
  add b c
  rld d b

  ; Check if the shot is out of bounds
  ldi b 400
  jg d b check_shot_oob

  ; Check if the shot hit an enemy
  ldi b 0     ; B is the enemy counter, C is the shot counter
check_shot_enemy:
  ld d num_enemies 
  je b d check_shot_enemies_done

  ; D = enemy X position
  ldi a enemy_xs
  add a b
  rld d a
  ld a enemy_offset
  add d a

  ; A = shot X position
  ldi a shot_xs
  add a c
  rld a a

  ; If enemy X != shot X then skip enemy
  jne a d check_shot_next_enemy

  ; D = enemy Y position
  ldi a enemy_ys
  add a b
  rld d a

  ; A = shot Y position
  ldi a shot_ys
  add a c
  rld a a

  ; If shot is past enemy then skip enemy
  jg a d check_shot_next_enemy

  ; If shot is not yet to enemy then skip enemy
  push a
  ldi a 8
  sub d a
  pop a

  jg d a check_shot_next_enemy

  push c
  push b
  call remove_shot
  pop c
  call remove_enemy
  pop c
  jmp check_shot

check_shot_next_enemy:
  ; Continue to next enemy
  inc b
  jmp check_shot_enemy

check_shot_enemies_done:
  ; Continue to next shot
  inc c
  jmp check_shot

check_shots_done:
  ret

check_shot_oob:
  push c
  call remove_shot
  pop c
  ; Continue to next shot, which is now in the current slot
  jmp check_shot


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
move_shots:
  ldi c 0
move_shot:
  ld d num_shots
  je c d move_shots_done

  ; Index into the shot array by the shot counter C
  ldi b shot_ys
  add b c
  rld d b     ; Load the shot Y value

  ; Move the shot up and store its new position
  ldi a 10
  add d a
  rstore b d

  inc c

  jmp move_shot

move_shots_done:
  ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
move_enemies:
  ld c enemy_move_timer
  dec c
  jz c move_enemies_now
  store enemy_move_timer c
  ret
move_enemies_now:
  ld d enemy_offset
  ld b enemy_move_direction
  jz b move_enemies_right
move_enemies_left:
  dec d
  store enemy_offset d
  jz d move_enemies_go_right
  jmp move_enemies_reset
move_enemies_right:
  inc d
  store enemy_offset d
  ldi c 20
  je d c move_enemies_go_left
  jmp move_enemies_reset
move_enemies_down:
  ldi c 0
move_enemies_down_loop:
  ld d num_enemies
  je c d move_enemies_reset

  ldi b enemy_ys
  add b c
  rld a b

  ldi d 8
  sub a d
  rstore b a

  inc c
  jmp move_enemies_down_loop
move_enemies_reset:
  ldi c 5
  store enemy_move_timer c
  ret
move_enemies_go_right:
  ldi d 0
  store enemy_move_direction d
  jmp move_enemies_down
move_enemies_go_left:
  ldi d 1
  store enemy_move_direction d
  jmp move_enemies_down

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
draw_enemies:
  ldi c 0
draw_enemy:
  ld d num_enemies
  je c d draw_enemies_done

  ; Index into the shot array by the shot counter C
  ldi b enemy_xs
  add b c
  rld a b

  ; Start the cursor at the ship's line
  ld b ship_position_start
  ; Add the shot's X position to the ship's line start
  add b a
  ; Add the global enemy side-to-side shift
  ld a enemy_offset
  add b a

  ; Load the Y position
  ; B holds the destination cursor
  ldi a enemy_ys
  add a c
  rld d a

  push c    ; Save the count

  ; Multiply the Y position by the character width of the screen to wrap around
  ldi c 80
  mul d c
  ; And subtract that Y line position from the cursor to pull it toward the top
  sub b d

  ldi a image_enemy
  call print_glyph
  
  ; Restore and increment the count
  pop c   
  inc c

  jmp draw_enemy

draw_enemies_done:
  ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Calling convention: enemy index is in C
remove_enemy:
  ; if we've reached the end of the list then skip to shortening it
  mov b c
  inc b
  ld d num_enemies
  je b d remove_enemy_decrement

  ; Get the next enemy and replace the current one with it
  ldi a enemy_ys
  add a b
  rld d a     ; next enemy position in D
  ldi a enemy_ys
  add a c
  rstore a d  ; write next enemy position into current enemy

  ldi a enemy_xs
  add a b
  rld d a     ; next enemy position in D
  ldi a enemy_xs
  add a c
  rstore a d ; write next enemy position into current enemy

  ; Go to next enemy
  inc c
  jmp remove_enemy

remove_enemy_decrement:
  ld d num_enemies
  dec d
  store num_enemies d

remove_enemy_done:
  ret


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; DATA ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ship_position:
  word 25
ship_position_start:
  word 38110  
ship_position_max:
  word 50
can_shoot:
  word 1

num_shots:
  word 0
max_shots:
  word 20
shot_xs:
  string 00000000000000000000
shot_ys:
  string 00000000000000000000

num_enemies:
  word 12
enemy_xs:
  word 0
  word 10
  word 20
  word 30
  word 0
  word 10
  word 20
  word 30
  word 0
  word 10
  word 20
  word 30
enemy_ys:
  word 300
  word 300
  word 300
  word 300
  word 200
  word 200
  word 200
  word 200
  word 250
  word 250
  word 250
  word 250
enemy_offset:
  word 10
enemy_move_direction:
  word 0
enemy_move_timer:
  word 5


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;; IMAGES ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
image_ship:
  word 0b00111100
  word 0b00111100
  word 0b01111110
  word 0b11111111
  word 0b11111111
  word 0b01111110
  word 0b00111100
  word 0b01100110

image_shot:
  word 0b00011000
  word 0b00011000
  word 0b00111100
  word 0b01011010
  word 0b00011000
  word 0b00011000
  word 0b00111100
  word 0b00011000

image_enemy:
  word 0b10000001
  word 0b01000010
  word 0b01111110
  word 0b10011001
  word 0b10111101
  word 0b00101000
  word 0b01000010
  word 0b10000001