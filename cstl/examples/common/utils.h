/**
 * @file utils.h
 * @author wzy (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-09-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "cstl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>      // 用于 time()


int64_t random_int64(int64_t min, int64_t max);


// 获取当前时间（毫秒）
long long get_current_time_ms_high_precision();


// 排序算法比较函数
int compare_a_b(const void* a, const void* b);

