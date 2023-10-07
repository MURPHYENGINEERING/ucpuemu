#include "ucpu.h"
#include <stdlib.h>


size_t op_to_uip(uint32_t op)
{
  // Ignore the lowest 2 bits, which specify a register for certain instructions
  switch (op & 0b11111100)
  {
  case 0b00000000: return 6;    // HALT
  case 0b00000001: return 7;    // NOP
  case 0b00111000: return 8;    // LDI
  case 0b00101000: return 13;   // LD
  case 0b10111000: return 20;   // RLD
  case 0b00111100: return 27;   // MOV
  case 0b00101100: return 33;   // STORE
  case 0b10101100: return 40;   // RSTORE
  case 0b00100000: return 47;   // INC
  case 0b00100100: return 50;   // DEC
  case 0b00000100: return 53;   // ADD
  case 0b00001000: return 61;   // SUB
  case 0b00001100: return 69;   // MUL
  case 0b00010000: return 77;   // DIV
  case 0b00010100: return 85;   // AND
  case 0b00011000: return 93;   // OR
  case 0b00011100: return 101;   // XOR
  case 0b01101100: return 109;   // SHL
  case 0b01110000: return 117;  // SHR
  case 0b11000000: return 125;  // JMP
  case 0b11000100: return 128;  // JE
  case 0b11001000: return 140;  // JNE
  case 0b11001100: return 152;  // JG
  case 0b11010000: return 164;  // JZ
  case 0b11100000: return 171;  // JNZ
  case 0b10000000: return 178;  // CALL
  case 0b11110000: return 185;  // RET
  case 0b10000100: return 189;  // PUSH
  case 0b10001000: return 193;  // POP
  }
  return 5; // UD_FAULT
}