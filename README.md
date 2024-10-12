# **Memory Allocation : Heap Arena Memory Allocation**

## **Resources**

To complete this project, I utilized the following resources:

1. **Geeks for Geeks**: [Dynamic Memory Allocation in C](https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/)
   
2. **YouTube Videos**:
   - [Video 1](https://youtu.be/Tc763vPm4Ws?si=bnGgVbA5KPKlRNms)
   - [Video 2](https://www.youtube.com/watch?v=ZisNZcQn6fo)

3. **Aligned Memory Allocator**: [Memory Management: Aligned Malloc and Free](https://medium.com/howsofcoding/memory-management-aligned-malloc-and-free-9273336bd4c6)

## **Explanation** :
### 1. Firstly I have included all the header files required for the task <br>
      #include <pthread.h>    // For thread safety
      #include <stdatomic.h>  // For atomic operations
      #include <stdlib.h>   // For malloc, free
      #include <stddef.h>   // For size_t
      #include <stdio.h>    // For debugging

### 2. Structure Definition
      typedef struct {
          void *memory;    // Pointer to the beginning of the memory pool
          size_t size;     // Total size of the arena
          size_t offset;   // Offset from the beginning (tracks where free memory starts)
      } arena_t;

- This defines a structure with three structure members.
- arena_t is a pointer pointing to the beginning of the memory pool, 'size' which defines the fixed size, and offset which gives the nearest free memory in the total allocated memory.

### 3. The function declarations.

### 4. Thread Safety
      pthread_mutex_t lock; // Mutex for thread safety
- A mutex is a lock that allows only one thread to access a critical section of code at any given time.

### 5. Memory Allocation using malloc
      arena_t *arena = (arena_t*)malloc(sizeof(arena_t));
- The above line allocates memory to store an object of type arena_t and returns a pointer to this allocated memory.
- (arena_t*) casts the void* pointer returned by malloc() function to a type of arena_t*.
- If the memory allocation fails (not enough memory available), it returns NULL.

        arena->memory = malloc(size);
- In this line the code tries to allocate memory to a structure member 'memory'.
- If memory allocation fails then it return NULL and frees the previously allocated memory.

### 6. Memory Allocation function
      arena_alloc_thread_safe(arena_t *arena, size_t size, size_t alignment) 
- This function allocates memory in a thread safe manner.
- It does so by using a mutex (mutual exclusion) to ensure that only one thread can perform the memory allocation operation at a time.

### 7. Arena Reset function
      arena_reset(arena_t *arena) 
- This function resets the memory to be reused. This does not free the memory.

### 8. Arena Destroy function
      arena_destroy(arena_t *arena) 
- This function destroys the mutex and frees the allocated memory.

### 9. Aligned arena allocation
      arena_alloc_aligned(arena_t *arena, size_t size, size_t alignment) 
- This function allocates the memory in an aligned manner.

### 10. Additional functions
      allocation(arena_t *my_arena) 
- This is a function which simply gets the user input from the console and sends it to the respective functions.

      more_allocation(arena_t *my_arena) 
- This is a function which takes in the amount of extra memory that needs to be allocated in addition to the memory already allocated.

        prompt_for_reset(arena_t *my_arena) 
- This function asks the user whether to reset the arena for new allocation or to free the memory and exit the program
