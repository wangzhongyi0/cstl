/**
 * @file queue_test_fixed.c
 * @author wzy (1340130745@qq.com)
 * @brief 修复版队列测试 - 演示正确的内存管理
 * @version 1.1
 * @date 2025-09-01
 * 
 * @copyright Copyright (c) 2025
 * 
 * 这个文件修复了原queue_test.c中的内存泄漏问题，
 * 为stl_audio_pcm结构体提供了正确的析构函数。
 */

#include "cstl.h"
#include "data_type.h"
#include "utils.h"

// stl版本的音频数据帧
typedef struct{
    vector_t* data;
    time_t time;
}stl_audio_pcm;

// 普通版本的音频数据帧
typedef struct{
    int16_t data[1024];
    time_t time;
}no_stl_audio_pcm;

// 创建音频数据队列
static queue_t* audio_queue = NULL;
static no_stl_audio_pcm no_stl_audio_queue[512];    // 普通数组实现音频数据队列

// 声明函数
void create_audio_queue(void);
void push_stl_audio_pcm_to_queue(void);
void sort_audio_data(void);
void verify_after_sort(stl_audio_pcm* pcm, uint16_t index);

// **新增**：stl_audio_pcm的析构函数
void stl_audio_pcm_destructor(void* data) {
    if (data == NULL) return;
    
    stl_audio_pcm* pcm = (stl_audio_pcm*)data;
    
    // 释放嵌套的向量容器
    if (pcm->data != NULL) {
        vector_destroy(pcm->data);
        pcm->data = NULL;
    }
    
    printf("释放stl_audio_pcm，时间戳: %ld\n", pcm->time);
}

// 创建数据队列（修复版）
void create_audio_queue(void){
    if(audio_queue != NULL){
        queue_destroy(audio_queue);
        audio_queue = NULL;
    }
    
    // **修复**：创建队列时提供析构函数
    audio_queue = queue_create(sizeof(stl_audio_pcm), NULL, stl_audio_pcm_destructor);

    for(uint16_t i = 0; i < 410; i++){
        push_stl_audio_pcm_to_queue();
    }
    
    printf("创建了包含 %zu 个音频帧的队列（带析构函数）\n", queue_size(audio_queue));
}

// 插入数据帧（保持原样）
void push_stl_audio_pcm_to_queue(void){
    vector_t* audio_samples = vector_create(sizeof(int16_t), 0, NULL, NULL);

    size_t audio_pcm_index = queue_size(audio_queue);

    // 生成随机数
    for(uint16_t i = 0; i < 1024; i++){
        int16_t sample = random_int64(0, 16000);
        vector_push_back(audio_samples, &sample);
        
        no_stl_audio_queue[audio_pcm_index].data[i] = sample;
    }

    // 获取当前时间
    time_t cur_time;
    time(&cur_time);

    stl_audio_pcm new_audio_pcm = {
        .data = audio_samples,
        .time = cur_time,
    };
    no_stl_audio_queue[audio_pcm_index].time = cur_time;

    queue_push(audio_queue, &new_audio_pcm);
}

// 对数据进行排序与验证（保持原样）
void sort_audio_data(void){
    /*
    1-100：快速排序
    101-200：归并排序
    201-300：堆排序
    301-400：插入排序
    */
    printf("开始排序\n");
    sort_algorithm_t sort_type = SORT_QUICK-1;
    for(uint16_t i = 0; i < 400; i++){
        if(i % 100 == 0){
            sort_type++;
            printf("\n\n----------切换排序算法：%d----------\n\n", sort_type);
        }
        
        stl_audio_pcm* cur_pcm;
        queue_front(audio_queue, &cur_pcm);
        queue_pop(audio_queue);
        iterator_t* begin = vector_begin(cur_pcm->data);
        iterator_t* end = vector_end(cur_pcm->data);
        algo_sort(begin, end, compare_a_b, sort_type);
        verify_after_sort(cur_pcm, i);
        iterator_destroy(begin);
        iterator_destroy(end);
    }
    printf("排序完成\n");
}

