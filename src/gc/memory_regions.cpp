#include <Arduino.h>
#include "memory_regions.h"

#define MR_LOG_MESSAGES 0

MemRegion_t *memory_regions = nullptr;
size_t memory_regions_count = 0;
size_t memory_regions_capacity = 0;

void memory_regions_init()
{
  memory_regions_count = 0;
  memory_regions_capacity = 10;
  memory_regions = (MemRegion_t *)malloc(memory_regions_capacity * sizeof(MemRegion_t));
}

void print_all_memory_regions(const char *prefix = "", uint32_t new_region_start = 0)
{
  for (size_t i = 0; i < memory_regions_count; i++)
  {
    Serial.print("Region ");
    Serial.print(i);
    if (new_region_start == memory_regions[i].start)
    {
      Serial.print(prefix);
    }
    Serial.print(": Start = 0x");
    Serial.print(memory_regions[i].start, HEX);
    Serial.print(", Size = ");
    Serial.println(memory_regions[i].size);
  }
}

void add_memory_regions(uint32_t start, size_t size, bool atomic)
{
  if (memory_regions_count >= memory_regions_capacity)
  {
    // Resize the array
    memory_regions_capacity *= 2;
    memory_regions = (MemRegion_t *)realloc(memory_regions, memory_regions_capacity * sizeof(MemRegion_t));
  }
  if (memory_regions == nullptr)
  {
    // Handle allocation failure
    Serial.println("Failed to allocate memory for memory regions");
    abort();
  }
  memory_regions[memory_regions_count].start = start;
  memory_regions[memory_regions_count].size = size;
  memory_regions[memory_regions_count].flags = 0;
  if (atomic)
  {
    SET_ATOMIC_FLAG(memory_regions[memory_regions_count]);
  }

  memory_regions_count++;

  if (MR_LOG_MESSAGES)
  {
    print_all_memory_regions("(NEW)", start);
  }
}

void compact_memory_regions()
{
  for (size_t i = 0; i < memory_regions_count; i++)
  {
    if (memory_regions[i].start == 0 && memory_regions[i].size == 0)
    {
      // Shift remaining regions down
      for (size_t j = i; j < memory_regions_count - 1; j++)
      {
        memory_regions[j] = memory_regions[j + 1];
      }
      memory_regions_count--;
      i--; // Check the new region at this index
    }
  }

  if (MR_LOG_MESSAGES)
  {
    print_all_memory_regions();
  }
}

void mark_for_removal(uint32_t start)
{
  for (size_t i = 0; i < memory_regions_count; i++)
  {
    if (memory_regions[i].start == start)
    {
      memory_regions[i].start = 0;
      memory_regions[i].size = 0;

      if (MR_LOG_MESSAGES)
      {
        Serial.print("Marked for removal region at start address 0x");
        Serial.println(start, HEX);
      }

      return;
    }
  }

  if (MR_LOG_MESSAGES)
  {
    Serial.print("No region found to mark for removal at start address 0x");
    Serial.println(start, HEX);
  }
}