#ifndef _lang_memory_c
#define _lang_memory_c

#include "common.h"

#define ALLOCATE(type, count)                                                  \
  (type *)reallocate(NULL, 0, sizeof((type)) * (count))

#define FREE(type, pointer) reallocate((pointer), sizeof(type), 0)

#define DYN_ARR_GROW_CAPACITY(capacity) ((capacity) < 8 ? 8 : (capacity) * 2)

#define DYN_ARR_GROW(type, entries, new_count)                                 \
  (type *)reallocate(entries, sizeof(type) * (new_count))

#define DYN_ARR_FREE(arr) reallocate(arr, 0)

#define DYN_ARR_PUSH(type, arr, entry)                                         \
  do {                                                                         \
    if ((arr)->capacity < (arr)->count + 1) {                                  \
      (arr)->capacity = DYN_ARR_GROW_CAPACITY((arr)->capacity);                \
      (arr)->entries = DYN_ARR_GROW(type, (arr)->entries, (arr)->capacity);    \
    }                                                                          \
    (arr)->entries[(arr)->count] = entry;                                      \
    (arr)->count++;                                                            \
  } while (0)

void *reallocate(void *pointer, size_t new_size);

#endif
