#include "ucpu.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main(int argc, char* argv[])
{
  printf("Hello, ucpu!\n\n");

  UCPU cpu;
  uint32_t mem[MEM_SIZE];

  if (load_rom(mem, "prog.rom") < 0) {
    return 1;
  }

  reset(&cpu);

  while (!(cpu.sig & SIG_halt)) {
    clock(&cpu, mem);
  }

  return 0;
}


void reset(UCPU* cpu)
{
  memset(cpu, 0, sizeof(UCPU));
  cpu->sp = MEM_SIZE-1;
}


void fetch(UCPU* cpu, uint32_t* mem)
{
  cpu->ir = mem[cpu->pc];
  cpu->uip = op_to_uip(cpu->ir);
}


void clock(UCPU* cpu, uint32_t* mem)
{
  // Fetch the instruction at PC
  //cpu->ir = urom[cpu->pc];
  

  // Look up the signal encoded by the current microcode instruction
  cpu->sig = urom[cpu->uip];

  // OUT to the bus
  if (cpu->sig & SIG_halt)  { printf("halt\n"); return; }
  if (cpu->sig & SIG_fetch) { printf("gofetch\n"); cpu->uip = 0; return; }
  if (cpu->sig & SIG_id2uip) { printf("id2uip\n"); cpu->uip = op_to_uip(cpu->ir); return; }
  if (cpu->sig & SIG_BCpc)  { cpu->dbus = cpu->pc; printf("BCpc "); }
  if (cpu->sig & SIG_BCmem) { cpu->dbus = mem[cpu->mar]; printf("BCmem "); }
  if (cpu->sig & SIG_BCsp)  { cpu->dbus = cpu->sp; printf("BCsp "); }
  if (cpu->sig & SIG_BCr)   { cpu->dbus = cpu->regs[cpu->r]; printf("BCr "); }
  if (cpu->sig & SIG_BCs)   { cpu->dbus = cpu->s; printf("BCs "); }
  if (cpu->sig & SIG_BCz)   { cpu->dbus = cpu->z; printf("BCz "); }
  if (cpu->sig & SIG_BCreg) { cpu->dbus = cpu->regs[cpu->reg]; printf("BCreg "); }

  // ALU
  cpu->z = 0;
  if (cpu->sig & SIG_aluinc) { cpu->z = cpu->dbus + 1; printf("aluinc "); }
  if (cpu->sig & SIG_aludec) { cpu->z = cpu->dbus - 1; printf("aludec "); }
  if (cpu->sig & SIG_aluadd) { cpu->z = cpu->dbus + cpu->s; printf("aluadd "); }
  if (cpu->sig & SIG_alusub) { cpu->z = cpu->dbus - cpu->s; printf("alusub "); }
  if (cpu->sig & SIG_alumul) { cpu->z = cpu->dbus * cpu->s; printf("alumul "); }
  if (cpu->sig & SIG_aludiv) { cpu->z = cpu->dbus / cpu->s; printf("aludiv "); }
  if (cpu->sig & SIG_aluand) { cpu->z = cpu->dbus & cpu->s; printf("aluand "); }
  if (cpu->sig & SIG_aluor)  { cpu->z = cpu->dbus | cpu->s; printf("aluor "); }
  if (cpu->sig & SIG_aluxor) { cpu->z = cpu->dbus ^ cpu->s; printf("aluxor "); }
  if (cpu->sig & SIG_alueq)  { cpu->z = (cpu->dbus == cpu->s) ? cpu->cmp : cpu->ncmp; printf("alueq "); }
  if (cpu->sig & SIG_alugr)  { cpu->z = (cpu->dbus > cpu->s) ? cpu->cmp : cpu->ncmp; printf("alugr "); }
  if (cpu->sig & SIG_aluz)   { cpu->z = (cpu->dbus == 0) ? cpu->cmp : cpu->ncmp; printf("aluz "); }
  if (cpu->sig & SIG_alushl) { cpu->z = cpu->dbus << cpu->s; printf("alushl "); }
  if (cpu->sig & SIG_alushr) { cpu->z = cpu->dbus >> cpu->s; printf("alushr "); }

  // IN from the bus
  if (cpu->sig & SIG_MEMen) { mem[cpu->mar] = cpu->dbus; printf("MEMen "); }
  if (cpu->sig & SIG_MARen) { cpu->mar = cpu->dbus; printf("MARen "); }
  if (cpu->sig & SIG_PCen)  { cpu->pc = cpu->dbus; printf("PCen "); }
  if (cpu->sig & SIG_IRen)  { cpu->ir = cpu->dbus; cpu->reg = cpu->ir & 0b11; printf("IRen "); }
  if (cpu->sig & SIG_Zen)   { /*cpu->z = cpu->aluout;*/ printf("Zen "); }
  if (cpu->sig & SIG_REGen) { cpu->regs[cpu->reg] = cpu->dbus; printf("REGen "); }
  if (cpu->sig & SIG_Sen)   { cpu->s = cpu->dbus; printf("Sen "); }
  if (cpu->sig & SIG_Ren)   { cpu->r = cpu->dbus; printf("Ren "); }
  if (cpu->sig & SIG_CMPen) { cpu->cmp = cpu->dbus; printf("CMPen "); }
  if (cpu->sig & SIG_NCMPen){ cpu->ncmp = cpu->dbus; printf("NCMPen "); }
  if (cpu->sig & SIG_push)  { --cpu->sp; printf("push "); }
  if (cpu->sig & SIG_pop)   { ++cpu->sp; printf("pop "); }

  printf("\n");

  dump_cpu(cpu);

  // Iterate the microinstruction machine
  ++cpu->uip;
}


int load_rom(uint32_t* mem, char* path)
{
  memset(mem, 0, MEM_SIZE * sizeof(uint32_t));

  FILE* f = fopen(path, "r");
  if (!f) {
    printf("Invalid ROM path: %s\n", path);
    return -1;
  }

  size_t nread = fread(mem, 4, MEM_SIZE, f);
  if (nread == MEM_SIZE) {
    printf("ROM file overflowed memory!\n");
    return -1;
  }
  printf("Read %lu words from ROM file (%s)\n", nread, path);

  printf("---------------------------------------");
  for (size_t memp = 0; memp < MEM_SIZE; ++memp) {
    if (memp % 8 == 0) {
      printf("\n");
    }
    printf("%04x ", mem[memp]);
  }
  printf("\n---------------------------------------\n\n");

  return 0;
}


void dump_cpu(UCPU* cpu)
{
  printf("---------------------------------------\n");
  printf("  PC: %04x\t IR: %04x\tUIP: %lu\n", cpu->pc, cpu->ir, cpu->uip);
  printf("DBUS: %04x\tMAR: %04x\n", cpu->dbus, cpu->mar);
  printf("   S: %04x\t  Z: %04x\n", cpu->s, cpu->z);
  printf(" CMP: %04x\tNMP: %04x\n", cpu->cmp, cpu->ncmp);
  printf("   R: %04x\tREG: %04x\n", cpu->r, cpu->reg);
  printf("  R0: %04x\t R1: %04x\n  R2: %04x\t R3: %04x\n", 
    cpu->regs[0], cpu->regs[1], cpu->regs[2], cpu->regs[3]);
  printf("---------------------------------------\n\n");
}
