#ifndef GC_MEMORY_REGIONS_H
#define GC_MEMORY_REGIONS_H

#include <stddef.h>
#include <stdint.h>

// in use flag means: while searching for regions to be garbage collected
// the region was detected to be still in use (i.e. can not be collected) 
#define CLEAR_IN_USE_FLAG(region) ((region).flags &= ~0x1)
#define SET_IN_USE_FLAG(region) ((region).flags |= 0x1)
#define HAS_IN_USE_FLAG(region) (((region).flags & 0x1) != 0)

// in use flag means: while searching for regions to be garbage collected
// the region was detected to be still in use (i.e. can not be collected) 
#define SET_ATOMIC_FLAG(region) ((region).flags |= 0x2)
#define HAS_ATOMIC_FLAG(region) (((region).flags & 0x2) != 0)

// active region means: region size > 0. regions can be marked for deletion
// by setting size to 0. Such regions are ignored in searches.
#define IS_ACTIVE_REGION(region) ((region).size > 0)

typedef struct
{
  uint32_t start;
  size_t size;
  uint32_t flags;
} MemRegion_t;

extern MemRegion_t *memory_regions;
extern size_t memory_regions_count;
extern size_t memory_regions_capacity;

void memory_regions_init();
void add_memory_regions(uint32_t start, size_t size, bool atomic);
void mark_for_removal(uint32_t start);
void compact_memory_regions();

#endif // GC_MEMORY_REGIONS_H
