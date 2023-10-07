#ifndef UCPU_H
#define UCPU_H

#include "gfx.h"

#include <stdint.h>
#include <stdlib.h>

#define N_REGISTERS 4
#define MEM_SIZE 32

typedef struct {
  // Program Counter register
  uint32_t pc;
  // Instruction register
  uint32_t ir;
  // Data bus register
  uint32_t dbus;
  // Memory address
  uint32_t mar;

  // ALU second input register; first input is DBus.
  uint32_t s;
  // ALU result
  uint32_t z;
  // Compare success result
  uint32_t cmp;
  // Compare failed result
  uint32_t ncmp;

  // General purpose registers
  uint32_t regs[N_REGISTERS];
  // Register pointers: indices into REGS
  // Register index indicated by the instruction
  uint8_t reg;
  // Register index loaded from data
  uint32_t r;

  // Stack pointer
  uint32_t sp;

  // Microinstruction pointer
  size_t uip;
  // Decoded microinstruction (from UIP)
  uint64_t sig;

} UCPU;


/*******************************************************************************
 * Memory
 */

// Load a program ROM file into memory
int load_rom(uint32_t* mem, char* path);


/*******************************************************************************
 * CPU
 */

// Print out the contents of the CPU
void dump_cpu(UCPU* cpu);
// Reset the CPU to its initial state (zero all)
void reset(UCPU* cpu);
// Execute one clock cycle, iterating the microcode machine
void clock_cpu(UCPU* cpu, uint32_t* mem, CPU_Window* wnd);

/*******************************************************************************
 * Microcode
 */

// Stores the microcode instructions that drive the CPU
extern uint64_t urom[];
extern uint32_t mem[];

// Map an opcode to an offset into the microcode ROM where that opcode's
// implementation begins.
size_t op_to_uip(uint32_t op);

// Microcode signals as encoded by microcode instructions
#define SIG_BCpc   0b100000000000000000000000000000000000
#define SIG_BCmem  0b010000000000000000000000000000000000
#define SIG_BCz    0b001000000000000000000000000000000000
#define SIG_BCreg  0b000100000000000000000000000000000000
#define SIG_BCsp   0b000010000000000000000000000000000000
#define SIG_BCs    0b000001000000000000000000000000000000
#define SIG_BCr    0b000000100000000000000000000000000000
#define SIG_MARen  0b000000010000000000000000000000000000
#define SIG_PCen   0b000000001000000000000000000000000000
#define SIG_IRen   0b000000000100000000000000000000000000
#define SIG_Zen    0b000000000010000000000000000000000000
#define SIG_REGen  0b000000000001000000000000000000000000
#define SIG_MEMen  0b000000000000100000000000000000000000
#define SIG_Sen    0b000000000000010000000000000000000000
#define SIG_Ren    0b000000000000001000000000000000000000
#define SIG_CMPen  0b000000000000000100000000000000000000
#define SIG_NCMPen 0b000000000000000010000000000000000000
#define SIG_push   0b000000000000000001000000000000000000
#define SIG_pop    0b000000000000000000100000000000000000
#define SIG_aluinc 0b000000000000000000010000000000000000
#define SIG_aludec 0b000000000000000000001000000000000000
#define SIG_aluadd 0b000000000000000000000100000000000000
#define SIG_alusub 0b000000000000000000000010000000000000
#define SIG_alumul 0b000000000000000000000001000000000000
#define SIG_aludiv 0b000000000000000000000000100000000000
#define SIG_aluand 0b000000000000000000000000010000000000
#define SIG_aluor  0b000000000000000000000000001000000000
#define SIG_aluxor 0b000000000000000000000000000100000000
#define SIG_alueq  0b000000000000000000000000000010000000
#define SIG_alugr  0b000000000000000000000000000001000000
#define SIG_aluz   0b000000000000000000000000000000100000
#define SIG_alushl 0b000000000000000000000000000000010000
#define SIG_alushr 0b000000000000000000000000000000001000
#define SIG_id2uip 0b000000000000000000000000000000000100
#define SIG_fetch  0b000000000000000000000000000000000010
#define SIG_halt   0b000000000000000000000000000000000001

#endif