// 从普通音频帧数组中查找
int find_value_linear(const no_stl_audio_pcm* pcm, int16_t target) {
    if (pcm == NULL) return -1;
    
    for (size_t i = 0; i < 1024; i++) {
        if (pcm->data[i] == target) {
            return (int)i;
        }
    }
    return -1;
}

// 验证排序后的数据（保持原样）
void verify_after_sort(stl_audio_pcm* pcm, uint16_t index){
    // 首先验证排序后的数据与原数据是否一致
    no_stl_audio_pcm* no_stl_pcm = &no_stl_audio_queue[index];
    
    int16_t* sorted_data = NULL;
    size_t sorted_count = 0;
    
    // 获取排序后的数据
    sorted_data = (int16_t*)vector_get_by_index(pcm->data, 0);
    sorted_count = vector_size(pcm->data);
    
    if(sorted_count != 1024){
        printf("错误：数据数量不一致\n");
        return;
    }
    
    // 验证数据是否一致
    for(size_t i = 0; i < sorted_count; i++){
        int16_t sorted_value = *(sorted_data + i);
        int linear_index = find_value_linear(no_stl_pcm, sorted_value);
        
        if(linear_index == -1){
            printf("错误：在原始数据中找不到值 %d\n", sorted_value);
            return;
        }
    }
    
    // 验证排序是否正确
    for(size_t i = 1; i < sorted_count; i++){
        if(*(sorted_data + i - 1) > *(sorted_data + i)){
            printf("错误：排序不正确，位置 %zu 和 %zu\n", i-1, i);
            return;
        }
    }
    
    if(index % 100 == 0){
        printf("验证通过：第 %d 个数据帧排序正确\n", index);
    }
}

// **新增**：演示内存泄漏的对比函数
void demonstrate_memory_leak(void) {
    printf("\n=== 内存泄漏演示 ===\n");
    
    // 错误示例：不提供析构函数
    printf("1. 错误示例：不提供析构函数\n");
    queue_t* leaky_queue = queue_create(sizeof(stl_audio_pcm), NULL, NULL);
    
    stl_audio_pcm leaky_pcm = {
        .data = vector_create(sizeof(int16_t), 10, NULL, NULL),
        .time = time(NULL)
    };
    
    // 填充一些数据
    for(int i = 0; i < 10; i++) {
        int16_t sample = i;
        vector_push_back(leaky_pcm.data, &sample);
    }
    
    queue_push(leaky_queue, &leaky_pcm);
    printf("   创建了包含嵌套向量的音频帧，但没有析构函数\n");
    
    // 这里需要手动清理，否则会造成内存泄漏
    printf("   手动清理内存泄漏...\n");
    if (leaky_pcm.data) {
        vector_destroy(leaky_pcm.data);
    }
    queue_destroy(leaky_queue);
    
    // 正确示例：提供析构函数
    printf("2. 正确示例：提供析构函数\n");
    queue_t* proper_queue = queue_create(sizeof(stl_audio_pcm), NULL, stl_audio_pcm_destructor);
    
    stl_audio_pcm proper_pcm = {
        .data = vector_create(sizeof(int16_t), 10, NULL, NULL),
        .time = time(NULL)
    };
    
    // 填充一些数据
    for(int i = 0; i < 10; i++) {
        int16_t sample = i;
        vector_push_back(proper_pcm.data, &sample);
    }
    
    queue_push(proper_queue, &proper_pcm);
    printf("   创建了包含嵌套向量的音频帧，提供了析构函数\n");
    
    // 自动清理，析构函数会被调用
    queue_destroy(proper_queue);
    printf("   队列已销毁，析构函数自动释放了嵌套向量\n");
}

int main(){
    long long start_time, end_time;
    start_time = get_current_time_ms_high_precision();
    printf("修复版队列实验开始\n");

    // 初始化随机种子，确保每次运行都有不同的随机数
    srand((unsigned int)time(NULL));

    create_audio_queue();
    
    end_time = get_current_time_ms_high_precision();
    printf("队列创建完成，耗时：%dms\n", end_time - start_time);

    sort_audio_data();

    end_time = get_current_time_ms_high_precision();
    printf("队列实验结束，总耗时：%dms\n", end_time - start_time);
    
    // 演示内存泄漏对比
    demonstrate_memory_leak();

    return 0;
}