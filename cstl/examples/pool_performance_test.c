/**
 * @file pool_performance_test.c
 * @brief CSTL库的内存池和对象池性能测试示例
 * 
 * 该文件展示了CSTL库的内存池和对象池性能测试，包括：
 * - 内存池与标准分配的性能对比
 * - 对象池与标准分配的性能对比
 * - 不同大小对象的内存池性能测试
 * - 不同大小对象的对象池性能测试
 * - 内存池和对象池在容器中的应用性能测试
 */

#include "cstl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

/**
 * @brief 测试元素数量
 */
#define TEST_ELEMENT_COUNT 100000

/**
 * @brief 测试迭代次数
 */
#define TEST_ITERATIONS 10

/**
 * @brief 小对象大小
 */
#define SMALL_OBJECT_SIZE 16

/**
 * @brief 中等对象大小
 */
#define MEDIUM_OBJECT_SIZE 256

/**
 * @brief 大对象大小
 */
#define LARGE_OBJECT_SIZE 4096

/**
 * @brief 计时函数
 * 
 * @return clock_t 当前时钟
 */
clock_t start_timer(void)
{
    return clock();
}

/**
 * @brief 停止计时并计算耗时（毫秒）
 * 
 * @param start 开始时钟
 * @return double 耗时（毫秒）
 */
double stop_timer(clock_t start)
{
    clock_t end = clock();
    return ((double)(end - start) / CLOCKS_PER_SEC) * 1000;
}

/**
 * @brief 测试标准分配性能
 * 
 * @param object_size 对象大小
 * @param count 对象数量
 * @return double 耗时（毫秒）
 */
double test_standard_allocation(size_t object_size, size_t count)
{
    clock_t start = start_timer();
    
    void** objects = (void**)malloc(count * sizeof(void*));
    if (objects == NULL) {
        perror("Failed to allocate memory for objects array");
        exit(EXIT_FAILURE);
    }
    
    /* 分配对象 */
    for (size_t i = 0; i < count; i++) {
        objects[i] = malloc(object_size);
        if (objects[i] == NULL) {
            perror("Failed to allocate object");
            exit(EXIT_FAILURE);
        }
    }
    
    /* 释放对象 */
    for (size_t i = 0; i < count; i++) {
        free(objects[i]);
    }
    
    free(objects);
    
    return stop_timer(start);
}

/**
 * @brief 测试内存池分配性能
 * 
 * @param object_size 对象大小
 * @param count 对象数量
 * @return double 耗时（毫秒）
 */
double test_memory_pool_allocation(size_t object_size, size_t count)
{
    /* 创建内存池 */
    mem_pool_t* pool = mem_pool_create(object_size, count, NULL);
    if (pool == NULL) {
        perror("Failed to create memory pool");
        exit(EXIT_FAILURE);
    }
    
    clock_t start = start_timer();
    
    void** objects = (void**)malloc(count * sizeof(void*));
    if (objects == NULL) {
        perror("Failed to allocate memory for objects array");
        exit(EXIT_FAILURE);
    }
    
    /* 从内存池分配对象 */
    for (size_t i = 0; i < count; i++) {
        objects[i] = mem_pool_alloc(pool);
        if (objects[i] == NULL) {
            perror("Failed to allocate object from memory pool");
            exit(EXIT_FAILURE);
        }
    }
    
    /* 释放对象回内存池 */
    for (size_t i = 0; i < count; i++) {
        mem_pool_free(pool, objects[i]);
    }
    
    free(objects);
    
    double elapsed = stop_timer(start);
    
    /* 销毁内存池 */
    mem_pool_destroy(pool);
    
    return elapsed;
}

/**
 * @brief 测试对象池分配性能
 * 
 * @param object_size 对象大小
 * @param count 对象数量
 * @return double 耗时（毫秒）
 */
