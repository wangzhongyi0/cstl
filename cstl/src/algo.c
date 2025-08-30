/**
 * @file algo.c
 * @brief CSTL库的算法模块实现
 * 
 * 该文件实现了CSTL库的算法模块，包括排序、查找、
 * 比较和转换等通用算法。
 */

#include "cstl/algo.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

/**
 * @brief 临时缓冲区结构体
 */
typedef struct {
    void* data;      /**< 数据指针 */
    size_t size;     /**< 缓冲区大小 */
    size_t capacity; /**< 缓冲区容量 */
} temp_buffer_t;

/**
 * @brief 创建临时缓冲区
 * 
 * @param capacity 缓冲区容量
 * @param element_size 元素大小
 * @return temp_buffer_t* 临时缓冲区指针，失败返回NULL
 */
static temp_buffer_t* temp_buffer_create(size_t capacity, size_t element_size)
{
    temp_buffer_t* buffer = (temp_buffer_t*)malloc(sizeof(temp_buffer_t));
    if (buffer == NULL) {
        return NULL;
    }
    
    buffer->data = malloc(capacity * element_size);
    if (buffer->data == NULL) {
        free(buffer);
        return NULL;
    }
    
    buffer->size = 0;
    buffer->capacity = capacity;
    
    return buffer;
}

/**
 * @brief 销毁临时缓冲区
 * 
 * @param buffer 临时缓冲区指针
 */
static void temp_buffer_destroy(temp_buffer_t* buffer)
{
    if (buffer != NULL) {
        free(buffer->data);
        free(buffer);
    }
}

/**
 * @brief 获取指定索引的元素
 * 
 * @param begin 起始迭代器
 * @param index 元素索引
 * @return void* 元素指针
 */
static void* get_element_at_index(iterator_t* begin, size_t index)
{
    iterator_t* iter = iterator_clone(begin);
    for (size_t i = 0; i < index; i++) {
        iterator_next(iter);
    }
    void* element = NULL;
    iterator_get(iter, &element);
    iterator_destroy(iter);
    return element;
}

/**
 * @brief 设置指定索引的元素
 * 
 * @param begin 起始迭代器
 * @param index 元素索引
 * @param value 要设置的值指针
 * @param element_size 元素大小
 */
static void set_element_at_index(iterator_t* begin, size_t index, const void* value, size_t element_size)
{
    iterator_t* iter = iterator_clone(begin);
    for (size_t i = 0; i < index; i++) {
        iterator_next(iter);
    }
    void* target = NULL;
    iterator_get(iter, &target);
    memcpy(target, value, element_size);
    iterator_destroy(iter);
}

/**
 * @brief 快速排序分区函数
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param element_size 元素大小
 * @param pivot 输出参数，存储分区点迭代器
 * @return error_code_t 错误码
 */
