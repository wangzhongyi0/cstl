/**
 * @file sorting_performance_test.c
 * @brief 排序算法性能测试
 * @author Claude
 * @version 0.1
 * @date 2025-09-02
 * 
 * @copyright Copyright (c) 2025
 * 
 * 本文件测试各种排序算法的性能，包括：
 * - 快速排序 (SORT_QUICK)
 * - 归并排序 (SORT_MERGE)
 * - 堆排序 (SORT_HEAP)
 * - 插入排序 (SORT_INSERT)
 * 
 * 使用utils中的高精度时间函数计算每种算法的耗时
 * 并将结果输出到日志文件中
 */

#include "cstl.h"
#include "common/utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#include <limits.h>

#define LOG_FILE "sorting_performance.log"
#define TEST_SIZES 5
#define MAX_ARRAY_SIZE 10000
#define NUM_TESTS 3

// 测试数据大小
const size_t test_sizes[TEST_SIZES] = {100, 1000, 5000, 10000, 20000};

// 排序算法名称
const char* algorithm_names[] = {
    "快速排序 (Quick Sort)",
    "归并排序 (Merge Sort)", 
    "堆排序 (Heap Sort)",
    "插入排序 (Insert Sort)"
};

/**
 * @brief 生成随机数组
 * 
 * @param size 数组大小
 * @param min 最小值
 * @param max 最大值
 * @return int16_t* 生成的数组指针
 */
int16_t* generate_random_array(size_t size, int16_t min, int16_t max) {
    int16_t* array = (int16_t*)malloc(size * sizeof(int16_t));
    if (array == NULL) {
        return NULL;
    }
    
    for (size_t i = 0; i < size; i++) {
        array[i] = (int16_t)random_int64(min, max);
    }
    
    return array;
}

/**
 * @brief 复制数组
 * 
 * @param src 源数组
 * @param size 数组大小
 * @return int16_t* 复制的数组指针
 */
int16_t* copy_array(const int16_t* src, size_t size) {
    int16_t* dest = (int16_t*)malloc(size * sizeof(int16_t));
    if (dest == NULL) {
        return NULL;
    }
    
    memcpy(dest, src, size * sizeof(int16_t));
    return dest;
}

/**
 * @brief 验证数组是否已排序
 * 
 * @param array 数组指针
 * @param size 数组大小
 * @return int 1表示已排序，0表示未排序
 */
int is_sorted(const int16_t* array, size_t size) {
    for (size_t i = 0; i < size - 1; i++) {
        if (array[i] > array[i + 1]) {
            return 0;
        }
    }
    return 1;
}

/**
 * @brief 测试单个排序算法的性能
 * 
 * @param algorithm 排序算法类型
 * @param array 数组指针
 * @param size 数组大小
 * @return long long 耗时（毫秒）
 */
long long test_sort_algorithm(sort_algorithm_t algorithm, int16_t* array, size_t size) {
    // 创建vector容器
    vector_t* vec = vector_create(sizeof(int16_t), 0, NULL, NULL);
    if (vec == NULL) {
        return -1;
    }
    
    // 填充数据
    for (size_t i = 0; i < size; i++) {
        vector_push_back(vec, &array[i]);
    }
    
    // 获取迭代器
    iterator_t* begin = vector_begin(vec);
    iterator_t* end = vector_end(vec);
    
    // 记录开始时间
    long long start_time = get_current_time_ms_high_precision();
    
    // 执行排序
    error_code_t result = algo_sort(begin, end, compare_a_b, algorithm);
    
    // 记录结束时间
    long long end_time = get_current_time_ms_high_precision();
    
    // 计算耗时
    long long elapsed_time = end_time - start_time;
    
    // 验证排序结果
    int sorted = 0;
    algo_is_sorted(begin, end, compare_a_b, &sorted);
    
    if (result != CSTL_OK || !sorted) {
        printf("错误: %s 排序失败或结果不正确!\n", algorithm_names[algorithm]);
        elapsed_time = -1;
    }
    
    // 清理资源
    iterator_destroy(begin);
    iterator_destroy(end);
    vector_destroy(vec);
    
    return elapsed_time;
}

/**
 * @brief 运行完整的性能测试
 */
