#ifndef _LANG_MEMORY_H
#define _LANG_MEMORY_H

#include "common.h"

#define ALLOCATE(type, count)                                                  \
  (type *)reallocate(NULL, 0, sizeof(type) * (count))

#define FREE(type, pointer) reallocate((pointer), sizeof(type), 0)

#define DYN_ARR_GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)

#define DYN_ARR_GROW(type, entries, old_count, new_count)                      \
  (type *)reallocate(entries, sizeof(type) * (old_count),                      \
                     sizeof(type) * (new_count))

#define DYN_ARR_FREE(type, pointer, count)                                     \
  reallocate(pointer, sizeof(type) * (count), 0)

#define DYN_ARR_PUSH(type, pointer, entry)                                     \
  do {                                                                         \
    if ((pointer)->capacity < (pointer)->count + 1) {                          \
      size_t old_capacity = (pointer)->capacity;                               \
      (pointer)->capacity = DYN_ARR_GROW_CAPACITY(old_capacity);               \
      (pointer)->entries = DYN_ARR_GROW(type, (pointer)->entries,              \
                                        old_capacity, (pointer)->capacity);    \
    }                                                                          \
    (pointer)->entries[(pointer)->count] = entry;                              \
    (pointer)->count++;                                                        \
  } while (0)

void *reallocate(void *pointer, size_t old_size, size_t new_size);

#endif