static error_code_t quick_sort_partition(iterator_t* begin, iterator_t* end,
                                        compare_fn_t compare, size_t element_size,
                                        iterator_t** pivot)
{
    if (begin == NULL || end == NULL || compare == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 检查范围是否为空或只有一个元素 */
    iterator_t* temp = iterator_clone(begin);
    iterator_next(temp);
    if (iterator_equal(temp, end) || !iterator_valid(temp)) {
        iterator_destroy(temp);
        *pivot = iterator_clone(begin);
        return CSTL_OK;
    }
    iterator_destroy(temp);
    
    /* 选择最后一个元素作为基准 */
    iterator_t* last = iterator_clone(end);
    iterator_prev(last);
    void* pivot_value_ptr = NULL;
    iterator_get(last, &pivot_value_ptr);
    
    /* 复制基准值，避免指针失效问题 */
    void* pivot_value = malloc(element_size);
    if (pivot_value == NULL) {
        iterator_destroy(last);
        return CSTL_ERROR_OUT_OF_MEMORY;
    }
    memcpy(pivot_value, pivot_value_ptr, element_size);
    
    /* 初始化i为begin-1，使用简单的指针计数而不是迭代器操作 */
    iterator_t* i = iterator_clone(begin);
    
    /* 遍历从begin到last的元素（不包括last） */
    iterator_t* j = iterator_clone(begin);
    while (iterator_valid(j) && !iterator_equal(j, last)) {
        void* current = NULL;
        iterator_get(j, &current);
        
        if (compare(current, pivot_value) <= 0) {
            /* 交换i和j位置的元素 */
            void* i_value = NULL;
            iterator_get(i, &i_value);
            algo_swap(i_value, current, element_size);
            
            /* 移动i到下一个位置 */
            iterator_next(i);
        }
        
        iterator_next(j);
    }
    
    /* 将基准值放到正确的位置（i的位置） */
    if (!iterator_valid(i)) {
        iterator_destroy(last);
        iterator_destroy(i);
        iterator_destroy(j);
        free(pivot_value);
        return CSTL_ERROR_ITERATOR_END;
    }
    
    /* 交换i位置和基准位置的元素 */
    void* i_value = NULL;
    iterator_get(i, &i_value);
    algo_swap(i_value, pivot_value_ptr, element_size);
    
    free(pivot_value);
    
    *pivot = iterator_clone(i);
    
    iterator_destroy(last);
    iterator_destroy(i);
    iterator_destroy(j);
    
    return CSTL_OK;
}

/**
 * @brief 快速排序实现
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param element_size 元素大小
 * @return error_code_t 错误码
 */
static error_code_t quick_sort_impl(iterator_t* begin, iterator_t* end,
                                   compare_fn_t compare, size_t element_size)
{
    if (begin == NULL || end == NULL || compare == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 计算范围大小 */
    size_t size = 0;
    iterator_t* temp = iterator_clone(begin);
    
    while (iterator_valid(temp) && !iterator_equal(temp, end)) {
        size++;
        iterator_next(temp);
    }
    
    iterator_destroy(temp);
    
    if (size <= 1) {
        return CSTL_OK;
    }
    
    iterator_t* pivot_iter = NULL;
    error_code_t result = quick_sort_partition(begin, end, compare, element_size, &pivot_iter);
    if (result != CSTL_OK) {
        return result;
    }
    
    if (pivot_iter == NULL) {
        return CSTL_OK;
    }
    
    /* 计算左侧部分大小 */
    size_t left_size = 0;
    temp = iterator_clone(begin);
    while (iterator_valid(temp) && !iterator_equal(temp, pivot_iter)) {
        left_size++;
        iterator_next(temp);
    }
    iterator_destroy(temp);
    
    /* 计算右侧部分大小 */
    size_t right_size = 0;
    temp = iterator_clone(pivot_iter);
    iterator_next(temp);
    while (iterator_valid(temp) && !iterator_equal(temp, end)) {
        right_size++;
        iterator_next(temp);
    }
    iterator_destroy(temp);
    
    /* 递归排序基准左侧部分 [begin, pivot) */
    if (left_size > 0) {
        iterator_t* left_begin = iterator_clone(begin);
        iterator_t* left_end = iterator_clone(pivot_iter);
        
        quick_sort_impl(left_begin, left_end, compare, element_size);
        
        iterator_destroy(left_begin);
        iterator_destroy(left_end);
    }
    
    /* 递归排序基准右侧部分 [pivot+1, end) */
    if (right_size > 0) {
        iterator_t* right_begin = iterator_clone(pivot_iter);
        iterator_next(right_begin);
        iterator_t* right_end = iterator_clone(end);
        
        quick_sort_impl(right_begin, right_end, compare, element_size);
        
        iterator_destroy(right_begin);
        iterator_destroy(right_end);
    }
    
    iterator_destroy(pivot_iter);
    
    return CSTL_OK;
}

/**
 * @brief 归并排序合并函数
 * 
 * @param begin 起始迭代器
 * @param mid 中间迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param element_size 元素大小
 * @return error_code_t 错误码
 */
static error_code_t merge_sort_merge(iterator_t* begin, iterator_t* mid, iterator_t* end, 
                                     compare_fn_t compare, size_t element_size)
{
    /* 创建临时缓冲区 */
    size_t left_size = 0;
    size_t right_size = 0;
    
    iterator_t* temp = iterator_clone(begin);
    while (iterator_valid(temp) && !iterator_equal(temp, mid)) {
        left_size++;
        iterator_next(temp);
    }
    iterator_destroy(temp);
    
    temp = iterator_clone(mid);
    while (iterator_valid(temp) && !iterator_equal(temp, end)) {
        right_size++;
        iterator_next(temp);
    }
    iterator_destroy(temp);
    
    temp_buffer_t* left_buffer = temp_buffer_create(left_size, element_size);
    if (left_buffer == NULL) {
        return CSTL_ERROR_OUT_OF_MEMORY;
    }
    
    temp_buffer_t* right_buffer = temp_buffer_create(right_size, element_size);
    if (right_buffer == NULL) {
        temp_buffer_destroy(left_buffer);
        return CSTL_ERROR_OUT_OF_MEMORY;
    }
    
    /* 复制数据到临时缓冲区 */
    size_t i = 0;
    iterator_t* iter = iterator_clone(begin);
    while (iterator_valid(iter) && !iterator_equal(iter, mid)) {
        void* element = NULL;
        iterator_get(iter, &element);
        memcpy((char*)left_buffer->data + i * element_size, element, element_size);
        i++;
        iterator_next(iter);
    }
    iterator_destroy(iter);
    
    i = 0;
    iter = iterator_clone(mid);
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* element = NULL;
        iterator_get(iter, &element);
        memcpy((char*)right_buffer->data + i * element_size, element, element_size);
        i++;
        iterator_next(iter);
    }
    iterator_destroy(iter);
    
    /* 合并两个缓冲区 */
    size_t left_index = 0;
    size_t right_index = 0;
    iter = iterator_clone(begin);
    
    while (left_index < left_size && right_index < right_size) {
        void* left_element = (char*)left_buffer->data + left_index * element_size;
        void* right_element = (char*)right_buffer->data + right_index * element_size;
        
        if (compare(left_element, right_element) <= 0) {
            void* target = NULL;
            iterator_get(iter, &target);
            memcpy(target, left_element, element_size);
            left_index++;
        } else {
            void* target = NULL;
            iterator_get(iter, &target);
            memcpy(target, right_element, element_size);
            right_index++;
        }
        
        iterator_next(iter);
    }
    
    /* 复制剩余元素 */
    while (left_index < left_size) {
        void* left_element = (char*)left_buffer->data + left_index * element_size;
        void* target = NULL;
        iterator_get(iter, &target);
        memcpy(target, left_element, element_size);
        left_index++;
        iterator_next(iter);
    }
    
    while (right_index < right_size) {
        void* right_element = (char*)right_buffer->data + right_index * element_size;
        void* target = NULL;
        iterator_get(iter, &target);
        memcpy(target, right_element, element_size);
        right_index++;
        iterator_next(iter);
    }
    
    iterator_destroy(iter);
    temp_buffer_destroy(left_buffer);
    temp_buffer_destroy(right_buffer);
    
    return CSTL_OK;
}

/**
 * @brief 归并排序实现
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param element_size 元素大小
 * @return error_code_t 错误码
 */
static error_code_t merge_sort_impl(iterator_t* begin, iterator_t* end, 
                                   compare_fn_t compare, size_t element_size)
{
    if (begin == NULL || end == NULL || compare == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 计算范围大小 */
    size_t size = 0;
    iterator_t* temp = iterator_clone(begin);
    
    while (iterator_valid(temp) && !iterator_equal(temp, end)) {
        size++;
        iterator_next(temp);
    }
    
    iterator_destroy(temp);
    
    if (size <= 1) {
        return CSTL_OK;
    }
    
    /* 找到中间位置 */
    size_t mid_size = size / 2;
    iterator_t* mid = iterator_clone(begin);
    
    for (size_t i = 0; i < mid_size; i++) {
        if (iterator_valid(mid)) {
            iterator_next(mid);
        }
    }
    
    /* 递归排序 */
    iterator_t* left_begin = iterator_clone(begin);
    iterator_t* left_end = iterator_clone(mid);
    
    iterator_t* right_begin = iterator_clone(mid);
    iterator_t* right_end = iterator_clone(end);
    
    merge_sort_impl(left_begin, left_end, compare, element_size);
    merge_sort_impl(right_begin, right_end, compare, element_size);
    
    /* 合并已排序的部分 */
    merge_sort_merge(begin, mid, end, compare, element_size);
    
    iterator_destroy(left_begin);
    iterator_destroy(left_end);
    iterator_destroy(right_begin);
    iterator_destroy(right_end);
    iterator_destroy(mid);
    
    return CSTL_OK;
}

/**
 * @brief 堆排序下沉函数
 * 
 * @param begin 起始迭代器
 * @param heap_size 堆大小
 * @param root_index 根索引
 * @param compare 比较函数指针
 * @param element_size 元素大小
 */
static void heap_sort_sift_down(iterator_t* begin, size_t heap_size, size_t root_index, 
                                compare_fn_t compare, size_t element_size)
{
    size_t largest = root_index;
    size_t left = 2 * root_index + 1;
    size_t right = 2 * root_index + 2;
    
    if (left < heap_size) {
        void* left_element = get_element_at_index(begin, left);
        void* largest_element = get_element_at_index(begin, largest);
        
        if (compare(left_element, largest_element) > 0) {
            largest = left;
        }
    }
    
    if (right < heap_size) {
        void* right_element = get_element_at_index(begin, right);
        void* largest_element = get_element_at_index(begin, largest);
        
        if (compare(right_element, largest_element) > 0) {
            largest = right;
        }
    }
    
    if (largest != root_index) {
        void* root_element = get_element_at_index(begin, root_index);
        void* largest_element = get_element_at_index(begin, largest);
        
        algo_swap(root_element, largest_element, element_size);
        
        heap_sort_sift_down(begin, heap_size, largest, compare, element_size);
    }
}

/**
 * @brief 堆排序实现
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param element_size 元素大小
 * @return error_code_t 错误码
 */
static error_code_t heap_sort_impl(iterator_t* begin, iterator_t* end, 
                                  compare_fn_t compare, size_t element_size)
{
    if (begin == NULL || end == NULL || compare == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 计算范围大小 */
    size_t size = 0;
    iterator_t* temp = iterator_clone(begin);
    
    while (iterator_valid(temp) && !iterator_equal(temp, end)) {
        size++;
        iterator_next(temp);
    }
    
    iterator_destroy(temp);
    
    if (size <= 1) {
        return CSTL_OK;
    }
    
    /* 构建最大堆 */
    for (int i = (int)size / 2 - 1; i >= 0; i--) {
        heap_sort_sift_down(begin, size, (size_t)i, compare, element_size);
    }
    
    /* 逐个提取元素 */
    for (size_t i = size - 1; i > 0; i--) {
        /* 交换根元素和最后一个元素 */
        void* root_element = get_element_at_index(begin, 0);
        void* last_element = get_element_at_index(begin, i);
        
        algo_swap(root_element, last_element, element_size);
        
        /* 重新构建堆 */
        heap_sort_sift_down(begin, i, 0, compare, element_size);
    }
    
    return CSTL_OK;
}

/**
 * @brief 插入排序实现
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param element_size 元素大小
 * @return error_code_t 错误码
 */
static error_code_t insert_sort_impl(iterator_t* begin, iterator_t* end, 
                                    compare_fn_t compare, size_t element_size)
{
    if (begin == NULL || end == NULL || compare == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 计算范围大小 */
    size_t size = 0;
    iterator_t* temp = iterator_clone(begin);
    
    while (iterator_valid(temp) && !iterator_equal(temp, end)) {
        size++;
        iterator_next(temp);
    }
    
    iterator_destroy(temp);
    
    if (size <= 1) {
        return CSTL_OK;
    }
    
    /* 插入排序 */
    iterator_t* i = iterator_clone(begin);
    iterator_next(i);
    
    while (iterator_valid(i) && !iterator_equal(i, end)) {
        /* 保存当前元素的值 */
        void* key = malloc(element_size);
        if (key == NULL) {
            iterator_destroy(i);
            return CSTL_ERROR_OUT_OF_MEMORY;
        }
        
        void* i_element = NULL;
        iterator_get(i, &i_element);
        memcpy(key, i_element, element_size);
        
        /* 找到key应该插入的位置 */
        iterator_t* j = iterator_clone(i);
        iterator_prev(j);
        
        /* 向前移动比key大的元素 */
        while (iterator_valid(j) && !iterator_equal(j, begin)) {
            void* j_element = NULL;
            iterator_get(j, &j_element);
            
            if (compare(j_element, key) > 0) {
                /* 将j元素移动到j+1位置 */
                iterator_t* j_next = iterator_clone(j);
                iterator_next(j_next);
                if (iterator_valid(j_next)) {
                    void* j_next_element = NULL;
                    iterator_get(j_next, &j_next_element);
                    memcpy(j_next_element, j_element, element_size);
                }
                iterator_destroy(j_next);
                
                iterator_prev(j);
            } else {
                break;
            }
        }
        
        /* 检查是否需要移动第一个元素 */
        if (iterator_valid(j) && !iterator_equal(j, begin)) {
            void* j_element = NULL;
            iterator_get(j, &j_element);
            
            if (compare(j_element, key) > 0) {
                /* 将j元素移动到j+1位置 */
                iterator_t* j_next = iterator_clone(j);
                iterator_next(j_next);
                if (iterator_valid(j_next)) {
                    void* j_next_element = NULL;
                    iterator_get(j_next, &j_next_element);
                    memcpy(j_next_element, j_element, element_size);
                }
                iterator_destroy(j_next);
                
                /* 将key放到第一个位置 */
                void* first_element = NULL;
                iterator_get(begin, &first_element);
                memcpy(first_element, key, element_size);
            } else {
                /* 将key放到j+1位置 */
                iterator_t* j_next = iterator_clone(j);
                iterator_next(j_next);
                if (iterator_valid(j_next)) {
                    void* j_next_element = NULL;
                    iterator_get(j_next, &j_next_element);
                    memcpy(j_next_element, key, element_size);
                }
                iterator_destroy(j_next);
            }
        } else {
            /* j无效或j等于begin，检查第一个元素 */
            if (iterator_valid(j)) {
                void* j_element = NULL;
                iterator_get(j, &j_element);
                
                if (compare(j_element, key) > 0) {
                    /* 将j元素移动到j+1位置 */
                    iterator_t* j_next = iterator_clone(j);
                    iterator_next(j_next);
                    if (iterator_valid(j_next)) {
                        void* j_next_element = NULL;
                        iterator_get(j_next, &j_next_element);
                        memcpy(j_next_element, j_element, element_size);
                    }
                    iterator_destroy(j_next);
                    
                    /* 将key放到第一个位置 */
                    void* first_element = NULL;
                    iterator_get(begin, &first_element);
                    memcpy(first_element, key, element_size);
                } else {
                    /* 将key放到j+1位置 */
                    iterator_t* j_next = iterator_clone(j);
                    iterator_next(j_next);
                    if (iterator_valid(j_next)) {
                        void* j_next_element = NULL;
                        iterator_get(j_next, &j_next_element);
                        memcpy(j_next_element, key, element_size);
                    }
                    iterator_destroy(j_next);
                }
            } else {
                /* j无效，将key放到第一个位置 */
                void* first_element = NULL;
                iterator_get(begin, &first_element);
                memcpy(first_element, key, element_size);
            }
        }
        
        iterator_destroy(j);
        free(key);
        iterator_next(i);
    }
    
    iterator_destroy(i);
    
    return CSTL_OK;
}

/**
 * @brief 对容器进行排序
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param algorithm 排序算法
 * @return error_code_t 错误码
 */
error_code_t algo_sort(iterator_t* begin, iterator_t* end, compare_fn_t compare, sort_algorithm_t algorithm)
{
    if (begin == NULL || end == NULL || compare == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    size_t element_size = begin->element_size;
    
    switch (algorithm) {
        case SORT_QUICK:
            return quick_sort_impl(begin, end, compare, element_size);
        case SORT_MERGE:
            return merge_sort_impl(begin, end, compare, element_size);
        case SORT_HEAP:
            return heap_sort_impl(begin, end, compare, element_size);
        case SORT_INSERT:
            return insert_sort_impl(begin, end, compare, element_size);
        default:
            return CSTL_ERROR_INVALID_ARGUMENT;
    }
}

/**
 * @brief 对容器进行稳定排序
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @return error_code_t 错误码
 */
error_code_t algo_stable_sort(iterator_t* begin, iterator_t* end, compare_fn_t compare)
{
    /* 使用归并排序作为稳定排序算法 */
    return algo_sort(begin, end, compare, SORT_MERGE);
}

/**
 * @brief 检查容器是否已排序
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param is_sorted 输出参数，存储是否已排序
 * @return error_code_t 错误码
 */
error_code_t algo_is_sorted(iterator_t* begin, iterator_t* end, compare_fn_t compare, int* is_sorted)
{
    if (begin == NULL || end == NULL || compare == NULL || is_sorted == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *is_sorted = 1;
    
    iterator_t* prev = iterator_clone(begin);
    iterator_t* current = iterator_clone(begin);
    iterator_next(current);
    
    while (iterator_valid(current) && !iterator_equal(current, end)) {
        void* prev_element = NULL;
        void* current_element = NULL;
        
        iterator_get(prev, &prev_element);
        iterator_get(current, &current_element);
        
        if (compare(prev_element, current_element) > 0) {
            *is_sorted = 0;
            break;
        }
        
        iterator_next(prev);
        iterator_next(current);
    }
    
    iterator_destroy(prev);
    iterator_destroy(current);
    
    return CSTL_OK;
}

/**
 * @brief 查找第一个等于给定值的元素
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param value 要查找的值指针
 * @param compare 比较函数指针
 * @param result 输出参数，存储找到的元素指针
 * @return error_code_t 错误码
 */
error_code_t algo_find(iterator_t* begin, iterator_t* end, const void* value, compare_fn_t compare, void** result)
{
    if (begin == NULL || end == NULL || value == NULL || compare == NULL || result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *result = NULL;
    
    iterator_t* iter = iterator_clone(begin);
    
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* element = NULL;
        iterator_get(iter, &element);
        
        if (compare(element, value) == 0) {
            *result = element;
            iterator_destroy(iter);
            return CSTL_OK;
        }
        
        iterator_next(iter);
    }
    
    iterator_destroy(iter);
    return CSTL_ERROR_NOT_FOUND;
}

/**
 * @brief 查找第一个满足谓词条件的元素
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param result 输出参数，存储找到的元素指针
 * @return error_code_t 错误码
 */
error_code_t algo_find_if(iterator_t* begin, iterator_t* end, predicate_fn_t predicate, void** result)
{
    if (begin == NULL || end == NULL || predicate == NULL || result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *result = NULL;
    
    iterator_t* iter = iterator_clone(begin);
    
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* element = NULL;
        iterator_get(iter, &element);
        
        if (predicate(element)) {
            *result = element;
            iterator_destroy(iter);
            return CSTL_OK;
        }
        
        iterator_next(iter);
    }
    
    iterator_destroy(iter);
    return CSTL_ERROR_NOT_FOUND;
}

/**
 * @brief 查找第一个不满足谓词条件的元素
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param result 输出参数，存储找到的元素指针
 * @return error_code_t 错误码
 */
error_code_t algo_find_if_not(iterator_t* begin, iterator_t* end, predicate_fn_t predicate, void** result)
{
    if (begin == NULL || end == NULL || predicate == NULL || result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *result = NULL;
    
    iterator_t* iter = iterator_clone(begin);
    
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* element = NULL;
        iterator_get(iter, &element);
        
        if (!predicate(element)) {
            *result = element;
            iterator_destroy(iter);
            return CSTL_OK;
        }
        
        iterator_next(iter);
    }
    
    iterator_destroy(iter);
    return CSTL_ERROR_NOT_FOUND;
}

/**
 * @brief 计算等于给定值的元素数量
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param value 要查找的值指针
 * @param compare 比较函数指针
 * @param count 输出参数，存储元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_count(iterator_t* begin, iterator_t* end, const void* value, compare_fn_t compare, size_t* count)
{
    if (begin == NULL || end == NULL || value == NULL || compare == NULL || count == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *count = 0;
    
    iterator_t* iter = iterator_clone(begin);
    
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* element = NULL;
        iterator_get(iter, &element);
        
        if (compare(element, value) == 0) {
            (*count)++;
        }
        
        iterator_next(iter);
    }
    
    iterator_destroy(iter);
    return CSTL_OK;
}

/**
 * @brief 计算满足谓词条件的元素数量
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param count 输出参数，存储元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_count_if(iterator_t* begin, iterator_t* end, predicate_fn_t predicate, size_t* count)
{
    if (begin == NULL || end == NULL || predicate == NULL || count == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *count = 0;
    
    iterator_t* iter = iterator_clone(begin);
    
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* element = NULL;
        iterator_get(iter, &element);
        
        if (predicate(element)) {
            (*count)++;
        }
        
        iterator_next(iter);
    }
    
    iterator_destroy(iter);
    return CSTL_OK;
}

/**
 * @brief 检查是否所有元素都满足谓词条件
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param result 输出参数，存储检查结果
 * @return error_code_t 错误码
 */
error_code_t algo_all_of(iterator_t* begin, iterator_t* end, predicate_fn_t predicate, int* result)
{
    if (begin == NULL || end == NULL || predicate == NULL || result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *result = 1;
    
    iterator_t* iter = iterator_clone(begin);
    
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* element = NULL;
        iterator_get(iter, &element);
        
        if (!predicate(element)) {
            *result = 0;
            iterator_destroy(iter);
            return CSTL_OK;
        }
        
        iterator_next(iter);
    }
    
    iterator_destroy(iter);
    return CSTL_OK;
}

/**
 * @brief 检查是否存在元素满足谓词条件
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param result 输出参数，存储检查结果
 * @return error_code_t 错误码
 */
error_code_t algo_any_of(iterator_t* begin, iterator_t* end, predicate_fn_t predicate, int* result)
{
    if (begin == NULL || end == NULL || predicate == NULL || result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *result = 0;
    
    iterator_t* iter = iterator_clone(begin);
    
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* element = NULL;
        iterator_get(iter, &element);
        
        if (predicate(element)) {
            *result = 1;
            iterator_destroy(iter);
            return CSTL_OK;
        }
        
        iterator_next(iter);
    }
    
    iterator_destroy(iter);
    return CSTL_OK;
}

/**
 * @brief 检查是否所有元素都不满足谓词条件
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param result 输出参数，存储检查结果
 * @return error_code_t 错误码
 */
error_code_t algo_none_of(iterator_t* begin, iterator_t* end, predicate_fn_t predicate, int* result)
{
    if (begin == NULL || end == NULL || predicate == NULL || result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *result = 1;
    
    iterator_t* iter = iterator_clone(begin);
    
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* element = NULL;
        iterator_get(iter, &element);
        
        if (predicate(element)) {
            *result = 0;
            iterator_destroy(iter);
            return CSTL_OK;
        }
        
        iterator_next(iter);
    }
    
    iterator_destroy(iter);
    return CSTL_OK;
}

/**
 * @brief 对容器中的每个元素应用一元操作
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param op 一元操作函数指针
 * @return error_code_t 错误码
 */
error_code_t algo_for_each(iterator_t* begin, iterator_t* end, unary_op_fn_t op)
{
    if (begin == NULL || end == NULL || op == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    iterator_t* iter = iterator_clone(begin);
    
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* element = NULL;
        iterator_get(iter, &element);
        
        op(element);
        
        iterator_next(iter);
    }
    
    iterator_destroy(iter);
    return CSTL_OK;
}

/**
 * @brief 查找两个相邻的相等元素
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储找到的第一个元素指针
 * @return error_code_t 错误码
 */
error_code_t algo_adjacent_find(iterator_t* begin, iterator_t* end, compare_fn_t compare, void** result)
{
    if (begin == NULL || end == NULL || compare == NULL || result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *result = NULL;
    
    iterator_t* prev = iterator_clone(begin);
    iterator_t* current = iterator_clone(begin);
    iterator_next(current);
    
    while (iterator_valid(current) && !iterator_equal(current, end)) {
        void* prev_element = NULL;
        void* current_element = NULL;
        
        iterator_get(prev, &prev_element);
        iterator_get(current, &current_element);
        
        if (compare(prev_element, current_element) == 0) {
            *result = prev_element;
            iterator_destroy(prev);
            iterator_destroy(current);
            return CSTL_OK;
        }
        
        iterator_next(prev);
        iterator_next(current);
    }
    
    iterator_destroy(prev);
    iterator_destroy(current);
    return CSTL_ERROR_NOT_FOUND;
}

/**
 * @brief 查找第一个在第二个范围中出现的元素
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param end2 第二个范围的结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储找到的元素指针
 * @return error_code_t 错误码
 */
error_code_t algo_find_first_of(iterator_t* begin1, iterator_t* end1, 
                               iterator_t* begin2, iterator_t* end2, 
                               compare_fn_t compare, void** result)
{
    if (begin1 == NULL || end1 == NULL || begin2 == NULL || end2 == NULL || 
        compare == NULL || result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *result = NULL;
    
    iterator_t* iter1 = iterator_clone(begin1);
    
    while (iterator_valid(iter1) && !iterator_equal(iter1, end1)) {
        void* element1 = NULL;
        iterator_get(iter1, &element1);
        
        iterator_t* iter2 = iterator_clone(begin2);
        
        while (iterator_valid(iter2) && !iterator_equal(iter2, end2)) {
            void* element2 = NULL;
            iterator_get(iter2, &element2);
            
            if (compare(element1, element2) == 0) {
                *result = element1;
                iterator_destroy(iter1);
                iterator_destroy(iter2);
                return CSTL_OK;
            }
            
            iterator_next(iter2);
        }
        
        iterator_destroy(iter2);
        iterator_next(iter1);
    }
    
    iterator_destroy(iter1);
    return CSTL_ERROR_NOT_FOUND;
}

/**
 * @brief 查找第一个不在第二个范围中出现的元素
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param end2 第二个范围的结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储找到的元素指针
 * @return error_code_t 错误码
 */
error_code_t algo_find_first_not_of(iterator_t* begin1, iterator_t* end1, 
                                    iterator_t* begin2, iterator_t* end2, 
                                    compare_fn_t compare, void** result)
{
    if (begin1 == NULL || end1 == NULL || begin2 == NULL || end2 == NULL || 
        compare == NULL || result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *result = NULL;
    
    iterator_t* iter1 = iterator_clone(begin1);
    
    while (iterator_valid(iter1) && !iterator_equal(iter1, end1)) {
        void* element1 = NULL;
        iterator_get(iter1, &element1);
        
        int found = 0;
        iterator_t* iter2 = iterator_clone(begin2);
        
        while (iterator_valid(iter2) && !iterator_equal(iter2, end2)) {
            void* element2 = NULL;
            iterator_get(iter2, &element2);
            
            if (compare(element1, element2) == 0) {
                found = 1;
                break;
            }
            
            iterator_next(iter2);
        }
        
        iterator_destroy(iter2);
        
        if (!found) {
            *result = element1;
            iterator_destroy(iter1);
            return CSTL_OK;
        }
        
        iterator_next(iter1);
    }
    
    iterator_destroy(iter1);
    return CSTL_ERROR_NOT_FOUND;
}

/**
 * @brief 检查两个范围是否相等
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param compare 比较函数指针
 * @param equal 输出参数，存储是否相等
 * @return error_code_t 错误码
 */
error_code_t algo_equal(iterator_t* begin1, iterator_t* end1, 
                       iterator_t* begin2, compare_fn_t compare, int* equal)
{
    if (begin1 == NULL || end1 == NULL || begin2 == NULL || compare == NULL || equal == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *equal = 1;
    
    iterator_t* iter1 = iterator_clone(begin1);
    iterator_t* iter2 = iterator_clone(begin2);
    
    while (iterator_valid(iter1) && !iterator_equal(iter1, end1)) {
        if (!iterator_valid(iter2)) {
            *equal = 0;
            break;
        }
        
        void* element1 = NULL;
        void* element2 = NULL;
        
        iterator_get(iter1, &element1);
        iterator_get(iter2, &element2);
        
        if (compare(element1, element2) != 0) {
            *equal = 0;
            break;
        }
        
        iterator_next(iter1);
        iterator_next(iter2);
    }
    
    /* 检查第二个范围是否还有剩余元素 */
    if (*equal && iterator_valid(iter2)) {
        *equal = 0;
    }
    
    iterator_destroy(iter1);
    iterator_destroy(iter2);
    
    return CSTL_OK;
}

/**
 * @brief 检查第一个范围是否是第二个范围的前缀
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param end2 第二个范围的结束迭代器
 * @param compare 比较函数指针
 * @param is_prefix 输出参数，存储是否是前缀
 * @return error_code_t 错误码
 */
error_code_t algo_starts_with(iterator_t* begin1, iterator_t* end1, 
                              iterator_t* begin2, iterator_t* end2, 
                              compare_fn_t compare, int* is_prefix)
{
    if (begin1 == NULL || end1 == NULL || begin2 == NULL || end2 == NULL || 
        compare == NULL || is_prefix == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *is_prefix = 0;
    
    /* 检查第一个范围是否比第二个范围短 */
    size_t size1 = 0;
    size_t size2 = 0;
    
    iterator_t* temp = iterator_clone(begin1);
    while (iterator_valid(temp) && !iterator_equal(temp, end1)) {
        size1++;
        iterator_next(temp);
    }
    iterator_destroy(temp);
    
    temp = iterator_clone(begin2);
    while (iterator_valid(temp) && !iterator_equal(temp, end2)) {
        size2++;
        iterator_next(temp);
    }
    iterator_destroy(temp);
    
    if (size1 < size2) {
        return CSTL_OK;
    }
    
    /* 检查前缀是否匹配 */
    iterator_t* iter1 = iterator_clone(begin1);
    iterator_t* iter2 = iterator_clone(begin2);
    
    while (iterator_valid(iter2) && !iterator_equal(iter2, end2)) {
        void* element1 = NULL;
        void* element2 = NULL;
        
        iterator_get(iter1, &element1);
        iterator_get(iter2, &element2);
        
        if (compare(element1, element2) != 0) {
            iterator_destroy(iter1);
            iterator_destroy(iter2);
            return CSTL_OK;
        }
        
        iterator_next(iter1);
        iterator_next(iter2);
    }
    
    *is_prefix = 1;
    
    iterator_destroy(iter1);
    iterator_destroy(iter2);
    
    return CSTL_OK;
}

/**
 * @brief 检查第一个范围是否是第二个范围的后缀
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param end2 第二个范围的结束迭代器
 * @param compare 比较函数指针
 * @param is_suffix 输出参数，存储是否是后缀
 * @return error_code_t 错误码
 */
error_code_t algo_ends_with(iterator_t* begin1, iterator_t* end1, 
                            iterator_t* begin2, iterator_t* end2, 
                            compare_fn_t compare, int* is_suffix)
{
    if (begin1 == NULL || end1 == NULL || begin2 == NULL || end2 == NULL || 
        compare == NULL || is_suffix == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *is_suffix = 0;
    
    /* 检查第一个范围是否比第二个范围短 */
    size_t size1 = 0;
    size_t size2 = 0;
    
    iterator_t* temp = iterator_clone(begin1);
    while (iterator_valid(temp) && !iterator_equal(temp, end1)) {
        size1++;
        iterator_next(temp);
    }
    iterator_destroy(temp);
    
    temp = iterator_clone(begin2);
    while (iterator_valid(temp) && !iterator_equal(temp, end2)) {
        size2++;
        iterator_next(temp);
    }
    iterator_destroy(temp);
    
    if (size1 < size2) {
        return CSTL_OK;
    }
    
    /* 定位到第一个范围的后缀位置 */
    iterator_t* iter1 = iterator_clone(begin1);
    for (size_t i = 0; i < size1 - size2; i++) {
        iterator_next(iter1);
    }
    
    /* 检查后缀是否匹配 */
    iterator_t* iter2 = iterator_clone(begin2);
    
    while (iterator_valid(iter2) && !iterator_equal(iter2, end2)) {
        void* element1 = NULL;
        void* element2 = NULL;
        
        iterator_get(iter1, &element1);
        iterator_get(iter2, &element2);
        
        if (compare(element1, element2) != 0) {
            iterator_destroy(iter1);
            iterator_destroy(iter2);
            return CSTL_OK;
        }
        
        iterator_next(iter1);
        iterator_next(iter2);
    }
    
    *is_suffix = 1;
    
    iterator_destroy(iter1);
    iterator_destroy(iter2);
    
    return CSTL_OK;
}

/**
 * @brief 查找第一个子范围
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 子范围的起始迭代器
 * @param end2 子范围的结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储找到的子范围起始位置
 * @return error_code_t 错误码
 */
error_code_t algo_search(iterator_t* begin1, iterator_t* end1, 
                        iterator_t* begin2, iterator_t* end2, 
                        compare_fn_t compare, void** result)
{
    if (begin1 == NULL || end1 == NULL || begin2 == NULL || end2 == NULL || 
        compare == NULL || result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *result = NULL;
    
    /* 检查子范围是否为空 */
    iterator_t* temp = iterator_clone(begin2);
    if (!iterator_valid(temp) || iterator_equal(temp, end2)) {
        iterator_destroy(temp);
        return CSTL_ERROR_INVALID_ARGUMENT;
    }
    iterator_destroy(temp);
    
    /* 计算子范围大小 */
    size_t sub_size = 0;
    temp = iterator_clone(begin2);
    while (iterator_valid(temp) && !iterator_equal(temp, end2)) {
        sub_size++;
        iterator_next(temp);
    }
    iterator_destroy(temp);
    
    /* 在第一个范围中搜索子范围 */
    iterator_t* iter1 = iterator_clone(begin1);
    
    while (iterator_valid(iter1) && !iterator_equal(iter1, end1)) {
        /* 检查是否有足够的剩余元素 */
        size_t remaining = 0;
        iterator_t* temp = iterator_clone(iter1);
        while (iterator_valid(temp) && !iterator_equal(temp, end1)) {
            remaining++;
            iterator_next(temp);
        }
        iterator_destroy(temp);
        
        if (remaining < sub_size) {
            break;
        }
        
        /* 检查是否匹配子范围 */
        iterator_t* sub_iter = iterator_clone(begin2);
        iterator_t* match_iter = iterator_clone(iter1);
        int match = 1;
        
        while (iterator_valid(sub_iter) && !iterator_equal(sub_iter, end2)) {
            void* element1 = NULL;
            void* element2 = NULL;
            
            iterator_get(match_iter, &element1);
            iterator_get(sub_iter, &element2);
            
            if (compare(element1, element2) != 0) {
                match = 0;
                break;
            }
            
            iterator_next(match_iter);
            iterator_next(sub_iter);
        }
        
        iterator_destroy(sub_iter);
        iterator_destroy(match_iter);
        
        if (match) {
            void* element = NULL;
            iterator_get(iter1, &element);
            *result = element;
            iterator_destroy(iter1);
            return CSTL_OK;
        }
        
        iterator_next(iter1);
    }
    
    iterator_destroy(iter1);
    return CSTL_ERROR_NOT_FOUND;
}

/**
 * @brief 查找最后一个子范围
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 子范围的起始迭代器
 * @param end2 子范围的结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储找到的子范围起始位置
 * @return error_code_t 错误码
 */
error_code_t algo_find_end(iterator_t* begin1, iterator_t* end1, 
                           iterator_t* begin2, iterator_t* end2, 
                           compare_fn_t compare, void** result)
{
    if (begin1 == NULL || end1 == NULL || begin2 == NULL || end2 == NULL || 
        compare == NULL || result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *result = NULL;
    
    /* 检查子范围是否为空 */
    iterator_t* temp = iterator_clone(begin2);
    if (!iterator_valid(temp) || iterator_equal(temp, end2)) {
        iterator_destroy(temp);
        return CSTL_ERROR_INVALID_ARGUMENT;
    }
    iterator_destroy(temp);
    
    /* 计算子范围大小 */
    size_t sub_size = 0;
    temp = iterator_clone(begin2);
    while (iterator_valid(temp) && !iterator_equal(temp, end2)) {
        sub_size++;
        iterator_next(temp);
    }
    iterator_destroy(temp);
    
    /* 在第一个范围中反向搜索子范围 */
    void* last_match = NULL;
    
    iterator_t* iter1 = iterator_clone(begin1);
    
    while (iterator_valid(iter1) && !iterator_equal(iter1, end1)) {
        /* 检查是否有足够的剩余元素 */
        size_t remaining = 0;
        iterator_t* temp = iterator_clone(iter1);
        while (iterator_valid(temp) && !iterator_equal(temp, end1)) {
            remaining++;
            iterator_next(temp);
        }
        iterator_destroy(temp);
        
        if (remaining < sub_size) {
            break;
        }
        
        /* 检查是否匹配子范围 */
        iterator_t* sub_iter = iterator_clone(begin2);
        iterator_t* match_iter = iterator_clone(iter1);
        int match = 1;
        
        while (iterator_valid(sub_iter) && !iterator_equal(sub_iter, end2)) {
            void* element1 = NULL;
            void* element2 = NULL;
            
            iterator_get(match_iter, &element1);
            iterator_get(sub_iter, &element2);
            
            if (compare(element1, element2) != 0) {
                match = 0;
                break;
            }
            
            iterator_next(match_iter);
            iterator_next(sub_iter);
        }
        
        iterator_destroy(sub_iter);
        iterator_destroy(match_iter);
        
        if (match) {
            void* element = NULL;
            iterator_get(iter1, &element);
            last_match = element;
        }
        
        iterator_next(iter1);
    }
    
    iterator_destroy(iter1);
    
    if (last_match != NULL) {
        *result = last_match;
        return CSTL_OK;
    }
    
    return CSTL_ERROR_NOT_FOUND;
}

/**
 * @brief 将源范围复制到目标范围
 * 
 * @param begin 源范围的起始迭代器
 * @param end 源范围的结束迭代器
 * @param dest 目标范围的起始迭代器
 * @param count 输出参数，存储复制的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_copy(iterator_t* begin, iterator_t* end, iterator_t* dest, size_t* count)
{
    if (begin == NULL || end == NULL || dest == NULL || count == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *count = 0;
    
    iterator_t* src_iter = iterator_clone(begin);
    iterator_t* dest_iter = iterator_clone(dest);
    
    while (iterator_valid(src_iter) && !iterator_equal(src_iter, end)) {
        void* src_element = NULL;
        void* dest_element = NULL;
        
        iterator_get(src_iter, &src_element);
        iterator_get(dest_iter, &dest_element);
        
        memcpy(dest_element, src_element, begin->element_size);
        
        (*count)++;
        iterator_next(src_iter);
        iterator_next(dest_iter);
    }
    
    iterator_destroy(src_iter);
    iterator_destroy(dest_iter);
    
    return CSTL_OK;
}

/**
 * @brief 将源范围反向复制到目标范围
 * 
 * @param begin 源范围的起始迭代器
 * @param end 源范围的结束迭代器
 * @param dest 目标范围的结束迭代器
 * @param count 输出参数，存储复制的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_copy_backward(iterator_t* begin, iterator_t* end, iterator_t* dest, size_t* count)
{
    if (begin == NULL || end == NULL || dest == NULL || count == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *count = 0;
    
    /* 定位到源范围的最后一个元素 */
    iterator_t* src_iter = iterator_clone(end);
    iterator_prev(src_iter);
    
    /* 定位到目标范围的最后一个元素 */
    iterator_t* dest_iter = iterator_clone(dest);
    iterator_prev(dest_iter);
    
    while (iterator_valid(src_iter) && !iterator_equal(src_iter, begin)) {
        void* src_element = NULL;
        void* dest_element = NULL;
        
        iterator_get(src_iter, &src_element);
        iterator_get(dest_iter, &dest_element);
        
        memcpy(dest_element, src_element, begin->element_size);
        
        (*count)++;
        iterator_prev(src_iter);
        iterator_prev(dest_iter);
    }
    
    /* 复制第一个元素 */
    if (iterator_valid(src_iter)) {
        void* src_element = NULL;
        void* dest_element = NULL;
        
        iterator_get(src_iter, &src_element);
        iterator_get(dest_iter, &dest_element);
        
        memcpy(dest_element, src_element, begin->element_size);
        (*count)++;
    }
    
    iterator_destroy(src_iter);
    iterator_destroy(dest_iter);
    
    return CSTL_OK;
}

/**
 * @brief 将满足谓词条件的元素复制到目标范围
 * 
 * @param begin 源范围的起始迭代器
 * @param end 源范围的结束迭代器
 * @param dest 目标范围的起始迭代器
 * @param predicate 谓词函数指针
 * @param count 输出参数，存储复制的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_copy_if(iterator_t* begin, iterator_t* end, iterator_t* dest, 
                          predicate_fn_t predicate, size_t* count)
{
    if (begin == NULL || end == NULL || dest == NULL || predicate == NULL || count == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *count = 0;
    
    iterator_t* src_iter = iterator_clone(begin);
    iterator_t* dest_iter = iterator_clone(dest);
    
    while (iterator_valid(src_iter) && !iterator_equal(src_iter, end)) {
        void* src_element = NULL;
        iterator_get(src_iter, &src_element);
        
        if (predicate(src_element)) {
            void* dest_element = NULL;
            iterator_get(dest_iter, &dest_element);
            
            memcpy(dest_element, src_element, begin->element_size);
            
            (*count)++;
            iterator_next(dest_iter);
        }
        
        iterator_next(src_iter);
    }
    
    iterator_destroy(src_iter);
    iterator_destroy(dest_iter);
    
    return CSTL_OK;
}

/**
 * @brief 交换两个元素的值
 * 
 * @param a 第一个元素指针
 * @param b 第二个元素指针
 * @param size 元素大小
 */
void algo_swap(void* a, void* b, size_t size)
{
    if (a == NULL || b == NULL || size == 0) {
        return;
    }
    
    void* temp = malloc(size);
    if (temp == NULL) {
        return;
    }
    
    memcpy(temp, a, size);
    memcpy(a, b, size);
    memcpy(b, temp, size);
    
    free(temp);
}

/**
 * @brief 交换两个范围的元素
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param count 输出参数，存储交换的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_swap_ranges(iterator_t* begin1, iterator_t* end1, 
                              iterator_t* begin2, size_t* count)
{
    if (begin1 == NULL || end1 == NULL || begin2 == NULL || count == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *count = 0;
    
    iterator_t* iter1 = iterator_clone(begin1);
    iterator_t* iter2 = iterator_clone(begin2);
    
    while (iterator_valid(iter1) && !iterator_equal(iter1, end1)) {
        void* element1 = NULL;
        void* element2 = NULL;
        
        iterator_get(iter1, &element1);
        iterator_get(iter2, &element2);
        
        algo_swap(element1, element2, begin1->element_size);
        
        (*count)++;
        iterator_next(iter1);
        iterator_next(iter2);
    }
    
    iterator_destroy(iter1);
    iterator_destroy(iter2);
    
    return CSTL_OK;
}

/**
 * @brief 将元素转换为目标范围
 * 
 * @param begin 源范围的起始迭代器
 * @param end 源范围的结束迭代器
 * @param dest 目标范围的起始迭代器
 * @param op 一元操作函数指针
 * @param count 输出参数，存储转换的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_transform(iterator_t* begin, iterator_t* end, iterator_t* dest, 
                            unary_op_fn_t op, size_t* count)
{
    if (begin == NULL || end == NULL || dest == NULL || op == NULL || count == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *count = 0;
    
    iterator_t* src_iter = iterator_clone(begin);
    iterator_t* dest_iter = iterator_clone(dest);
    
    while (iterator_valid(src_iter) && !iterator_equal(src_iter, end)) {
        void* src_element = NULL;
        void* dest_element = NULL;
        
        iterator_get(src_iter, &src_element);
        iterator_get(dest_iter, &dest_element);
        
        /* 复制源元素到目标元素 */
        memcpy(dest_element, src_element, begin->element_size);
        
        /* 应用一元操作 */
        op(dest_element);
        
        (*count)++;
        iterator_next(src_iter);
        iterator_next(dest_iter);
    }
    
    iterator_destroy(src_iter);
    iterator_destroy(dest_iter);
    
    return CSTL_OK;
}

/**
 * @brief 将两个范围的元素转换为目标范围
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param dest 目标范围的起始迭代器
 * @param op 二元操作函数指针
 * @param count 输出参数，存储转换的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_transform_binary(iterator_t* begin1, iterator_t* end1, 
                                   iterator_t* begin2, iterator_t* dest, 
                                   binary_op_fn_t op, size_t* count)
{
    if (begin1 == NULL || end1 == NULL || begin2 == NULL || dest == NULL || 
        op == NULL || count == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *count = 0;
    
    iterator_t* iter1 = iterator_clone(begin1);
    iterator_t* iter2 = iterator_clone(begin2);
    iterator_t* dest_iter = iterator_clone(dest);
    
    while (iterator_valid(iter1) && !iterator_equal(iter1, end1)) {
        void* element1 = NULL;
        void* element2 = NULL;
        void* dest_element = NULL;
        
        iterator_get(iter1, &element1);
        iterator_get(iter2, &element2);
        iterator_get(dest_iter, &dest_element);
        
        /* 复制第一个元素到目标元素 */
        memcpy(dest_element, element1, begin1->element_size);
        
        /* 应用二元操作 */
        op(dest_element, element2);
        
        (*count)++;
        iterator_next(iter1);
        iterator_next(iter2);
        iterator_next(dest_iter);
    }
    
    iterator_destroy(iter1);
    iterator_destroy(iter2);
    iterator_destroy(dest_iter);
    
    return CSTL_OK;
}

/**
 * @brief 替换等于给定值的元素
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param old_value 旧值指针
 * @param new_value 新值指针
 * @param compare 比较函数指针
 * @param count 输出参数，存储替换的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_replace(iterator_t* begin, iterator_t* end, 
                         const void* old_value, const void* new_value, 
                         compare_fn_t compare, size_t* count)
{
    if (begin == NULL || end == NULL || old_value == NULL || new_value == NULL || 
        compare == NULL || count == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *count = 0;
    
    iterator_t* iter = iterator_clone(begin);
    
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* element = NULL;
        iterator_get(iter, &element);
        
        if (compare(element, old_value) == 0) {
            memcpy(element, new_value, begin->element_size);
            (*count)++;
        }
        
        iterator_next(iter);
    }
    
    iterator_destroy(iter);
    
    return CSTL_OK;
}

/**
 * @brief 替换满足谓词条件的元素
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param new_value 新值指针
 * @param count 输出参数，存储替换的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_replace_if(iterator_t* begin, iterator_t* end, 
                             predicate_fn_t predicate, const void* new_value, 
                             size_t* count)
{
    if (begin == NULL || end == NULL || predicate == NULL || new_value == NULL || 
        count == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *count = 0;
    
    iterator_t* iter = iterator_clone(begin);
    
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* element = NULL;
        iterator_get(iter, &element);
        
        if (predicate(element)) {
            memcpy(element, new_value, begin->element_size);
            (*count)++;
        }
        
        iterator_next(iter);
    }
    
    iterator_destroy(iter);
    
    return CSTL_OK;
}

/**
 * @brief 将满足谓词条件的元素复制到目标范围，并从源范围中移除
 * 
 * @param begin 源范围的起始迭代器
 * @param end 源范围的结束迭代器
 * @param dest 目标范围的起始迭代器
 * @param predicate 谓词函数指针
 * @param count 输出参数，存储移动的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_remove_copy_if(iterator_t* begin, iterator_t* end, iterator_t* dest, 
                                 predicate_fn_t predicate, size_t* count)
{
    if (begin == NULL || end == NULL || dest == NULL || predicate == NULL || count == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *count = 0;
    
    iterator_t* src_iter = iterator_clone(begin);
    iterator_t* dest_iter = iterator_clone(dest);
    
    while (iterator_valid(src_iter) && !iterator_equal(src_iter, end)) {
        void* src_element = NULL;
        iterator_get(src_iter, &src_element);
        
        if (predicate(src_element)) {
            void* dest_element = NULL;
            iterator_get(dest_iter, &dest_element);
            
            memcpy(dest_element, src_element, begin->element_size);
            
            (*count)++;
            iterator_next(dest_iter);
        }
        
        iterator_next(src_iter);
    }
    
    iterator_destroy(src_iter);
    iterator_destroy(dest_iter);
    
    return CSTL_OK;
}

/**
 * @brief 填充范围
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param value 填充值指针
 * @param count 输出参数，存储填充的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_fill(iterator_t* begin, iterator_t* end, const void* value, size_t* count)
{
    if (begin == NULL || end == NULL || value == NULL || count == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *count = 0;
    
    iterator_t* iter = iterator_clone(begin);
    
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* element = NULL;
        iterator_get(iter, &element);
        
        memcpy(element, value, begin->element_size);
        
        (*count)++;
        iterator_next(iter);
    }
    
    iterator_destroy(iter);
    
    return CSTL_OK;
}

/**
 * @brief 填充指定数量的元素
 * 
 * @param begin 起始迭代器
 * @param count 填充数量
 * @param value 填充值指针
 * @return error_code_t 错误码
 */
error_code_t algo_fill_n(iterator_t* begin, size_t count, const void* value)
{
    if (begin == NULL || value == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    iterator_t* iter = iterator_clone(begin);
    
    for (size_t i = 0; i < count && iterator_valid(iter); i++) {
        void* element = NULL;
        iterator_get(iter, &element);
        
        memcpy(element, value, begin->element_size);
        
        iterator_next(iter);
    }
    
    iterator_destroy(iter);
    
    return CSTL_OK;
}

/**
 * @brief 生成范围
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param generator 生成函数指针
 * @param count 输出参数，存储生成的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_generate(iterator_t* begin, iterator_t* end, 
                           void (*generator)(void*), size_t* count)
{
    if (begin == NULL || end == NULL || generator == NULL || count == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *count = 0;
    
    iterator_t* iter = iterator_clone(begin);
    
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* element = NULL;
        iterator_get(iter, &element);
        
        generator(element);
        
        (*count)++;
        iterator_next(iter);
    }
    
    iterator_destroy(iter);
    
    return CSTL_OK;
}

/**
 * @brief 生成指定数量的元素
 * 
 * @param begin 起始迭代器
 * @param count 生成数量
 * @param generator 生成函数指针
 * @return error_code_t 错误码
 */
error_code_t algo_generate_n(iterator_t* begin, size_t count, 
                             void (*generator)(void*))
{
    if (begin == NULL || generator == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    iterator_t* iter = iterator_clone(begin);
    
    for (size_t i = 0; i < count && iterator_valid(iter); i++) {
        void* element = NULL;
        iterator_get(iter, &element);
        
        generator(element);
        
        iterator_next(iter);
    }
    
    iterator_destroy(iter);
    
    return CSTL_OK;
}

/**
 * @brief 移除连续的重复元素
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param count 输出参数，存储移除的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_unique(iterator_t* begin, iterator_t* end, 
                         compare_fn_t compare, size_t* count)
{
    if (begin == NULL || end == NULL || compare == NULL || count == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *count = 0;
    
    /* 检查范围是否为空 */
    iterator_t* temp = iterator_clone(begin);
    if (!iterator_valid(temp) || iterator_equal(temp, end)) {
        iterator_destroy(temp);
        return CSTL_OK;
    }
    iterator_destroy(temp);
    
    iterator_t* prev = iterator_clone(begin);
    iterator_t* current = iterator_clone(begin);
    iterator_next(current);
    
    while (iterator_valid(current) && !iterator_equal(current, end)) {
        void* prev_element = NULL;
        void* current_element = NULL;
        
        iterator_get(prev, &prev_element);
        iterator_get(current, &current_element);
        
        if (compare(prev_element, current_element) == 0) {
            /* 找到重复元素，移除当前元素 */
            iterator_t* next = iterator_clone(current);
            iterator_next(next);
            
            /* 将后续元素前移 */
            iterator_t* move_src = iterator_clone(next);
            iterator_t* move_dest = iterator_clone(current);
            
            while (iterator_valid(move_src) && !iterator_equal(move_src, end)) {
                void* src_element = NULL;
                void* dest_element = NULL;
                
                iterator_get(move_src, &src_element);
                iterator_get(move_dest, &dest_element);
                
                memcpy(dest_element, src_element, begin->element_size);
                
                iterator_next(move_src);
                iterator_next(move_dest);
            }
            
            iterator_destroy(move_src);
            iterator_destroy(move_dest);
            iterator_destroy(next);
            
            (*count)++;
            
            /* 调整结束迭代器 */
            iterator_prev(end);
        } else {
            iterator_next(prev);
            iterator_next(current);
        }
    }
    
    iterator_destroy(prev);
    iterator_destroy(current);
    
    return CSTL_OK;
}

/**
 * @brief 反转范围
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @return error_code_t 错误码
 */
error_code_t algo_reverse(iterator_t* begin, iterator_t* end)
{
    if (begin == NULL || end == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 检查范围是否为空 */
    iterator_t* temp = iterator_clone(begin);
    if (!iterator_valid(temp) || iterator_equal(temp, end)) {
        iterator_destroy(temp);
        return CSTL_OK;
    }
    iterator_destroy(temp);
    
    iterator_t* left = iterator_clone(begin);
    iterator_t* right = iterator_clone(end);
    iterator_prev(right);
    
    while (iterator_valid(left) && iterator_valid(right)) {
        void* left_element = NULL;
        void* right_element = NULL;
        
        iterator_get(left, &left_element);
        iterator_get(right, &right_element);
        
        algo_swap(left_element, right_element, begin->element_size);
        
        iterator_t* left_next = iterator_clone(left);
        iterator_next(left_next);
        
        iterator_t* right_prev = iterator_clone(right);
        iterator_prev(right_prev);
        
        iterator_destroy(left);
        iterator_destroy(right);
        
        left = left_next;
        right = right_prev;
        
        if (iterator_equal(left, right) || iterator_equal(left_next, right)) {
            iterator_destroy(left_next);
            iterator_destroy(right_prev);
            break;
        }
        
        iterator_destroy(left_next);
        iterator_destroy(right_prev);
    }
    
    iterator_destroy(left);
    iterator_destroy(right);
    
    return CSTL_OK;
}

/**
 * @brief 旋转范围
 * 
 * @param begin 起始迭代器
 * @param middle 旋转点迭代器
 * @param end 结束迭代器
 * @return error_code_t 错误码
 */
error_code_t algo_rotate(iterator_t* begin, iterator_t* middle, iterator_t* end)
{
    if (begin == NULL || middle == NULL || end == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 检查范围是否为空 */
    iterator_t* temp = iterator_clone(begin);
    if (!iterator_valid(temp) || iterator_equal(temp, end)) {
        iterator_destroy(temp);
        return CSTL_OK;
    }
    iterator_destroy(temp);
    
    /* 反转第一部分 */
    iterator_t* temp_middle = iterator_clone(middle);
    error_code_t result = algo_reverse(begin, temp_middle);
    iterator_destroy(temp_middle);
    
    if (result != CSTL_OK) {
        return result;
    }
    
    /* 反转第二部分 */
    iterator_t* temp_begin = iterator_clone(begin);
    result = algo_reverse(middle, end);
    iterator_destroy(temp_begin);
    
    if (result != CSTL_OK) {
        return result;
    }
    
    /* 反转整个范围 */
    return algo_reverse(begin, end);
}

/**
 * @brief 随机数生成器
 * 
 * @return unsigned int 随机数
 */
static unsigned int random_number(void)
{
    static unsigned int seed = 0;
    
    if (seed == 0) {
        seed = (unsigned int)time(NULL);
    }
    
    /* 线性同余生成器 */
    seed = (seed * 1103515245 + 12345) & 0x7fffffff;
    
    return seed;
}

/**
 * @brief 随机打乱范围
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @return error_code_t 错误码
 */
error_code_t algo_shuffle(iterator_t* begin, iterator_t* end)
{
    if (begin == NULL || end == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 计算范围大小 */
    size_t size = 0;
    iterator_t* temp = iterator_clone(begin);
    
    while (iterator_valid(temp) && !iterator_equal(temp, end)) {
        size++;
        iterator_next(temp);
    }
    
    iterator_destroy(temp);
    
    if (size <= 1) {
        return CSTL_OK;
    }
    
    /* Fisher-Yates 洗牌算法 */
    for (size_t i = size - 1; i > 0; i--) {
        /* 生成随机索引 */
        size_t j = random_number() % (i + 1);
        
        /* 获取第i个元素 */
        iterator_t* iter_i = iterator_clone(begin);
        for (size_t k = 0; k < i; k++) {
            iterator_next(iter_i);
        }
        
        /* 获取第j个元素 */
        iterator_t* iter_j = iterator_clone(begin);
        for (size_t k = 0; k < j; k++) {
            iterator_next(iter_j);
        }
        
        /* 交换元素 */
        void* element_i = NULL;
        void* element_j = NULL;
        
        iterator_get(iter_i, &element_i);
        iterator_get(iter_j, &element_j);
        
        algo_swap(element_i, element_j, begin->element_size);
        
        iterator_destroy(iter_i);
        iterator_destroy(iter_j);
    }
    
    return CSTL_OK;
}

/**
 * @brief 对范围进行分区
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param result 输出参数，存储分区点迭代器
 * @return error_code_t 错误码
 */
error_code_t algo_partition(iterator_t* begin, iterator_t* end, 
                           predicate_fn_t predicate, void** result)
{
    if (begin == NULL || end == NULL || predicate == NULL || result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *result = NULL;
    
    /* 检查范围是否为空 */
    iterator_t* temp = iterator_clone(begin);
    if (!iterator_valid(temp) || iterator_equal(temp, end)) {
        iterator_destroy(temp);
        return CSTL_OK;
    }
    iterator_destroy(temp);
    
    iterator_t* first = iterator_clone(begin);
    iterator_t* last = iterator_clone(end);
    iterator_prev(last);
    
    while (iterator_valid(first)) {
        while (iterator_valid(first) && !iterator_equal(first, end)) {
            void* element = NULL;
            iterator_get(first, &element);
            
            if (predicate(element)) {
                break;
            }
            
            iterator_next(first);
        }
        
        while (iterator_valid(last) && !iterator_equal(last, begin)) {
            void* element = NULL;
            iterator_get(last, &element);
            
            if (!predicate(element)) {
                break;
            }
            
            iterator_prev(last);
        }
        
        if (!iterator_valid(first) || !iterator_valid(last) || 
            iterator_equal(first, last) || iterator_equal(first, end)) {
            break;
        }
        
        void* first_element = NULL;
        void* last_element = NULL;
        
        iterator_get(first, &first_element);
        iterator_get(last, &last_element);
        
        algo_swap(first_element, last_element, begin->element_size);
        
        iterator_next(first);
        iterator_prev(last);
    }
    
    if (iterator_valid(first)) {
        void* element = NULL;
        iterator_get(first, &element);
        *result = element;
    }
    
    iterator_destroy(first);
    iterator_destroy(last);
    
    return CSTL_OK;
}

/**
 * @brief 检查范围是否已分区
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param is_partitioned 输出参数，存储是否已分区
 * @return error_code_t 错误码
 */
error_code_t algo_is_partitioned(iterator_t* begin, iterator_t* end, 
                               predicate_fn_t predicate, int* is_partitioned)
{
    if (begin == NULL || end == NULL || predicate == NULL || is_partitioned == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *is_partitioned = 1;
    
    /* 检查范围是否为空 */
    iterator_t* temp = iterator_clone(begin);
    if (!iterator_valid(temp) || iterator_equal(temp, end)) {
        iterator_destroy(temp);
        return CSTL_OK;
    }
    iterator_destroy(temp);
    
    /* 查找第一个不满足谓词条件的元素 */
    iterator_t* iter = iterator_clone(begin);
    int found_false = 0;
    
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* element = NULL;
        iterator_get(iter, &element);
        
        if (!predicate(element)) {
            found_false = 1;
            break;
        }
        
        iterator_next(iter);
    }
    
    /* 如果所有元素都满足谓词条件，则已分区 */
    if (!found_false) {
        iterator_destroy(iter);
        return CSTL_OK;
    }
    
    /* 检查剩余元素是否都不满足谓词条件 */
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* element = NULL;
        iterator_get(iter, &element);
        
        if (predicate(element)) {
            *is_partitioned = 0;
            iterator_destroy(iter);
            return CSTL_OK;
        }
        
        iterator_next(iter);
    }
    
    iterator_destroy(iter);
    return CSTL_OK;
}

/**
 * @brief 计算范围的最小值
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储最小值指针
 * @return error_code_t 错误码
 */
error_code_t algo_min_element(iterator_t* begin, iterator_t* end, 
                             compare_fn_t compare, void** result)
{
    if (begin == NULL || end == NULL || compare == NULL || result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 检查范围是否为空 */
    iterator_t* temp = iterator_clone(begin);
    if (!iterator_valid(temp) || iterator_equal(temp, end)) {
        iterator_destroy(temp);
        return CSTL_ERROR_CONTAINER_EMPTY;
    }
    iterator_destroy(temp);
    
    iterator_t* iter = iterator_clone(begin);
    void* min_element = NULL;
    iterator_get(iter, &min_element);
    iterator_next(iter);
    
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* current_element = NULL;
        iterator_get(iter, &current_element);
        
        if (compare(current_element, min_element) < 0) {
            min_element = current_element;
        }
        
        iterator_next(iter);
    }
    
    *result = min_element;
    iterator_destroy(iter);
    
    return CSTL_OK;
}

/**
 * @brief 计算范围的最大值
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储最大值指针
 * @return error_code_t 错误码
 */
error_code_t algo_max_element(iterator_t* begin, iterator_t* end, 
                             compare_fn_t compare, void** result)
{
    if (begin == NULL || end == NULL || compare == NULL || result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 检查范围是否为空 */
    iterator_t* temp = iterator_clone(begin);
    if (!iterator_valid(temp) || iterator_equal(temp, end)) {
        iterator_destroy(temp);
        return CSTL_ERROR_CONTAINER_EMPTY;
    }
    iterator_destroy(temp);
    
    iterator_t* iter = iterator_clone(begin);
    void* max_element = NULL;
    iterator_get(iter, &max_element);
    iterator_next(iter);
    
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* current_element = NULL;
        iterator_get(iter, &current_element);
        
        if (compare(current_element, max_element) > 0) {
            max_element = current_element;
        }
        
        iterator_next(iter);
    }
    
    *result = max_element;
    iterator_destroy(iter);
    
    return CSTL_OK;
}

/**
 * @brief 计算范围的最小值和最大值
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param min_result 输出参数，存储最小值指针
 * @param max_result 输出参数，存储最大值指针
 * @return error_code_t 错误码
 */
error_code_t algo_minmax_element(iterator_t* begin, iterator_t* end, 
                                compare_fn_t compare, void** min_result, void** max_result)
{
    if (begin == NULL || end == NULL || compare == NULL || 
        min_result == NULL || max_result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 检查范围是否为空 */
    iterator_t* temp = iterator_clone(begin);
    if (!iterator_valid(temp) || iterator_equal(temp, end)) {
        iterator_destroy(temp);
        return CSTL_ERROR_CONTAINER_EMPTY;
    }
    iterator_destroy(temp);
    
    iterator_t* iter = iterator_clone(begin);
    void* min_element = NULL;
    void* max_element = NULL;
    iterator_get(iter, &min_element);
    max_element = min_element;
    iterator_next(iter);
    
    while (iterator_valid(iter) && !iterator_equal(iter, end)) {
        void* current_element = NULL;
        iterator_get(iter, &current_element);
        
        if (compare(current_element, min_element) < 0) {
            min_element = current_element;
        }
        
        if (compare(current_element, max_element) > 0) {
            max_element = current_element;
        }
        
        iterator_next(iter);
    }
    
    *min_result = min_element;
    *max_result = max_element;
    iterator_destroy(iter);
    
    return CSTL_OK;
}

/**
 * @brief 计算两个范围的字典序最小值
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param end2 第二个范围的结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储最小值范围（1或2）
 * @return error_code_t 错误码
 */
error_code_t algo_lexicographical_compare(iterator_t* begin1, iterator_t* end1, 
                                         iterator_t* begin2, iterator_t* end2, 
                                         compare_fn_t compare, int* result)
{
    if (begin1 == NULL || end1 == NULL || begin2 == NULL || end2 == NULL || 
        compare == NULL || result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    iterator_t* iter1 = iterator_clone(begin1);
    iterator_t* iter2 = iterator_clone(begin2);
    
    while (iterator_valid(iter1) && !iterator_equal(iter1, end1) && 
           iterator_valid(iter2) && !iterator_equal(iter2, end2)) {
        void* element1 = NULL;
        void* element2 = NULL;
        
        iterator_get(iter1, &element1);
        iterator_get(iter2, &element2);
        
        int cmp = compare(element1, element2);
        if (cmp != 0) {
            *result = cmp < 0 ? 1 : 2;
            iterator_destroy(iter1);
            iterator_destroy(iter2);
            return CSTL_OK;
        }
        
        iterator_next(iter1);
        iterator_next(iter2);
    }
    
    /* 检查哪个范围更短 */
    if (iterator_valid(iter1) && !iterator_equal(iter1, end1)) {
        /* 第二个范围更短 */
        *result = 2;
    } else if (iterator_valid(iter2) && !iterator_equal(iter2, end2)) {
        /* 第一个范围更短 */
        *result = 1;
    } else {
        /* 两个范围相等 */
        *result = 1;
    }
    
    iterator_destroy(iter1);
    iterator_destroy(iter2);
    
    return CSTL_OK;
}

/**
 * @brief 检查是否一个范围是另一个范围的排列
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param end2 第二个范围的结束迭代器
 * @param compare 比较函数指针
 * @param is_permutation 输出参数，存储是否是排列
 * @return error_code_t 错误码
 */
error_code_t algo_is_permutation(iterator_t* begin1, iterator_t* end1, 
                                iterator_t* begin2, iterator_t* end2, 
                                compare_fn_t compare, int* is_permutation)
{
    if (begin1 == NULL || end1 == NULL || begin2 == NULL || end2 == NULL || 
        compare == NULL || is_permutation == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *is_permutation = 0;
    
    /* 检查两个范围的大小是否相同 */
    size_t size1 = 0;
    size_t size2 = 0;
    
    iterator_t* temp = iterator_clone(begin1);
    while (iterator_valid(temp) && !iterator_equal(temp, end1)) {
        size1++;
        iterator_next(temp);
    }
    iterator_destroy(temp);
    
    temp = iterator_clone(begin2);
    while (iterator_valid(temp) && !iterator_equal(temp, end2)) {
        size2++;
        iterator_next(temp);
    }
    iterator_destroy(temp);
    
    if (size1 != size2) {
        return CSTL_OK;
    }
    
    /* 检查每个元素在第二个范围中出现的次数是否相同 */
    iterator_t* iter1 = iterator_clone(begin1);
    
    while (iterator_valid(iter1) && !iterator_equal(iter1, end1)) {
        void* element1 = NULL;
        iterator_get(iter1, &element1);
        
        /* 计算元素在第一个范围中出现的次数 */
        size_t count1 = 0;
        iterator_t* temp_iter = iterator_clone(begin1);
        
        while (iterator_valid(temp_iter) && !iterator_equal(temp_iter, end1)) {
            void* temp_element = NULL;
            iterator_get(temp_iter, &temp_element);
            
            if (compare(temp_element, element1) == 0) {
                count1++;
            }
            
            iterator_next(temp_iter);
        }
        
        iterator_destroy(temp_iter);
        
        /* 计算元素在第二个范围中出现的次数 */
        size_t count2 = 0;
        temp_iter = iterator_clone(begin2);
        
        while (iterator_valid(temp_iter) && !iterator_equal(temp_iter, end2)) {
            void* temp_element = NULL;
            iterator_get(temp_iter, &temp_element);
            
            if (compare(temp_element, element1) == 0) {
                count2++;
            }
            
            iterator_next(temp_iter);
        }
        
        iterator_destroy(temp_iter);
        
        if (count1 != count2) {
            iterator_destroy(iter1);
            return CSTL_OK;
        }
        
        iterator_next(iter1);
    }
    
    iterator_destroy(iter1);
    *is_permutation = 1;
    
    return CSTL_OK;
}

/**
 * @brief 生成下一个排列
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储是否生成了下一个排列
 * @return error_code_t 错误码
 */
error_code_t algo_next_permutation(iterator_t* begin, iterator_t* end, 
                                  compare_fn_t compare, int* result)
{
    if (begin == NULL || end == NULL || compare == NULL || result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *result = 0;
    
    /* 检查范围是否为空 */
    iterator_t* temp = iterator_clone(begin);
    if (!iterator_valid(temp) || iterator_equal(temp, end)) {
        iterator_destroy(temp);
        return CSTL_OK;
    }
    iterator_destroy(temp);
    
    /* 计算范围大小 */
    size_t size = 0;
    temp = iterator_clone(begin);
    
    while (iterator_valid(temp) && !iterator_equal(temp, end)) {
        size++;
        iterator_next(temp);
    }
    
    iterator_destroy(temp);
    
    if (size <= 1) {
        return CSTL_OK;
    }
    
    /* 查找最长的递减后缀 */
    size_t i = size - 1;
    while (i > 0) {
        iterator_t* iter_i = iterator_clone(begin);
        for (size_t j = 0; j < i; j++) {
            iterator_next(iter_i);
        }
        
        iterator_t* iter_i_minus_1 = iterator_clone(begin);
        for (size_t j = 0; j < i - 1; j++) {
            iterator_next(iter_i_minus_1);
        }
        
        void* element_i = NULL;
        void* element_i_minus_1 = NULL;
        
        iterator_get(iter_i, &element_i);
        iterator_get(iter_i_minus_1, &element_i_minus_1);
        
        if (compare(element_i_minus_1, element_i) < 0) {
            /* 找到第一个大于element_i_minus_1的元素 */
            size_t j = size - 1;
            
            while (j > i) {
                iterator_t* iter_j = iterator_clone(begin);
                for (size_t k = 0; k < j; k++) {
                    iterator_next(iter_j);
                }
                
                void* element_j = NULL;
                iterator_get(iter_j, &element_j);
                
                if (compare(element_j, element_i_minus_1) > 0) {
                    /* 交换元素 */
                    algo_swap(element_i_minus_1, element_j, begin->element_size);
                    
                    /* 反转后缀 */
                    iterator_t* suffix_begin = iterator_clone(begin);
                    for (size_t k = 0; k < i; k++) {
                        iterator_next(suffix_begin);
                    }
                    
                    iterator_t* suffix_end = iterator_clone(end);
                    
                    algo_reverse(suffix_begin, suffix_end);
                    
                    iterator_destroy(suffix_begin);
                    iterator_destroy(suffix_end);
                    iterator_destroy(iter_j);
                    iterator_destroy(iter_i);
                    iterator_destroy(iter_i_minus_1);
                    
                    *result = 1;
                    return CSTL_OK;
                }
                
                iterator_destroy(iter_j);
                j--;
            }
        }
        
        iterator_destroy(iter_i);
        iterator_destroy(iter_i_minus_1);
        i--;
    }
    
    /* 这是最后一个排列，反转整个范围以获得第一个排列 */
    algo_reverse(begin, end);
    
    return CSTL_OK;
}

/**
 * @brief 生成上一个排列
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储是否生成了上一个排列
 * @return error_code_t 错误码
 */
error_code_t algo_prev_permutation(iterator_t* begin, iterator_t* end, 
                                  compare_fn_t compare, int* result)
{
    if (begin == NULL || end == NULL || compare == NULL || result == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *result = 0;
    
    /* 检查范围是否为空 */
    iterator_t* temp = iterator_clone(begin);
    if (!iterator_valid(temp) || iterator_equal(temp, end)) {
        iterator_destroy(temp);
        return CSTL_OK;
    }
    iterator_destroy(temp);
    
    /* 计算范围大小 */
    size_t size = 0;
    temp = iterator_clone(begin);
    
    while (iterator_valid(temp) && !iterator_equal(temp, end)) {
        size++;
        iterator_next(temp);
    }
    
    iterator_destroy(temp);
    
    if (size <= 1) {
        return CSTL_OK;
    }
    
    /* 查找最长的递增后缀 */
    size_t i = size - 1;
    while (i > 0) {
        iterator_t* iter_i = iterator_clone(begin);
        for (size_t j = 0; j < i; j++) {
            iterator_next(iter_i);
        }
        
        iterator_t* iter_i_minus_1 = iterator_clone(begin);
        for (size_t j = 0; j < i - 1; j++) {
            iterator_next(iter_i_minus_1);
        }
        
        void* element_i = NULL;
        void* element_i_minus_1 = NULL;
        
        iterator_get(iter_i, &element_i);
        iterator_get(iter_i_minus_1, &element_i_minus_1);
        
        if (compare(element_i_minus_1, element_i) > 0) {
            /* 找到第一个小于element_i_minus_1的元素 */
            size_t j = size - 1;
            
            while (j > i) {
                iterator_t* iter_j = iterator_clone(begin);
                for (size_t k = 0; k < j; k++) {
                    iterator_next(iter_j);
                }
                
                void* element_j = NULL;
                iterator_get(iter_j, &element_j);
                
                if (compare(element_j, element_i_minus_1) < 0) {
                    /* 交换元素 */
                    algo_swap(element_i_minus_1, element_j, begin->element_size);
                    
                    /* 反转后缀 */
                    iterator_t* suffix_begin = iterator_clone(begin);
                    for (size_t k = 0; k < i; k++) {
                        iterator_next(suffix_begin);
                    }
                    
                    iterator_t* suffix_end = iterator_clone(end);
                    
                    algo_reverse(suffix_begin, suffix_end);
                    
                    iterator_destroy(suffix_begin);
                    iterator_destroy(suffix_end);
                    iterator_destroy(iter_j);
                    iterator_destroy(iter_i);
                    iterator_destroy(iter_i_minus_1);
                    
                    *result = 1;
                    return CSTL_OK;
                }
                
                iterator_destroy(iter_j);
                j--;
            }
        }
        
        iterator_destroy(iter_i);
        iterator_destroy(iter_i_minus_1);
        i--;
    }
    
    /* 这是第一个排列，反转整个范围以获得最后一个排列 */
    algo_reverse(begin, end);
    
    return CSTL_OK;
}
