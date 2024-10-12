#include <pthread.h>    // For thread safety (optional)
#include <stdatomic.h>  // For atomic operations (optional, C11 or later)
#include <stdlib.h>   // For malloc, free
#include <stddef.h>   // For size_t
#include <stdio.h>    // For optional debugging (e.g., printf)

typedef struct {
    void *memory;    // Pointer to the beginning of the memory pool
    size_t size;     // Total size of the arena
    size_t offset;   // Offset from the beginning (tracks where free memory starts)
} arena_t;

void* arena_alloc(arena_t *arena, size_t size);
void* arena_alloc_aligned(arena_t *arena, size_t size, size_t alignment);
void* arena_alloc_thread_safe(arena_t *arena, size_t size, size_t alignment);
void arena_reset(arena_t *arena);
void arena_destroy(arena_t *arena);
void* arena_alloc_aligned(arena_t *arena, size_t size, size_t alignment);
int allocation(arena_t *my_arena) ;
int more_allocation(arena_t *my_arena);


pthread_mutex_t lock; // Mutex for thread safety

arena_t* arena_create(size_t size) {
    arena_t *arena = (arena_t*)malloc(sizeof(arena_t));
    if (!arena) return NULL;  // Error handling: unable to allocate arena structure

    arena->memory = malloc(size);
    if (!arena->memory) {
        free(arena);
        return NULL;  // Error handling: unable to allocate memory pool
    }

    arena->size = size;
    arena->offset = 0;  // Initially, the entire arena is free
    
    pthread_mutex_init(&lock, NULL);  // Initialize the mutex
    
    return arena;
}

void* arena_alloc_thread_safe(arena_t *arena, size_t size, size_t alignment) {
    pthread_mutex_lock(&lock);
    void *ptr = arena_alloc_aligned(arena, size, alignment);
    pthread_mutex_unlock(&lock);
    return ptr;
}

/* void* arena_alloc(arena_t *arena, size_t size) {
    pthread_mutex_lock(&lock);  // Lock the mutex

    if (arena->offset + size > arena->size) {
        pthread_mutex_unlock(&lock);  // Unlock before returning
        return NULL;  // Error handling: out of memory
    }

    void *ptr = (void*)((char*)arena->memory + arena->offset);
    arena->offset += size;  // Move the offset by the requested size

    pthread_mutex_unlock(&lock);  // Unlock the mutex

    return ptr;
}
*/

void arena_reset(arena_t *arena) {
    pthread_mutex_lock(&lock);  // Lock the mutex
    arena->offset = 0;  // All memory can be reused, but it is not cleared
    pthread_mutex_unlock(&lock);  // Unlock the mutex
}


void arena_destroy(arena_t *arena) {
    pthread_mutex_destroy(&lock);  // Destroy the mutex
    free(arena->memory);
    free(arena);
}

void* arena_alloc_aligned(arena_t *arena, size_t size, size_t alignment) {
    pthread_mutex_lock(&lock);  // Lock the mutex

    size_t current_offset = arena->offset;
    size_t aligned_offset = (current_offset + alignment - 1) & ~(alignment - 1); // Align to the nearest multiple

    if (aligned_offset + size > arena->size) {
        pthread_mutex_unlock(&lock);  // Unlock before returning
        return NULL;  // Error handling: out of memory
    }

    void *ptr = (void*)((char*)arena->memory + aligned_offset);
    arena->offset = aligned_offset + size;

    pthread_mutex_unlock(&lock);  // Unlock the mutex
    return ptr;
}

int allocation(arena_t *my_arena) {
    size_t alloc_size = 0;  // Size of the allocation in bytes
    size_t alignment = 8;   // Define the required alignment
    printf("Enter size of memory to allocate: ");
    
    scanf("%zu", &alloc_size); 

    void *ptr1 = arena_alloc_aligned(my_arena, alloc_size, alignment);
    if (ptr1) {
        printf("Allocated %zu bytes from the arena.\n", alloc_size);
        return 0; // Success
    } else {
        printf("Failed to allocate memory from the arena.\n");
        printf("Please try again with a smaller size.\n");
        return -1; // Failure
    }
}

int more_allocation(arena_t *my_arena) {
    size_t alloc_size = 0;
    size_t alignment = 8;
    printf("Enter size of new memory to allocate: ");

    scanf("%zu", &alloc_size); 

    void *ptr2 = arena_alloc_aligned(my_arena, alloc_size, alignment);
    if (ptr2) {
        printf("Allocated another %zu bytes from the arena.\n", alloc_size);
        return 0; // Success
    } else {
        printf("Failed to allocate memory from the arena.\n");
        return -1; // Failure
    }
}

int main() {
    pthread_mutex_t lock; // Mutex for thread safety

    pthread_mutex_init(&lock, NULL);
    
    arena_t *my_arena = arena_create(1024);  // Create a 1024-byte arena

    int attempts = 3;  // Allow up to 3 attempts before giving up
    while (allocation(my_arena) != 0 && attempts-- > 0) {
        printf("Retrying allocation...\n");
    }

    if (attempts <= 0) {
        printf("Memory allocation failed after multiple attempts.\n");
        return 1;  // Exit with failure
    }

    attempts = 3;  // Reset attempts for more_allocation
    while (more_allocation(my_arena) != 0 && attempts-- > 0) {
        printf("Retrying more allocation...\n");
    }

    if (attempts <= 0) {
        printf("Memory allocation failed after multiple attempts.\n");
        return 1;  // Exit with failure
    }

    arena_destroy(my_arena);  // Clean up the arena
    printf("Arena reset. Memory can be reused.");
    return 0;  // Success
    pthread_mutex_destroy(&lock);
}