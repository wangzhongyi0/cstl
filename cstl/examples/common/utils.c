#include "utils.h"



int64_t random_int64(int64_t min, int64_t max) {
    // 确保min <= max
    if (min > max) {
        int64_t temp = min;
        min = max;
        max = temp;
    }
    
    // 计算范围
    uint64_t range = (uint64_t)max - (uint64_t)min + 1;
    
    // 如果范围是0，直接返回min
    if (range == 0) {
        return min;
    }
    
    // 生成高质量的随机数
    uint64_t random_value = 0;
    
    // 使用多个rand()调用来构建64位随机数
    // 每次调用提供约15位随机数(RAND_MAX通常是32767)
    random_value = ((uint64_t)rand() & 0x7FFF);
    random_value |= ((uint64_t)rand() & 0x7FFF) << 15;
    random_value |= ((uint64_t)rand() & 0x7FFF) << 30;
    random_value |= ((uint64_t)rand() & 0x7FFF) << 45;
    random_value |= ((uint64_t)rand() & 0x0FFF) << 60;  // 只用12位避免溢出
    
    // 使用模运算确保在指定范围内
    random_value %= range;
    
    return (int64_t)random_value + min;
}


// 获取当前时间（毫秒）- 跨平台版本
long long get_current_time_ms_high_precision() {
#ifdef _WIN32
    // Windows 平台使用高精度计数器
    LARGE_INTEGER frequency;
    LARGE_INTEGER counter;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&counter);
    return (long long)(counter.QuadPart * 1000.0 / frequency.QuadPart);
#else
    // Unix/Linux 平台使用 clock_gettime
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#endif
}


// 排序算法比较函数
int compare_a_b(const void* a, const void* b){
    int16_t aa = (int16_t)*(int16_t*)a;
    int16_t bb = (int16_t)*(int16_t*)b;

    return (aa >= bb) ? ((aa > bb) ? 1 : 0) : -1;
}
