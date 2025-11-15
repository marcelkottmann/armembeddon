#ifndef GC_GC_H
#define GC_GC_H

#include <stdint.h>
#include <Arduino.h>

typedef struct
{
  uint32_t r0;
  uint32_t r1;
  uint32_t r2;
  uint32_t r3;
  uint32_t r4;
  uint32_t r5;
  uint32_t r6;
  uint32_t r7;
  uint32_t r8;
  uint32_t r9;
  uint32_t r10;
  uint32_t r11;
  uint32_t r12;
  uint32_t sp;
} ArmRegisters_t;

void gc_init();
void *gc_malloc(uint32_t size);
void *gc_malloc_atomic(uint32_t size);
void gc_free(void* p);
void *gc_realloc(void* p, uint32_t size);
void capture_registers(ArmRegisters_t *regs);
void run_gc();
void print_current_state();

#endif // GC_GC_H
