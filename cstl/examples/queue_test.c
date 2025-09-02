/**
 * @file queue_test.c
 * @author wzy (1340130745@qq.com)
 * @brief 测试队列功能
 * @version 0.1
 * @date 2025-09-01
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "cstl.h"
#include "data_type.h"
#include "utils.h"


// 创建音频数据队列
static queue_t* audio_queue = NULL;
static no_stl_audio_pcm no_stl_audio_queue[512];    // 普通数组实现音频数据队列



// 创建数据队列
void create_audio_queue(void);

// 插入数据帧
void push_stl_audio_pcm_to_queue(void);

// 对数据进行排序与验证
void sort_audio_data();

// 验证排序后的数据
void verify_after_sort(stl_audio_pcm* pcm, uint16_t index);

// 验证排序后的数据
void verify_after_sort(stl_audio_pcm* pcm, uint16_t index);




int main(){

    long long start_time, end_time;
    start_time = get_current_time_ms_high_precision();
    printf("队列实验开始\n");

    // 初始化随机种子，确保每次运行都有不同的随机数
    srand((unsigned int)time(NULL));

    create_audio_queue();

    sort_audio_data();

    end_time = get_current_time_ms_high_precision();


    printf("队列实验结束，总耗时：%dms\n", end_time - start_time);

    return 0;
}


// 创建数据队列
void create_audio_queue(void){
    if(audio_queue != NULL){
        queue_destroy(audio_queue);
        audio_queue = NULL;
    }
    audio_queue = queue_create(sizeof(stl_audio_pcm), NULL, NULL);

    for(uint16_t i = 0; i < 410; i++){
        push_stl_audio_pcm_to_queue();
    }

}

// 插入数据帧
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



// 对数据进行排序与验证
void sort_audio_data(){
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
// 线性查找函数 - 最通用但效率较低
int find_value_linear(const no_stl_audio_pcm* pcm, int16_t target) {
    if (pcm == NULL) return -1;
    
    for (size_t i = 0; i < 1024; i++) {
        if (pcm->data[i] == target) {
            return (int)i;
        }
    }
    return -1;
}


// 验证排序后的数据
void verify_after_sort(stl_audio_pcm* pcm, uint16_t index){
    // 首先验证排序后的数据与原数据是否一致
    for(uint16_t i = 0; i < 1024; i++){
        int16_t sample = *(int16_t*)vector_get_by_index(pcm->data, i);
        int res = find_value_linear(no_stl_audio_queue[index].data, sample);
        if(res < 0 || res >= 1024){
            printf("排序前后数据不一致\n");
            return;
        }
    }

    // 然后验证排序是否正确
    for(uint16_t i = 0; i < 1024; i++){
        int16_t sample = *(int16_t*)vector_get_by_index(pcm->data, i);
        if(i == 0){
            continue;
        }
        int16_t pre_sample = *(int16_t*)vector_get_by_index(pcm->data, i - 1);
        if(sample < pre_sample){
            printf("排序算法有误\n");
            return;
        }
    }

}

