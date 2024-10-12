# **Memory Allocation : Heap Arena Memory Allocation**

## **Resources**

To complete this project, I utilized the following resources:

1. **Geeks for Geeks**: [Dynamic Memory Allocation in C](https://www.geeksforgeeks.org/dynamic-memory-allocation-in-c-using-malloc-calloc-free-and-realloc/)
   
2. **YouTube Videos**:
   - [Video 1](https://youtu.be/Tc763vPm4Ws?si=bnGgVbA5KPKlRNms)
   - [Video 2](https://www.youtube.com/watch?v=ZisNZcQn6fo)

3. **Aligned Memory Allocator**: [Memory Management: Aligned Malloc and Free](https://medium.com/howsofcoding/memory-management-aligned-malloc-and-free-9273336bd4c6)

Firstly I have initialised all the header files required for the task
#include <pthread.h>    // For thread safety
#include <stdatomic.h>  // For atomic operations
#include <stdlib.h>   // For malloc, free
#include <stddef.h>   // For size_t
#include <stdio.h>    // For optional debugging
