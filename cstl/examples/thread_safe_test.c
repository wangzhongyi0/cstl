/**
 * @file thread_safe_test.c
 * @brief CSTL库的线程安全功能测试示例
 * 
 * 该文件展示了CSTL库的线程安全功能使用方法，包括：
 * - 多线程环境下的向量容器操作
 * - 多线程环境下的双向链表操作
 * - 多线程环境下的栈适配器操作
 * - 多线程环境下的队列适配器操作
 * - 线程安全性能对比
 */

#include "cstl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>
#include <process.h>
typedef HANDLE pthread_t;
typedef unsigned int (__stdcall *thread_func_t)(void*);
#define pthread_create(handle, attr, func, arg) \
    (((*handle) = (HANDLE)_beginthreadex(NULL, 0, (thread_func_t)func, arg, 0, NULL)) == NULL ? -1 : 0)
#define pthread_join(handle, res) \
    (WaitForSingleObject(handle, INFINITE) == WAIT_OBJECT_0 ? 0 : -1)
#define pthread_close(handle) CloseHandle(handle)
#else
#include <pthread.h>
#endif

/**
 * @brief 测试线程数量
 */
#define TEST_THREAD_COUNT 8

/**
 * @brief 每个线程的操作次数
 */
#define OPERATIONS_PER_THREAD 1000

/**
 * @brief 整数比较函数
 * 
 * @param a 第一个整数指针
 * @param b 第二个整数指针
 * @return int 如果a < b返回负数，如果a == b返回0，如果a > b返回正数
 */
int int_compare(const void* a, const void* b)
{
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    
    if (ia < ib) return -1;
    if (ia > ib) return 1;
    return 0;
}

/**
 * @brief 线程共享数据结构
 */
typedef struct {
    vector_t* vector;      /**< 线程安全的向量指针 */
    list_t* list;          /**< 线程安全的链表指针 */
    stack_t* stack;        /**< 线程安全的栈指针 */
    queue_t* queue;        /**< 线程安全的队列指针 */
    int thread_id;         /**< 线程ID */
    int success_count;     /**< 成功操作计数 */
    int failure_count;     /**< 失败操作计数 */
} thread_data_t;

/**
 * @brief 向量线程函数
 * 
 * @param arg 线程数据指针
 * @return void* 返回值
 */