void run_performance_tests() {
    FILE* log_file = fopen(LOG_FILE, "a");
    if (log_file == NULL) {
        printf("无法打开日志文件 %s\n", LOG_FILE);
        return;
    }
    
    // 写入日志头部
    fprintf(log_file, "\n=== 排序算法性能测试报告 ===\n");
    fprintf(log_file, "测试时间: %s", ctime(&(time_t){time(NULL)}));
    fprintf(log_file, "测试数据类型: int16_t\n");
    fprintf(log_file, "测试轮数: %d\n\n", NUM_TESTS);
    
    printf("开始排序算法性能测试...\n");
    printf("结果将保存到 %s\n\n", LOG_FILE);
    
    // 对每个测试大小进行测试
    for (int size_idx = 0; size_idx < TEST_SIZES; size_idx++) {
        size_t size = test_sizes[size_idx];
        
        fprintf(log_file, "--- 测试大小: %zu 元素 ---\n", size);
        printf("测试大小: %zu 元素\n", size);
        
        // 生成测试数据
        int16_t* original_array = generate_random_array(size, -1000, 1000);
        if (original_array == NULL) {
            fprintf(log_file, "错误: 无法生成测试数据\n");
            continue;
        }
        
        // 测试每种排序算法
        for (int algo = 0; algo < 4; algo++) {
            fprintf(log_file, "%s:\n", algorithm_names[algo]);
            printf("测试 %s...\n", algorithm_names[algo]);
            
            long long total_time = 0;
            long long min_time = LONG_MAX;
            long long max_time = 0;
            int successful_tests = 0;
            
            // 运行多轮测试
            for (int test = 0; test < NUM_TESTS; test++) {
                // 复制原始数据
                int16_t* test_array = copy_array(original_array, size);
                if (test_array == NULL) {
                    fprintf(log_file, "  测试 %d: 内存分配失败\n", test + 1);
                    continue;
                }
                
                // 执行测试
                long long elapsed_time = test_sort_algorithm(algo, test_array, size);
                free(test_array);
                
                if (elapsed_time >= 0) {
                    total_time += elapsed_time;
                    if (elapsed_time < min_time) min_time = elapsed_time;
                    if (elapsed_time > max_time) max_time = elapsed_time;
                    successful_tests++;
                }
                
                fprintf(log_file, "  测试 %d: %lld ms\n", test + 1, elapsed_time);
            }
            
            // 计算统计数据
            if (successful_tests > 0) {
                double avg_time = (double)total_time / successful_tests;
                fprintf(log_file, "  平均耗时: %.2f ms\n", avg_time);
                fprintf(log_file, "  最小耗时: %lld ms\n", min_time);
                fprintf(log_file, "  最大耗时: %lld ms\n", max_time);
                fprintf(log_file, "  成功测试: %d/%d\n\n", successful_tests, NUM_TESTS);
                
                printf("  平均耗时: %.2f ms\n", avg_time);
                printf("  最小耗时: %lld ms\n", min_time);
                printf("  最大耗时: %lld ms\n", max_time);
                printf("  成功测试: %d/%d\n\n", successful_tests, NUM_TESTS);
            } else {
                fprintf(log_file, "  所有测试均失败\n\n");
                printf("  所有测试均失败\n\n");
            }
        }
        
        free(original_array);
        fprintf(log_file, "\n");
        printf("\n");
    }
    
    // 性能对比总结
    fprintf(log_file, "=== 性能对比总结 ===\n");
    printf("=== 性能对比总结 ===\n");
    
    for (int algo = 0; algo < 4; algo++) {
        fprintf(log_file, "\n%s:\n", algorithm_names[algo]);
        printf("\n%s:\n", algorithm_names[algo]);
        
        for (int size_idx = 0; size_idx < TEST_SIZES; size_idx++) {
            size_t size = test_sizes[size_idx];
            
            // 生成测试数据
            int16_t* test_array = generate_random_array(size, -1000, 1000);
            if (test_array == NULL) continue;
            
            // 执行测试
            long long elapsed_time = test_sort_algorithm(algo, test_array, size);
            free(test_array);
            
            if (elapsed_time >= 0) {
                double time_per_element = (double)elapsed_time / size;
                fprintf(log_file, "  %zu 元素: %lld ms (%.3f ms/元素)\n", 
                        size, elapsed_time, time_per_element);
                printf("  %zu 元素: %lld ms (%.3f ms/元素)\n", 
                       size, elapsed_time, time_per_element);
            }
        }
    }
    
    fprintf(log_file, "\n=== 测试完成 ===\n\n");
    printf("\n=== 测试完成 ===\n");
    
    fclose(log_file);
}

/**
 * @brief 显示帮助信息
 */
void show_help() {
    printf("排序算法性能测试程序\n");
    printf("用法: ./sorting_performance_test [选项]\n");
    printf("选项:\n");
    printf("  -h, --help     显示帮助信息\n");
    printf("  -r, --run      运行性能测试\n");
    printf("  -c, --clear    清空日志文件\n");
    printf("\n");
    printf("说明:\n");
    printf("  本程序测试四种排序算法的性能:\n");
    printf("  1. 快速排序 (Quick Sort)\n");
    printf("  2. 归并排序 (Merge Sort)\n");
    printf("  3. 堆排序 (Heap Sort)\n");
    printf("  4. 插入排序 (Insert Sort)\n");
    printf("\n");
    printf("  测试结果将保存到 %s 文件中\n", LOG_FILE);
}

/**
 * @brief 主函数
 * 
 * @param argc 参数个数
 * @param argv 参数数组
 * @return int 程序退出码
 */
int main(int argc, char* argv[]) {
    // 初始化随机数种子
    srand((unsigned int)time(NULL));
    
    if (argc < 2) {
        show_help();
        return 0;
    }
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            show_help();
            return 0;
        } else if (strcmp(argv[i], "-r") == 0 || strcmp(argv[i], "--run") == 0) {
            run_performance_tests();
            return 0;
        } else if (strcmp(argv[i], "-c") == 0 || strcmp(argv[i], "--clear") == 0) {
            FILE* log_file = fopen(LOG_FILE, "w");
            if (log_file) {
                fclose(log_file);
                printf("日志文件 %s 已清空\n", LOG_FILE);
            }
            return 0;
        } else {
            printf("未知选项: %s\n", argv[i]);
            show_help();
            return 1;
        }
    }
    
    return 0;
}