double test_object_pool_allocation(size_t object_size, size_t count)
{
    /* 创建对象池 */
    obj_pool_t* pool = obj_pool_create(object_size, count, count, NULL, NULL);
    if (pool == NULL) {
        perror("Failed to create object pool");
        exit(EXIT_FAILURE);
    }
    
    clock_t start = start_timer();
    
    void** objects = (void**)malloc(count * sizeof(void*));
    if (objects == NULL) {
        perror("Failed to allocate memory for objects array");
        exit(EXIT_FAILURE);
    }
    
    /* 从对象池分配对象 */
    for (size_t i = 0; i < count; i++) {
        objects[i] = obj_pool_alloc(pool);
        if (objects[i] == NULL) {
            perror("Failed to allocate object from object pool");
            exit(EXIT_FAILURE);
        }
    }
    
    /* 释放对象回对象池 */
    for (size_t i = 0; i < count; i++) {
        obj_pool_free(pool, objects[i]);
    }
    
    free(objects);
    
    double elapsed = stop_timer(start);
    
    /* 销毁对象池 */
    obj_pool_destroy(pool);
    
    return elapsed;
}

/**
 * @brief 测试向量容器标准分配性能
 * 
 * @param object_size 对象大小
 * @param count 对象数量
 * @return double 耗时（毫秒）
 */
double test_vector_standard_allocation(size_t object_size, size_t count)
{
    clock_t start = start_timer();
    
    /* 创建向量 */
    vector_t* vector = vector_create(object_size, 0, NULL, NULL);
    if (vector == NULL) {
        perror("Failed to create vector");
        exit(EXIT_FAILURE);
    }
    
    /* 添加元素 */
    for (size_t i = 0; i < count; i++) {
        /* 创建适当大小的测试数据 */
        char test_data[object_size];
        memset(test_data, 0, object_size);
        /* 存储索引值，确保数据大小匹配 */
        if (object_size >= sizeof(size_t)) {
            *(size_t*)test_data = i;
        } else if (object_size >= sizeof(int)) {
            *(int*)test_data = (int)i;
        }
        
        if (vector_push_back(vector, test_data) != CSTL_OK) {
            perror("Failed to push element to vector");
            exit(EXIT_FAILURE);
        }
    }
    
    /* 删除元素 */
    for (size_t i = 0; i < count; i++) {
        if (vector_pop_back(vector) != CSTL_OK) {
            perror("Failed to pop element from vector");
            exit(EXIT_FAILURE);
        }
    }
    
    /* 销毁向量 */
    vector_destroy(vector);
    
    return stop_timer(start);
}

/**
 * @brief 测试向量容器内存池分配性能
 * 
 * @param object_size 对象大小
 * @param count 对象数量
 * @return double 耗时（毫秒）
 */
double test_vector_memory_pool_allocation(size_t object_size, size_t count)
{
    /* 创建内存池 - 块大小要足够容纳vector的连续内存需求 */
    /* 估算vector最终容量，预留足够空间 */
    size_t estimated_capacity = count * 2;  /* 考虑vector的增长策略 */
    size_t pool_block_size = object_size * estimated_capacity;
    
    mem_pool_t* pool = mem_pool_create(pool_block_size, 4, NULL);  /* 分配4个大块 */
    if (pool == NULL) {
        perror("Failed to create memory pool");
        exit(EXIT_FAILURE);
    }
    
    clock_t start = start_timer();
    
    /* 创建向量 */
    vector_t* vector = vector_create(object_size, 0, NULL, NULL);
    if (vector == NULL) {
        perror("Failed to create vector");
        exit(EXIT_FAILURE);
    }
    
    /* 设置内存池 */
    vector_set_memory_pool(vector, pool);
    
    /* 添加元素 */
    for (size_t i = 0; i < count; i++) {
        /* 创建适当大小的测试数据 */
        char test_data[object_size];
        memset(test_data, 0, object_size);
        /* 存储索引值，确保数据大小匹配 */
        if (object_size >= sizeof(size_t)) {
            *(size_t*)test_data = i;
        } else if (object_size >= sizeof(int)) {
            *(int*)test_data = (int)i;
        }
        
        if (vector_push_back(vector, test_data) != CSTL_OK) {
            perror("Failed to push element to vector");
            exit(EXIT_FAILURE);
        }
    }
    
    /* 删除元素 */
    for (size_t i = 0; i < count; i++) {
        if (vector_pop_back(vector) != CSTL_OK) {
            perror("Failed to pop element from vector");
            exit(EXIT_FAILURE);
        }
    }
    
    /* 销毁向量 */
    vector_destroy(vector);
    
    double elapsed = stop_timer(start);
    
    /* 销毁内存池 */
    mem_pool_destroy(pool);
    
    return elapsed;
}

/**
 * @brief 测试链表容器标准分配性能
 * 
 * @param object_size 对象大小
 * @param count 对象数量
 * @return double 耗时（毫秒）
 */