#ifdef _WIN32
unsigned int __stdcall vector_thread_func(void* arg)
#else
void* vector_thread_func(void* arg)
#endif
{
    thread_data_t* data = (thread_data_t*)arg;
    data->success_count = 0;
    data->failure_count = 0;
    
    for (int i = 0; i < OPERATIONS_PER_THREAD; i++) {
        int value = data->thread_id * OPERATIONS_PER_THREAD + i;
        
        /* 添加元素 */
        if (vector_push_back(data->vector, &value) == CSTL_OK) {
            data->success_count++;
        } else {
            data->failure_count++;
        }
        
        /* 随机删除元素 */
        if (i % 10 == 0 && vector_size(data->vector) > 0) {
            if (vector_pop_back(data->vector) == CSTL_OK) {
                data->success_count++;
            } else {
                data->failure_count++;
            }
        }
        
        /* 随机访问元素 */
        if (i % 5 == 0 && vector_size(data->vector) > 0) {
            size_t index = i % vector_size(data->vector);
            int* element = NULL;
            if (vector_at(data->vector, index, (void**)&element) == CSTL_OK) {
                data->success_count++;
            } else {
                data->failure_count++;
            }
        }
    }
    
#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

/**
 * @brief 链表线程函数
 * 
 * @param arg 线程数据指针
 * @return void* 返回值
 */
#ifdef _WIN32
unsigned int __stdcall list_thread_func(void* arg)
#else
void* list_thread_func(void* arg)
#endif
{
    thread_data_t* data = (thread_data_t*)arg;
    data->success_count = 0;
    data->failure_count = 0;
    
    for (int i = 0; i < OPERATIONS_PER_THREAD; i++) {
        int value = data->thread_id * OPERATIONS_PER_THREAD + i;
        
        /* 添加元素到尾部 */
        if (list_push_back(data->list, &value) == CSTL_OK) {
            data->success_count++;
        } else {
            data->failure_count++;
        }
        
        /* 随机添加元素到头部 */
        if (i % 10 == 0) {
            if (list_push_front(data->list, &value) == CSTL_OK) {
                data->success_count++;
            } else {
                data->failure_count++;
            }
        }
        
        /* 随机删除元素 */
        if (i % 15 == 0 && list_size(data->list) > 0) {
            if (i % 2 == 0) {
                if (list_pop_back(data->list) == CSTL_OK) {
                    data->success_count++;
                } else {
                    data->failure_count++;
                }
            } else {
                if (list_pop_front(data->list) == CSTL_OK) {
                    data->success_count++;
                } else {
                    data->failure_count++;
                }
            }
        }
        
        /* 随机访问元素 */
        if (i % 7 == 0 && list_size(data->list) > 0) {
            size_t index = i % list_size(data->list);
            int* element = NULL;
            if (list_at(data->list, index, (void**)&element) == CSTL_OK) {
                data->success_count++;
            } else {
                data->failure_count++;
            }
        }
    }
    
#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

/**
 * @brief 栈线程函数
 * 
 * @param arg 线程数据指针
 * @return void* 返回值
 */
#ifdef _WIN32
unsigned int __stdcall stack_thread_func(void* arg)
#else
void* stack_thread_func(void* arg)
#endif
{
    thread_data_t* data = (thread_data_t*)arg;
    data->success_count = 0;
    data->failure_count = 0;
    
    for (int i = 0; i < OPERATIONS_PER_THREAD; i++) {
        int value = data->thread_id * OPERATIONS_PER_THREAD + i;
        
        /* 入栈操作 */
        if (stack_push(data->stack, &value) == CSTL_OK) {
            data->success_count++;
        } else {
            data->failure_count++;
        }
        
        /* 随机出栈操作 */
        if (i % 3 == 0 && !stack_empty(data->stack)) {
            if (stack_pop(data->stack) == CSTL_OK) {
                data->success_count++;
            } else {
                data->failure_count++;
            }
        }
        
        /* 随机访问栈顶元素 */
        if (i % 5 == 0 && !stack_empty(data->stack)) {
            int* element = NULL;
            if (stack_top(data->stack, (void**)&element) == CSTL_OK) {
                data->success_count++;
            } else {
                data->failure_count++;
            }
        }
    }
    
#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

/**
 * @brief 队列线程函数
 * 
 * @param arg 线程数据指针
 * @return void* 返回值
 */
#ifdef _WIN32
unsigned int __stdcall queue_thread_func(void* arg)
#else
void* queue_thread_func(void* arg)
#endif
{
    thread_data_t* data = (thread_data_t*)arg;
    data->success_count = 0;
    data->failure_count = 0;
    
    for (int i = 0; i < OPERATIONS_PER_THREAD; i++) {
        int value = data->thread_id * OPERATIONS_PER_THREAD + i;
        
        /* 入队操作 */
        if (queue_push(data->queue, &value) == CSTL_OK) {
            data->success_count++;
        } else {
            data->failure_count++;
        }
        
        /* 随机出队操作 */
        if (i % 3 == 0 && !queue_empty(data->queue)) {
            if (queue_pop(data->queue) == CSTL_OK) {
                data->success_count++;
            } else {
                data->failure_count++;
            }
        }
        
        /* 随机访问队首元素 */
        if (i % 7 == 0 && !queue_empty(data->queue)) {
            int* element = NULL;
            if (queue_front(data->queue, (void**)&element) == CSTL_OK) {
                data->success_count++;
            } else {
                data->failure_count++;
            }
        }
        
        /* 随机访问队尾元素 */
        if (i % 11 == 0 && !queue_empty(data->queue)) {
            int* element = NULL;
            if (queue_back(data->queue, (void**)&element) == CSTL_OK) {
                data->success_count++;
            } else {
                data->failure_count++;
            }
        }
    }
    
#ifdef _WIN32
    return 0;
#else
    return NULL;
#endif
}

/**
 * @brief 运行线程安全测试
 * 
 * @param test_name 测试名称
 * @param thread_func 线程函数
 * @param data 线程数据数组
 */
#ifdef _WIN32
void run_thread_safe_test(const char* test_name, unsigned int (__stdcall *thread_func)(void*), thread_data_t* data)
#else
void run_thread_safe_test(const char* test_name, void* (*thread_func)(void*), thread_data_t* data)
#endif
{
    printf("\n=== %s ===\n", test_name);
    
    pthread_t threads[TEST_THREAD_COUNT];
    
    /* 创建线程 */
    for (int i = 0; i < TEST_THREAD_COUNT; i++) {
        data[i].thread_id = i;
        if (pthread_create(&threads[i], NULL, thread_func, &data[i]) != 0) {
            perror("Failed to create thread");
            exit(EXIT_FAILURE);
        }
    }
    
    /* 等待线程完成 */
    for (int i = 0; i < TEST_THREAD_COUNT; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
            exit(EXIT_FAILURE);
        }
    }
    
    /* 统计结果 */
    int total_success = 0;
    int total_failure = 0;
    for (int i = 0; i < TEST_THREAD_COUNT; i++) {
        total_success += data[i].success_count;
        total_failure += data[i].failure_count;
        printf("Thread %d: %d successes, %d failures\n", 
               i, data[i].success_count, data[i].failure_count);
    }
    
    printf("Total: %d successes, %d failures\n", total_success, total_failure);
    
    /* 验证数据完整性 */
    if (strcmp(test_name, "\xe5\x90\x91\xe9\x87\x8f\xe7\xba\xbf\xe7\xa8\x8b\xe5\xae\x89\xe5\x85\xa8\xe6\xb5\x8b\xe8\xaf\x95") == 0) {
        size_t expected_size = TEST_THREAD_COUNT * OPERATIONS_PER_THREAD;
        for (int i = 0; i < TEST_THREAD_COUNT; i++) {
            expected_size -= OPERATIONS_PER_THREAD / 10;  /* 减去pop_back操作 */
        }
        
        size_t actual_size = vector_size(data[0].vector);
        printf("Expected size: %zu, Actual size: %zu\n", expected_size, actual_size);
        
        /* 检查是否有重复或缺失的元素 */
        int* found = (int*)calloc(TEST_THREAD_COUNT * OPERATIONS_PER_THREAD, sizeof(int));
        if (found == NULL) {
            perror("Failed to allocate memory");
            exit(EXIT_FAILURE);
        }
        
        iterator_t* begin = vector_begin(data[0].vector);
        iterator_t* end = vector_end(data[0].vector);
        
        while (iterator_valid(begin) && !iterator_equal(begin, end)) {
            int* element = NULL;
            iterator_get(begin, (void**)&element);
            if (*element >= 0 && *element < TEST_THREAD_COUNT * OPERATIONS_PER_THREAD) {
                found[*element]++;
            }
            iterator_next(begin);
        }
        
        iterator_destroy(begin);
        iterator_destroy(end);
        
        /* 检查是否有重复 */
        int duplicates = 0;
        for (int i = 0; i < TEST_THREAD_COUNT * OPERATIONS_PER_THREAD; i++) {
            if (found[i] > 1) {
                duplicates++;
            }
        }
        
        free(found);
        printf("Duplicate elements: %d\n", duplicates);
    }
    else if (strcmp(test_name, "\xe9\x93\xbe\xe8\xa1\xa8\xe7\xba\xbf\xe7\xa8\x8b\xe5\xae\x89\xe5\x85\xa8\xe6\xb5\x8b\xe8\xaf\x95") == 0) {
        size_t expected_size = TEST_THREAD_COUNT * OPERATIONS_PER_THREAD;
        for (int i = 0; i < TEST_THREAD_COUNT; i++) {
            expected_size += OPERATIONS_PER_THREAD / 10;  /* 加上push_front操作 */
            expected_size -= OPERATIONS_PER_THREAD / 15;  /* 减去pop操作 */
        }
        
        size_t actual_size = list_size(data[0].list);
        printf("Expected size: %zu, Actual size: %zu\n", expected_size, actual_size);
    }
    else if (strcmp(test_name, "\xe6\xa0\x88\xe7\xba\xbf\xe7\xa8\x8b\xe5\xae\x89\xe5\x85\xa8\xe6\xb5\x8b\xe8\xaf\x95") == 0) {
        size_t expected_size = TEST_THREAD_COUNT * OPERATIONS_PER_THREAD;
        for (int i = 0; i < TEST_THREAD_COUNT; i++) {
            expected_size -= OPERATIONS_PER_THREAD / 3;  /* 减去pop操作 */
        }
        
        size_t actual_size = stack_size(data[0].stack);
        printf("Expected size: %zu, Actual size: %zu\n", expected_size, actual_size);
    }
    else if (strcmp(test_name, "\xe9\x98\x9f\xe5\x88\x97\xe7\xba\xbf\xe7\xa8\x8b\xe5\xae\x89\xe5\x85\xa8\xe6\xb5\x8b\xe8\xaf\x95") == 0) {
        size_t expected_size = TEST_THREAD_COUNT * OPERATIONS_PER_THREAD;
        for (int i = 0; i < TEST_THREAD_COUNT; i++) {
            expected_size -= OPERATIONS_PER_THREAD / 3;  /* 减去pop操作 */
        }
        
        size_t actual_size = queue_size(data[0].queue);
        printf("Expected size: %zu, Actual size: %zu\n", expected_size, actual_size);
    }
}

/**
 * @brief 比较线程安全和非线程安全性能
 */
void performance_comparison(void)
{
    printf("\n=== 线程安全性能对比 ===\n");
    
    /* 创建非线程安全的向量 */
    vector_t* vector_no_ts = vector_create(sizeof(int), 0, NULL, NULL);
    
    /* 创建线程安全的向量 */
    vector_t* vector_ts = vector_create(sizeof(int), 0, NULL, NULL);
    vector_enable_thread_safety(vector_ts);
    
    pthread_t threads[TEST_THREAD_COUNT];
    thread_data_t data[TEST_THREAD_COUNT];
    
    /* 测试非线程安全向量 */
    printf("\xe6\xb5\x8b\xe8\xaf\x95\xe9\x9d\x9e\xe7\xba\xbf\xe7\xa8\x8b\xe5\xae\x89\xe5\x85\xa8\xe5\x90\x91\xe9\x87\x8f...\n");
    for (int i = 0; i < TEST_THREAD_COUNT; i++) {
        data[i].vector = vector_no_ts;
        data[i].thread_id = i;
    }
    
    clock_t start = clock();
    for (int i = 0; i < TEST_THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, vector_thread_func, &data[i]);
    }
    
    for (int i = 0; i < TEST_THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }
    clock_t end = clock();
    
    double time_no_ts = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\xe9\x9d\x9e\xe7\xba\xbf\xe7\xa8\x8b\xe5\xae\x89\xe5\x85\xa8\xe5\x90\x91\xe9\x87\x8f\xe8\x80\x97\xe6\x97\xb6: %.3f \xe7\xa7\x92\n", time_no_ts);
    
    /* 清理非线程安全向量 */
    vector_clear(vector_no_ts);
    
    /* 测试线程安全向量 */
    printf("\xe6\xb5\x8b\xe8\xaf\x95\xe7\xba\xbf\xe7\xa8\x8b\xe5\xae\x89\xe5\x85\xa8\xe5\x90\x91\xe9\x87\x8f...\n");
    for (int i = 0; i < TEST_THREAD_COUNT; i++) {
        data[i].vector = vector_ts;
        data[i].thread_id = i;
    }
    
    start = clock();
    for (int i = 0; i < TEST_THREAD_COUNT; i++) {
        pthread_create(&threads[i], NULL, vector_thread_func, &data[i]);
    }
    
    for (int i = 0; i < TEST_THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }
    end = clock();
    
    double time_ts = (double)(end - start) / CLOCKS_PER_SEC;
    printf("\xe7\xba\xbf\xe7\xa8\x8b\xe5\xae\x89\xe5\x85\xa8\xe5\x90\x91\xe9\x87\x8f\xe8\x80\x97\xe6\x97\xb6: %.3f \xe7\xa7\x92\n", time_ts);
    
    /* 计算性能开销 */
    double overhead = (time_ts - time_no_ts) / time_no_ts * 100;
    printf("\xe7\xba\xbf\xe7\xa8\x8b\xe5\xae\x89\xe5\x85\xa8\xe6\x80\xa7\xe8\x83\xbd\xe5\xbc\x80\xe9\x94\x80: %.2f%%\n", overhead);
    
    /* 清理 */
    vector_destroy(vector_no_ts);
    vector_destroy(vector_ts);
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
    printf("CSTL库线程安全功能测试示例\n");
    printf("版本: %s\n", cstl_version());
    printf("线程数量: %d\n", TEST_THREAD_COUNT);
    printf("每个线程的操作次数: %d\n", OPERATIONS_PER_THREAD);
    
    /* 创建线程安全的数据结构 */
    vector_t* vector = vector_create(sizeof(int), 0, NULL, NULL);
    vector_enable_thread_safety(vector);
    
    list_t* list = list_create(sizeof(int), NULL, NULL);
    list_enable_thread_safety(list);
    
    stack_t* stack = stack_create(sizeof(int), 0, NULL, NULL);
    stack_enable_thread_safety(stack);
    
    queue_t* queue = queue_create(sizeof(int), NULL, NULL);
    queue_enable_thread_safety(queue);
    
    /* 准备线程数据 */
    thread_data_t vector_data[TEST_THREAD_COUNT];
    thread_data_t list_data[TEST_THREAD_COUNT];
    thread_data_t stack_data[TEST_THREAD_COUNT];
    thread_data_t queue_data[TEST_THREAD_COUNT];
    
    for (int i = 0; i < TEST_THREAD_COUNT; i++) {
        vector_data[i].vector = vector;
        list_data[i].list = list;
        stack_data[i].stack = stack;
        queue_data[i].queue = queue;
    }
    
    /* 运行线程安全测试 */
    run_thread_safe_test("\xe5\x90\x91\xe9\x87\x8f\xe7\xba\xbf\xe7\xa8\x8b\xe5\xae\x89\xe5\x85\xa8\xe6\xb5\x8b\xe8\xaf\x95", vector_thread_func, vector_data);
    run_thread_safe_test("\xe9\x93\xbe\xe8\xa1\xa8\xe7\xba\xbf\xe7\xa8\x8b\xe5\xae\x89\xe5\x85\xa8\xe6\xb5\x8b\xe8\xaf\x95", list_thread_func, list_data);
    run_thread_safe_test("\xe6\xa0\x88\xe7\xba\xbf\xe7\xa8\x8b\xe5\xae\x89\xe5\x85\xa8\xe6\xb5\x8b\xe8\xaf\x95", stack_thread_func, stack_data);
    run_thread_safe_test("\xe9\x98\x9f\xe5\x88\x97\xe7\xba\xbf\xe7\xa8\x8b\xe5\xae\x89\xe5\x85\xa8\xe6\xb5\x8b\xe8\xaf\x95", queue_thread_func, queue_data);
    
    /* 性能对比 */
    performance_comparison();
    
    /* 清理资源 */
    vector_destroy(vector);
    list_destroy(list);
    stack_destroy(stack);
    queue_destroy(queue);
    
    printf("\n所有测试完成！\n");
    
    return 0;
}
