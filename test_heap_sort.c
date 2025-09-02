#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "cstl.h"

int compare_int(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

void print_array(vector_t* vec) {
    size_t size = vector_size(vec);
    printf("[");
    for (size_t i = 0; i < size; i++) {
        int val;
        vector_at(vec, i, (void**)&val);
        printf("%d", val);
        if (i < size - 1) printf(", ");
    }
    printf("]\n");
}

int main() {
    vector_t* vec = vector_create(sizeof(int), 10, NULL, NULL);
    
    // 测试小数组
    int test_data[] = {64, 34, 25, 12, 22, 11, 90};
    for (size_t i = 0; i < sizeof(test_data)/sizeof(test_data[0]); i++) {
        vector_push_back(vec, &test_data[i]);
    }
    
    printf("原始数组: ");
    print_array(vec);
    
    // 测试堆排序
    iterator_t* begin = vector_begin(vec);
    iterator_t* end = vector_end(vec);
    
    clock_t start = clock();
    error_code_t result = algo_sort(begin, end, compare_int, SORT_HEAP);
    clock_t end_time = clock();
    
    if (result == CSTL_OK) {
        printf("堆排序后: ");
        print_array(vec);
        printf("堆排序耗时: %f ms\n", (double)(end_time - start) * 1000 / CLOCKS_PER_SEC);
    } else {
        printf("堆排序失败: %d\n", result);
    }
    
    iterator_destroy(begin);
    iterator_destroy(end);
    vector_destroy(vec);
    
    return 0;
}