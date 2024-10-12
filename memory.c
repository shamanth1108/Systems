#include <pthread.h>    // For thread safety
#include <stdatomic.h>  // For atomic operations
#include <stdlib.h>   // For malloc, free
#include <stddef.h>   // For size_t
#include <stdio.h>    // For debugging

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
void prompt_for_reset(arena_t *my_arena);


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
    pthread_mutex_unlock(&lock);  // Unlock before destruction
    pthread_mutex_destroy(&lock);  // Destroy the mutex
}

void* arena_alloc_aligned(arena_t *arena, size_t size, size_t alignment) {
    pthread_mutex_lock(&lock);  // Lock the mutex

    size_t current_offset = arena->offset;
    size_t aligned_offset = (current_offset + alignment - 1) & ~(alignment - 1); // Align to the nearest multiple
    size_t remaining_space = arena->size - aligned_offset;
    
    // printf("Current Offset: %zu, Aligned Offset: %zu, Remaining Space: %zu\n", current_offset, aligned_offset, remaining_space);

     // Check if the aligned offset + requested size exceeds the total arena size
    if (remaining_space < size) {
        pthread_mutex_unlock(&lock);  // Unlock before returning
        return NULL;  // Out of memory
    }

    void *ptr = (void*)((char*)arena->memory + aligned_offset);
    arena->offset = aligned_offset + size;

    pthread_mutex_unlock(&lock);  // Unlock the mutex
    return ptr;
}

int allocation(arena_t *my_arena) {
    size_t alloc_size = 0;  // Size of the allocation in bytes
    size_t alignment = 8;   // Define the required alignment
    int result;
    // Keep asking for input until a valid size is provided
    while (1) {
        printf("Enter size of memory to allocate: ");
        result = scanf("%zu", &alloc_size);

        // Check if the input is a valid integer
        if (result != 1) {
            printf("Invalid input. Please enter an integer value.\n");
            // Clear the input buffer
            while (getchar() != '\n');
            continue;
        }

        // Check if the requested size is larger than the total arena size
        if (alloc_size > my_arena->size) {
            printf("Allocated memory more than the available size (1024 bytes). Please try again.\n");
        } else {
            break;  // Exit the loop when a valid size is entered
        }
    }
    void *ptr1 = arena_alloc_aligned(my_arena, alloc_size, alignment);
    if (ptr1) {
        printf("Allocated %zu bytes from the arena.\n", alloc_size);
        return 0; // Success
    } else {
        printf("Failed to allocate memory from the arena. \n");
        return -1; // Failure
    }
}

int more_allocation(arena_t *my_arena) {
    size_t alloc_size = 0;
    size_t alignment = 8;
    int result;
    // Keep asking for input until a valid size is provided
    while (1) {
        printf("Enter size of new memory to allocate: ");
        result = scanf("%zu", &alloc_size);
        // Check if the input is a valid integer
        if (result != 1) {
            printf("Invalid input. Please enter an integer value.\n");
            // Clear the input buffer
            while (getchar() != '\n');
            continue;
        }
        // Check if the user input is 0
        if (alloc_size == 0) {
            printf("Allocation size cannot be 0. Please enter a valid size.\n");
            continue;  // Continue to prompt for input
        }

        // Calculate the remaining free space
        size_t remaining_space = my_arena->size - my_arena->offset;

        // Check if the requested size exceeds the remaining free space
        if (alloc_size > remaining_space) {
            printf("Requested size (%zu bytes) exceeds available free space (%zu bytes). Please try again.\n", alloc_size, remaining_space);
        }
        else if(remaining_space == 0)
            break; 
        
        else {
            break;  // Exit the loop when a valid size is entered
        }
    }
    void *ptr2 = arena_alloc_aligned(my_arena, alloc_size, alignment);
    if (ptr2) {
        printf("Allocated another %zu bytes from the arena.\n", alloc_size);
        return 0; // Success
    } 
    return -1;
}
void prompt_for_reset(arena_t *my_arena) {
    char choice;
    printf("Arena is full. Do you want to reset the arena to allocate more memory? (y/n): ");
    scanf(" %c", &choice);  // Read a single character with space before %c to handle newline

    if (choice == 'y' || choice == 'Y') {
        arena_reset(my_arena);  // Reset the arena for future allocations
        printf("Arena reset. You can now allocate more memory.\n");
        if (allocation(my_arena) != 0) {
            prompt_for_reset(my_arena);
        }
    } else {
        printf("Exiting the program.\n");
        arena_destroy(my_arena);  // Clean up the arena
        pthread_mutex_destroy(&lock);
        exit(0);  // Exit the program
    }
}

int main() {
    pthread_mutex_t lock; // Mutex for thread safety

    pthread_mutex_init(&lock, NULL);
    
    arena_t *my_arena = arena_create(1024);  // Create a 1024-byte arena
    if (!my_arena) {
        printf("Failed to create memory arena.\n");
        return 1;  // Exit with failure
    }
    // Calling Allocation
    if (allocation(my_arena) != 0) {
        prompt_for_reset(my_arena);
    }
    // Continuously call more_allocation() until the arena is full
    while (1) {
        // Calculate the remaining space
        size_t remaining_space = my_arena->size - my_arena->offset;

        if (remaining_space == 0) {
            prompt_for_reset(my_arena);  // Prompt to reset the arena once it's full
        }
        
        if (more_allocation(my_arena) != 0) {
            prompt_for_reset(my_arena);  // Prompt to reset the arena once it's full
        }
    }

    arena_destroy(my_arena);  // Clean up the arena
    pthread_mutex_destroy(&lock);
    return 0;
}