double test_list_standard_allocation(size_t object_size, size_t count)
{
    clock_t start = start_timer();
    
    /* 创建链表 */
    list_t* list = list_create(object_size, NULL, NULL);
    if (list == NULL) {
        perror("Failed to create list");
        exit(EXIT_FAILURE);
    }
    
    /* 添加元素 */
    for (size_t i = 0; i < count; i++) {
        /* 创建适当大小的测试数据 */
        char test_data[object_size];
        memset(test_data, 0, object_size);
        /* 存储索引值，确保数据大小匹配 */
        if (object_size >= sizeof(size_t)) {
            *(size_t*)test_data = i;
        } else if (object_size >= sizeof(int)) {
            *(int*)test_data = (int)i;
        }
        
        if (list_push_back(list, test_data) != CSTL_OK) {
            perror("Failed to push element to list");
            exit(EXIT_FAILURE);
        }
    }
    
    /* 删除元素 */
    for (size_t i = 0; i < count; i++) {
        if (list_pop_back(list) != CSTL_OK) {
            perror("Failed to pop element from list");
            exit(EXIT_FAILURE);
        }
    }
    
    /* 销毁链表 */
    list_destroy(list);
    
    return stop_timer(start);
}

/**
 * @brief 测试链表容器对象池分配性能
 * 
 * @param object_size 对象大小
 * @param count 对象数量
 * @return double 耗时（毫秒）
 */
double test_list_object_pool_allocation(size_t object_size, size_t count)
{
    /* 创建节点对象池 - list_node_t只包含指针，不包含数据 */
    obj_pool_t* pool = obj_pool_create(sizeof(list_node_t), count * 2, count * 2, NULL, NULL);
    if (pool == NULL) {
        perror("Failed to create object pool");
        exit(EXIT_FAILURE);
    }
    
    clock_t start = start_timer();
    
    /* 创建链表 */
    list_t* list = list_create(object_size, NULL, NULL);
    if (list == NULL) {
        perror("Failed to create list");
        exit(EXIT_FAILURE);
    }
    
    /* 设置节点对象池 */
    list_set_node_pool(list, pool);
    
    /* 添加元素 */
    for (size_t i = 0; i < count; i++) {
        /* 创建适当大小的测试数据 */
        char test_data[object_size];
        memset(test_data, 0, object_size);
        /* 存储索引值，确保数据大小匹配 */
        if (object_size >= sizeof(size_t)) {
            *(size_t*)test_data = i;
        } else if (object_size >= sizeof(int)) {
            *(int*)test_data = (int)i;
        }
        
        if (list_push_back(list, test_data) != CSTL_OK) {
            perror("Failed to push element to list");
            exit(EXIT_FAILURE);
        }
    }
    
    /* 删除元素 */
    for (size_t i = 0; i < count; i++) {
        if (list_pop_back(list) != CSTL_OK) {
            perror("Failed to pop element from list");
            exit(EXIT_FAILURE);
        }
    }
    
    /* 销毁链表 */
    list_destroy(list);
    
    double elapsed = stop_timer(start);
    
    /* 销毁对象池 */
    obj_pool_destroy(pool);
    
    return elapsed;
}

/**
 * @brief 打印性能对比结果
 * 
 * @param test_name 测试名称
 * @param standard_time 标准分配时间
 * @param pool_time 池分配时间
 */
void print_performance_comparison(const char* test_name, double standard_time, double pool_time)
{
    double improvement = (standard_time - pool_time) / standard_time * 100;
    
    printf("%s:\n", test_name);
    printf("  标准分配: %.3f 毫秒\n", standard_time);
    printf("  池分配:   %.3f 毫秒\n", pool_time);
    printf("  性能提升: %.2f%%\n", improvement);
    printf("\n");
}

/**
 * @brief 运行内存池性能测试
 */
