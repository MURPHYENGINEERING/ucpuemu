#include "ucpu.h"
#include "gfx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


uint32_t mem[MEM_SIZE];


int main(int argc, char* argv[])
{
  printf("Hello, ucpu!\n\n");

  UCPU cpu;

  if (!load_rom(mem, "prog.rom")) {
    return -1;
  }

  reset(&cpu);

  CPU_Window wnd;
  wnd.title = "uCPUEMU - Matt Murphy";
  wnd.width = 640;
  wnd.height = 480;
  if (!window_create(&wnd)) {
    return -1;
  }

  while (!(cpu.sig & SIG_halt)) {
    clock_cpu(&cpu, mem, &wnd);
  }

  return 0;
}


void reset(UCPU* cpu)
{
  memset(cpu, 0, sizeof(UCPU));
  cpu->sp = MEM_SIZE-1;
}

size_t draw_skip = 0;

void clock_cpu(UCPU* cpu, uint32_t* mem, CPU_Window* wnd)
{
  // Look up the signal encoded by the current microcode instruction
  cpu->sig = urom[cpu->uip];

  // OUT to the bus
  if (cpu->sig & SIG_halt)  { return; }
  if (cpu->sig & SIG_fetch) { 
    cpu->uip = 0; 
    if (draw_skip++ > 10000) {
      if (!window_process_events(wnd, mem)) {
        cpu->sig = SIG_halt;
        return;
      }
      window_draw(wnd, mem); 
      draw_skip = 0;
    }
    return; 
  }
  if (cpu->sig & SIG_id2uip) { cpu->uip  = op_to_uip(cpu->ir); return; }
  if (cpu->sig & SIG_BCpc)   { cpu->dbus = cpu->pc; }
  if (cpu->sig & SIG_BCmem)  { cpu->dbus = mem[cpu->mar]; }
  if (cpu->sig & SIG_BCsp)   { cpu->dbus = cpu->sp; }
  if (cpu->sig & SIG_BCr)    { cpu->dbus = cpu->regs[cpu->r]; }
  if (cpu->sig & SIG_BCs)    { cpu->dbus = cpu->s; }
  if (cpu->sig & SIG_BCz)    { cpu->dbus = cpu->z; }
  if (cpu->sig & SIG_BCreg)  { cpu->dbus = cpu->regs[cpu->reg]; }

  // ALU
  cpu->z = 0;
  if (cpu->sig & SIG_aluinc) { cpu->z = cpu->dbus + 1; }
  if (cpu->sig & SIG_aludec) { cpu->z = cpu->dbus - 1; }
  if (cpu->sig & SIG_aluadd) { cpu->z = cpu->dbus + cpu->s; }
  if (cpu->sig & SIG_alusub) { cpu->z = cpu->dbus - cpu->s; }
  if (cpu->sig & SIG_alumul) { cpu->z = cpu->dbus * cpu->s; }
  if (cpu->sig & SIG_aludiv) { cpu->z = cpu->dbus / cpu->s; }
  if (cpu->sig & SIG_aluand) { cpu->z = cpu->dbus & cpu->s; }
  if (cpu->sig & SIG_aluor)  { cpu->z = cpu->dbus | cpu->s; }
  if (cpu->sig & SIG_aluxor) { cpu->z = cpu->dbus ^ cpu->s; }
  if (cpu->sig & SIG_alueq)  { cpu->z = (cpu->dbus == cpu->s) ? cpu->cmp : cpu->ncmp; }
  if (cpu->sig & SIG_alugr)  { cpu->z = (cpu->dbus > cpu->s) ? cpu->cmp : cpu->ncmp; }
  if (cpu->sig & SIG_aluz)   { cpu->z = (cpu->dbus == 0) ? cpu->cmp : cpu->ncmp; }
  if (cpu->sig & SIG_alushl) { cpu->z = cpu->dbus << cpu->s; }
  if (cpu->sig & SIG_alushr) { cpu->z = cpu->dbus >> cpu->s; }

  // IN from the bus
  if (cpu->sig & SIG_MEMen) { mem[cpu->mar] = cpu->dbus; }
  if (cpu->sig & SIG_MARen) { cpu->mar = cpu->dbus; }
  if (cpu->sig & SIG_PCen)  { cpu->pc = cpu->dbus; }
  if (cpu->sig & SIG_IRen)  { cpu->ir = cpu->dbus; cpu->reg = cpu->ir & 0b11; }
  if (cpu->sig & SIG_Zen)   { /*cpu->z = cpu->aluout;*/ }
  if (cpu->sig & SIG_REGen) { cpu->regs[cpu->reg] = cpu->dbus; }
  if (cpu->sig & SIG_Sen)   { cpu->s = cpu->dbus; }
  if (cpu->sig & SIG_Ren)   { cpu->r = cpu->dbus; }
  if (cpu->sig & SIG_CMPen) { cpu->cmp = cpu->dbus; }
  if (cpu->sig & SIG_NCMPen){ cpu->ncmp = cpu->dbus; }
  if (cpu->sig & SIG_push)  { --cpu->sp; }
  if (cpu->sig & SIG_pop)   { ++cpu->sp; }

  //printf("\n");

  //dump_cpu(cpu);

  // Iterate the microinstruction machine
  ++cpu->uip;
}


int load_rom(uint32_t* mem, char* path)
{
  memset(mem, 0, MEM_SIZE * sizeof(uint32_t));

  FILE* f = fopen(path, "r");
  if (!f) {
    printf("Invalid ROM path: %s\n", path);
    return 0;
  }

  size_t nread = fread(mem, 4, MEM_SIZE, f);
  if (nread == MEM_SIZE) {
    printf("ROM file overflowed memory!\n");
    return 0;
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

  return 1;
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
