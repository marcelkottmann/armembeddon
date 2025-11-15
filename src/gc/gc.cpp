
#include <Arduino.h>
#include "gc.h"
#include "memory_regions.h"

#define GC_LOG_MESSAGES 0
#define GC_LOG_DURATION 0

uint32_t aggregate_gc_time = 0;

int my_posix_memalign(void **memptr, size_t alignment, size_t size) {
    if ((alignment & (alignment - 1)) != 0 || alignment < sizeof(void *))
        return 22; // EINVAL

    // allocate enough space for alignment + original pointer
    void *raw = malloc(size + alignment - 1 + sizeof(void *));
    if (!raw)
        return 12; // ENOMEM

    uintptr_t raw_addr = (uintptr_t)raw + sizeof(void *);
    uintptr_t aligned_addr = (raw_addr + alignment - 1) & ~(uintptr_t)(alignment - 1);

    ((void **)aligned_addr)[-1] = raw;  // store original malloc pointer
    *memptr = (void *)aligned_addr;
    return 0;
}

void my_aligned_free(void *ptr) {
    if (ptr)
        free(((void **)ptr)[-1]);
}

static inline void print_hex(uint32_t n)
{
    Serial.print("0x");
    Serial.println(n, 16);
}

void gc_init()
{
    memory_regions_init();
}

void capture_registers(ArmRegisters_t *regs)
{
    __asm volatile(
        // R0-R12 are general purpose. We use the MOV instruction to move them
        // into the C variables which are represented by the output constraints (%0, %1, etc.).
        "mov %0, r0 \n"
        "mov %1, r1 \n"
        "mov %2, r2 \n"
        "mov %3, r3 \n"
        "mov %4, r4 \n"
        "mov %5, r5 \n"
        "mov %6, r6 \n"
        "mov %7, r7 \n"
        "mov %8, r8 \n"
        "mov %9, r9 \n"
        "mov %10, r10 \n"
        "mov %11, r11 \n"
        "mov %12, r12 \n"
        "mov %13, sp \n"

        : "=r"(regs->r0), "=r"(regs->r1), "=r"(regs->r2), "=r"(regs->r3),
          "=r"(regs->r4), "=r"(regs->r5), "=r"(regs->r6), "=r"(regs->r7),
          "=r"(regs->r8), "=r"(regs->r9), "=r"(regs->r10), "=r"(regs->r11),
          "=r"(regs->r12), "=r"(regs->sp));
}

void gc_free(void *p)
{
    mark_for_removal((uint32_t)p);
    // don't compact to not mess when gc_free was called in a loop
    my_aligned_free(p);

    if (GC_LOG_MESSAGES)
    {
        Serial.print("Free: address ");
        print_hex((uint32_t)p);
    }
}

void run_gc()
{
#if GC_LOG_DURATION
    uint32_t start = micros();
#endif

    ArmRegisters_t regs;
    capture_registers(&regs);

    for (size_t i = 0; i < memory_regions_count; i++)
    {
        CLEAR_IN_USE_FLAG(memory_regions[i]);

        if (!IS_ACTIVE_REGION(memory_regions[i]))
        {
            continue;
        }

        uint32_t region_start = memory_regions[i].start;
        if (
            regs.r0 == region_start ||
            regs.r1 == region_start ||
            regs.r2 == region_start ||
            regs.r3 == region_start ||
            regs.r4 == region_start ||
            regs.r5 == region_start ||
            regs.r6 == region_start ||
            regs.r7 == region_start ||
            regs.r8 == region_start ||
            regs.r9 == region_start ||
            regs.r10 == region_start ||
            regs.r11 == region_start ||
            regs.r12 == region_start)
        {
            SET_IN_USE_FLAG(memory_regions[i]);

            if (GC_LOG_MESSAGES)
            {
                Serial.print("Found global pointer to region ");
                Serial.print(i);
                Serial.println(" in register");
            }
        }
    }

    uint32_t stack_base = 0x2000FFFF;
    uint32_t addr = (uint32_t)regs.sp;
    while (addr <= stack_base)
    {
        uint32_t potential_ptr = *((uint32_t *)addr);
        for (size_t i = 0; i < memory_regions_count; i++)
        {
            if (!IS_ACTIVE_REGION(memory_regions[i]))
            {
                continue;
            }

            uint32_t region_start = memory_regions[i].start;
            if (potential_ptr == region_start)
            {
                SET_IN_USE_FLAG(memory_regions[i]);

                if (GC_LOG_MESSAGES)
                {
                    Serial.print("Found global pointer to region ");
                    Serial.print(i);
                    Serial.print(" at stack address ");
                    print_hex(addr);
                }
            }
        }

        addr += 4;
    }

    /* globals have now been marked.
        search for occurence of non marked region
        start pointer in memory addresses of marked regions:
    */
    uint32_t new_references_found = ~0;
    while (new_references_found > 0)
    {
        new_references_found = 0;
        for (size_t m = 0; m < memory_regions_count; m++)
        {
            if (!IS_ACTIVE_REGION(memory_regions[m]) || HAS_ATOMIC_FLAG(memory_regions[m]))
            {
                continue;
            }

            if (HAS_IN_USE_FLAG(memory_regions[m]))
            {
                for (size_t u = 0; u < memory_regions_count; u++)
                {
                    if (!IS_ACTIVE_REGION(memory_regions[u]))
                    {
                        continue;
                    }

                    if (!HAS_IN_USE_FLAG(memory_regions[u]))
                    {
                        uint32_t unmarked_region_start = memory_regions[u].start;
                        for (uint32_t p = memory_regions[m].start + memory_regions[m].size - 4; p >= memory_regions[m].start; p -= 4)
                        {
                            uint32_t val_at_p = *(uint32_t *)p;
                            if (unmarked_region_start == val_at_p)
                            {
                                SET_IN_USE_FLAG(memory_regions[u]);
                                new_references_found++;

                                if (GC_LOG_MESSAGES)
                                {
                                    Serial.print("Found pointer to region ");
                                    Serial.print(u);
                                    Serial.print(" inside region ");
                                    Serial.print(m);
                                    Serial.print(" at memory address ");
                                    print_hex(p);
                                }
                            }
                        }
                    }
                }
            }
        }

        if (GC_LOG_MESSAGES)
        {
            Serial.print("Number of new found references: ");
            Serial.println(new_references_found);
        }
    }

    for (size_t u = 0; u < memory_regions_count; u++)
    {
        if (!IS_ACTIVE_REGION(memory_regions[u]))
        {
            continue;
        }

        if (!HAS_IN_USE_FLAG(memory_regions[u]))
        {
            if (GC_LOG_MESSAGES)
            {
                Serial.print("Region ");
                Serial.print(u);
                Serial.println(" is unreferenced and can be freed.");
            }

            gc_free((void *)memory_regions[u].start);
        }
    }
    compact_memory_regions();

#if GC_LOG_DURATION
    uint32_t end = micros() - start;
    aggregate_gc_time += end;
    Serial.print("GC run time (us): ");
    Serial.println(end);
    Serial.print("Aggregated GC run time (us): ");
    Serial.println(aggregate_gc_time);
#endif
}