void run_memory_pool_tests(void)
{
    printf("=== 内存池性能测试 ===\n");
    printf("测试元素数量: %d\n", TEST_ELEMENT_COUNT);
    printf("测试迭代次数: %d\n", TEST_ITERATIONS);
    printf("\n");
    
    /* 小对象测试 */
    printf("--- 小对象测试 (%d 字节) ---\n", SMALL_OBJECT_SIZE);
    
    double small_standard_total = 0;
    double small_pool_total = 0;
    
    for (int i = 0; i < TEST_ITERATIONS; i++) {
        small_standard_total += test_standard_allocation(SMALL_OBJECT_SIZE, TEST_ELEMENT_COUNT);
        small_pool_total += test_memory_pool_allocation(SMALL_OBJECT_SIZE, TEST_ELEMENT_COUNT);
    }
    
    double small_standard_avg = small_standard_total / TEST_ITERATIONS;
    double small_pool_avg = small_pool_total / TEST_ITERATIONS;
    
    print_performance_comparison("\xe5\xb0\x8f\xe5\xaf\xb9\xe8\xb1\xa1\xe5\x88\x86\xe9\x85\x8d", small_standard_avg, small_pool_avg);
    
    /* 中等对象测试 */
    printf("--- 中等对象测试 (%d 字节) ---\n", MEDIUM_OBJECT_SIZE);
    
    double medium_standard_total = 0;
    double medium_pool_total = 0;
    
    for (int i = 0; i < TEST_ITERATIONS; i++) {
        medium_standard_total += test_standard_allocation(MEDIUM_OBJECT_SIZE, TEST_ELEMENT_COUNT);
        medium_pool_total += test_memory_pool_allocation(MEDIUM_OBJECT_SIZE, TEST_ELEMENT_COUNT);
    }
    
    double medium_standard_avg = medium_standard_total / TEST_ITERATIONS;
    double medium_pool_avg = medium_pool_total / TEST_ITERATIONS;
    
    print_performance_comparison("\xe4\xb8\xad\xe7\xad\x89\xe5\xaf\xb9\xe8\xb1\xa1\xe5\x88\x86\xe9\x85\x8d", medium_standard_avg, medium_pool_avg);
    
    /* 大对象测试 */
    printf("--- 大对象测试 (%d 字节) ---\n", LARGE_OBJECT_SIZE);
    
    double large_standard_total = 0;
    double large_pool_total = 0;
    
    for (int i = 0; i < TEST_ITERATIONS; i++) {
        large_standard_total += test_standard_allocation(LARGE_OBJECT_SIZE, TEST_ELEMENT_COUNT / 10);  /* 减少数量以避免内存不足 */
        large_pool_total += test_memory_pool_allocation(LARGE_OBJECT_SIZE, TEST_ELEMENT_COUNT / 10);
    }
    
    double large_standard_avg = large_standard_total / TEST_ITERATIONS;
    double large_pool_avg = large_pool_total / TEST_ITERATIONS;
    
    print_performance_comparison("\xe5\xa4\xa7\xe5\xaf\xb9\xe8\xb1\xa1\xe5\x88\x86\xe9\x85\x8d", large_standard_avg, large_pool_avg);
}

/**
 * @brief 运行对象池性能测试
 */
