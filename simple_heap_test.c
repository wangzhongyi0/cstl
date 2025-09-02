#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cstl/include/cstl.h"

int compare_int(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

int main() {
    const int SIZE = 20000;
    int* array = malloc(SIZE * sizeof(int));
    if (!array) return 1;
    
    // Fill with random numbers
    srand(42);
    for (int i = 0; i < SIZE; i++) {
        array[i] = rand() % 100000;
    }
    
    // Create vector
    vector_t* vec = vector_create(sizeof(int), 0, NULL, NULL);
    if (!vec) {
        free(array);
        return 1;
    }
    
    // Push all elements
    for (int i = 0; i < SIZE; i++) {
        vector_push_back(vec, &array[i]);
    }
    
    printf("Testing heap sort with %d elements...\n", SIZE);
    
    // Get iterators
    iterator_t* begin = vector_begin(vec);
    iterator_t* end = vector_end(vec);
    
    // Time the heap sort
    clock_t start = clock();
    error_code_t result = algo_sort(begin, end, compare_int, SORT_HEAP);
    clock_t end_time = clock();
    
    if (result == CSTL_OK) {
        double elapsed = (double)(end_time - start) * 1000 / CLOCKS_PER_SEC;
        printf("Heap sort completed in %.2f ms\n", elapsed);
        
        // Verify sorted
        int sorted = 1;
        for (int i = 0; i < SIZE - 1; i++) {
            int a, b;
            vector_at(vec, i, (void**)&a);
            vector_at(vec, i + 1, (void**)&b);
            if (a > b) {
                sorted = 0;
                break;
            }
        }
        printf("Sort verification: %s\n", sorted ? "PASSED" : "FAILED");
    } else {
        printf("Heap sort failed: %d\n", result);
    }
    
    // Cleanup
    iterator_destroy(begin);
    iterator_destroy(end);
    vector_destroy(vec);
    free(array);
    
    return 0;
}