void print_current_state()
{
    ArmRegisters_t regs;
    capture_registers(&regs);

    Serial.print("r0: ");
    print_hex((uint32_t)regs.r0);
    Serial.print("r1: ");
    print_hex((uint32_t)regs.r1);
    Serial.print("r2: ");
    print_hex((uint32_t)regs.r2);
    Serial.print("r3: ");
    print_hex((uint32_t)regs.r3);
    Serial.print("r4: ");
    print_hex((uint32_t)regs.r4);
    Serial.print("r5: ");
    print_hex((uint32_t)regs.r5);
    Serial.print("r6: ");
    print_hex((uint32_t)regs.r6);
    Serial.print("r7: ");
    print_hex((uint32_t)regs.r7);
    Serial.print("r8: ");
    print_hex((uint32_t)regs.r8);
    Serial.print("r9: ");
    print_hex((uint32_t)regs.r9);
    Serial.print("r10: ");
    print_hex((uint32_t)regs.r10);
    Serial.print("r11: ");
    print_hex((uint32_t)regs.r11);
    Serial.print("r12: ");
    print_hex((uint32_t)regs.r12);

    Serial.print("Current SP: ");
    print_hex((uint32_t)regs.sp);
}

void *gc_malloc(uint32_t size, bool atomic)
{
    run_gc();

    // void *p = malloc(size);
    void *p;
    my_posix_memalign(&p, 16, size); 

    if (GC_LOG_MESSAGES)
    {
        Serial.print("Malloc: Size ");
        Serial.print(size);
        Serial.print(", address ");
        print_hex((uint32_t)p);
    }

    add_memory_regions((uint32_t)p, size, atomic);

    return p;
}

void *gc_malloc(uint32_t size)
{
    return gc_malloc(size, false);
}

void *gc_malloc_atomic(uint32_t size)
{
    return gc_malloc(size, true);
}

void *gc_realloc(void *p, uint32_t size)
{
    if(p==nullptr)
    {
        return gc_malloc(size);
    } 

    MemRegion_t *found_region = nullptr;
    for (size_t i = 0; i < memory_regions_count; i++)
    {
        if (memory_regions[i].start == (uint32_t)p)
        {
            found_region = &memory_regions[i];
            break;
        }
    }

    if (found_region == nullptr)
    {
        Serial.println("Realloc failed: region not found");
        abort();
    }

    // find_globals();
    void *newp = realloc(p, size);

    if (size > 0 && newp == nullptr)
    {
        Serial.print("Realloc failed: realloc returned nullptr. Requested size: ");
        Serial.println(size);
        abort();
    }

    found_region->size = size;
    found_region->start = (uint32_t)newp;

    if (GC_LOG_MESSAGES)
    {
        Serial.print("Realloc: Size ");
        Serial.print(size);
        Serial.print(", address ");
        print_hex((uint32_t)newp);
    }

    if (size == 0)
    {
        mark_for_removal((uint32_t)p);
        compact_memory_regions();
    }

    return newp;
}