void run_object_pool_tests(void)
{
    printf("=== 对象池性能测试 ===\n");
    printf("测试元素数量: %d\n", TEST_ELEMENT_COUNT);
    printf("测试迭代次数: %d\n", TEST_ITERATIONS);
    printf("\n");
    
    /* 小对象测试 */
    printf("--- 小对象测试 (%d 字节) ---\n", SMALL_OBJECT_SIZE);
    
    double small_standard_total = 0;
    double small_pool_total = 0;
    
    for (int i = 0; i < TEST_ITERATIONS; i++) {
        small_standard_total += test_standard_allocation(SMALL_OBJECT_SIZE, TEST_ELEMENT_COUNT);
        small_pool_total += test_object_pool_allocation(SMALL_OBJECT_SIZE, TEST_ELEMENT_COUNT);
    }
    
    double small_standard_avg = small_standard_total / TEST_ITERATIONS;
    double small_pool_avg = small_pool_total / TEST_ITERATIONS;
    
    print_performance_comparison("\xe5\xb0\x8f\xe5\xaf\xb9\xe8\xb1\xa1\xe5\x88\x86\xe9\x85\x8d", small_standard_avg, small_pool_avg);
    
    /* 中等对象测试 */
    printf("--- 中等对象测试 (%d 字节) ---\n", MEDIUM_OBJECT_SIZE);
    
    double medium_standard_total = 0;
    double medium_pool_total = 0;
    
    for (int i = 0; i < TEST_ITERATIONS; i++) {
        medium_standard_total += test_standard_allocation(MEDIUM_OBJECT_SIZE, TEST_ELEMENT_COUNT);
        medium_pool_total += test_object_pool_allocation(MEDIUM_OBJECT_SIZE, TEST_ELEMENT_COUNT);
    }
    
    double medium_standard_avg = medium_standard_total / TEST_ITERATIONS;
    double medium_pool_avg = medium_pool_total / TEST_ITERATIONS;
    
    print_performance_comparison("\xe4\xb8\xad\xe7\xad\x89\xe5\xaf\xb9\xe8\xb1\xa1\xe5\x88\x86\xe9\x85\x8d", medium_standard_avg, medium_pool_avg);
    
    /* 大对象测试 */
    printf("--- 大对象测试 (%d 字节) ---\n", LARGE_OBJECT_SIZE);
    
    double large_standard_total = 0;
    double large_pool_total = 0;
    
    for (int i = 0; i < TEST_ITERATIONS; i++) {
        large_standard_total += test_standard_allocation(LARGE_OBJECT_SIZE, TEST_ELEMENT_COUNT / 10);  /* 减少数量以避免内存不足 */
        large_pool_total += test_object_pool_allocation(LARGE_OBJECT_SIZE, TEST_ELEMENT_COUNT / 10);
    }
    
    double large_standard_avg = large_standard_total / TEST_ITERATIONS;
    double large_pool_avg = large_pool_total / TEST_ITERATIONS;
    
    print_performance_comparison("\xe5\xa4\xa7\xe5\xaf\xb9\xe8\xb1\xa1\xe5\x88\x86\xe9\x85\x8d", large_standard_avg, large_pool_avg);
}

/**
 * @brief 运行容器内存池性能测试
 */
void run_container_memory_pool_tests(void)
{
    printf("=== 容器内存池性能测试 ===\n");
    printf("测试元素数量: %d\n", TEST_ELEMENT_COUNT);
    printf("测试迭代次数: %d\n", TEST_ITERATIONS);
    printf("\n");
    
    /* 向量容器测试 */
    printf("--- 向量容器测试 ---\n");
    
    double vector_standard_total = 0;
    double vector_pool_total = 0;
    
    for (int i = 0; i < TEST_ITERATIONS; i++) {
        vector_standard_total += test_vector_standard_allocation(sizeof(int), TEST_ELEMENT_COUNT);
        vector_pool_total += test_vector_memory_pool_allocation(sizeof(int), TEST_ELEMENT_COUNT);
    }
    
    double vector_standard_avg = vector_standard_total / TEST_ITERATIONS;
    double vector_pool_avg = vector_pool_total / TEST_ITERATIONS;
    
    print_performance_comparison("\xe5\x90\x91\xe9\x87\x8f\xe5\xae\xb9\xe5\x99\xa8\xe5\x88\x86\xe9\x85\x8d", vector_standard_avg, vector_pool_avg);
    
    /* 链表容器测试 */
    printf("--- 链表容器测试 ---\n");
    
    double list_standard_total = 0;
    double list_pool_total = 0;
    
    for (int i = 0; i < TEST_ITERATIONS; i++) {
        list_standard_total += test_list_standard_allocation(sizeof(int), TEST_ELEMENT_COUNT);
        list_pool_total += test_list_object_pool_allocation(sizeof(int), TEST_ELEMENT_COUNT);
    }
    
    double list_standard_avg = list_standard_total / TEST_ITERATIONS;
    double list_pool_avg = list_pool_total / TEST_ITERATIONS;
    
    print_performance_comparison("\xe9\x93\xbe\xe8\xa1\xa8\xe5\xae\xb9\xe5\x99\xa8\xe5\x88\x86\xe9\x85\x8d", list_standard_avg, list_pool_avg);
}

/**
 * @brief 主函数
 * 
 * @param argc 参数计数
 * @param argv 参数向量
 * @return int 退出代码
 */
int main(int argc, char* argv[])
{
    printf("CSTL库内存池和对象池性能测试示例\n");
    printf("版本: %s\n", cstl_version());
    
    /* 运行内存池性能测试 */
    run_memory_pool_tests();
    
    /* 运行对象池性能测试 */
    run_object_pool_tests();
    
    /* 运行容器内存池性能测试 */
    run_container_memory_pool_tests();
    
    printf("所有测试完成！\n");
    
    return 0;
}
