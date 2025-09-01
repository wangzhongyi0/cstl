/**
 * @file basic_test.c
 * @brief CSTL库的基本功能测试示例
 * 
 * 该文件展示了CSTL库的基本功能使用方法，包括：
 * - 向量容器的创建和使用
 * - 双向链表的创建和使用
 * - 栈适配器的创建和使用
 * - 队列适配器的创建和使用
 * - 算法模块的使用
 */

#include "cstl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <time.h>      // 用于 time()
#include "data_type.h"


// 创建数据结构保存音频数据
static vector_t* audio_data = NULL;
// 创建内存池
static mem_pool_t* mem_pool = NULL;


// 音频数据对象内存释放函数
static void* audio_data_deallocate(struct allocator_t* allocator, void* ptr);




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

// 打印音频数据帧
void print_audio_pcm(audio_pcm* pcm){
    if(pcm == NULL){
        printf("pcm is NULL\n");
        return;
    }
    printf("audio_pcm time: %d\n", pcm->time);

    vector_t* data = pcm->data;
    if(data == NULL || data->size == 0){
        printf("the data in pcm is null\n");
        return;
    }
    for(uint16_t i = 0; i < data->size; i++){
        if(i % 50 == 0){
            int16_t* data_i = NULL;
            vector_at(data, i, &data_i);
            printf("data[%d] = %d, ", i, *data_i);
        }
    }
    printf("\n");
}

// 获取一个音频数据帧并插入音频数据数组
void push_pcm(void){
    vector_t* pcm_data = vector_create(sizeof(int16_t), 0, NULL, NULL);
    for(uint16_t i = 0; i< 1024; i++){
        int16_t data = random_int64(0, 1024 * 8);
        vector_push_back(pcm_data, &data);
    }
    audio_pcm pcm = {
        .data = pcm_data,
        .time = 11223,
    };
    vector_push_back(audio_data, &pcm);
}



// 创建音频数据
void create_audio_data(uint32_t pcm_num){
    // 创建内存池
    audio_data = vector_create(sizeof(audio_pcm), 0, NULL, NULL);

    
    for(uint32_t i = 0; i < pcm_num; i++){
        push_pcm();
    }

}

// 排序算法比较函数
int compare_a_b(const void* a, const void* b){
    int16_t aa = (int16_t)*(int16_t*)a;
    int16_t bb = (int16_t)*(int16_t*)b;

    return (aa >= bb) ? ((aa > bb) ? 1 : 0) : -1;
}

// 执行一种排序算法
void one_sort_algo(audio_pcm* pcm, sort_algorithm_t sort_algo){
    // 获取首尾迭代器
    iterator_t* begin = vector_begin(pcm->data);
    iterator_t* end = vector_end(pcm->data);

    algo_sort(begin, end, compare_a_b, sort_algo);

    
    // 清理迭代器
    iterator_destroy(begin);
    iterator_destroy(end);
}



// 测试排序算法
void test_sort_algo(void){
    // 测试快速排序
    printf("test quick sort:\n");
    audio_pcm* pcm1 = vector_get_by_index(audio_data, 0);
    one_sort_algo(pcm1, SORT_QUICK);

    // 测试归并排序
    printf("\ntest merge sort:\n");
    audio_pcm* pcm2 = vector_get_by_index(audio_data, 1);
    one_sort_algo(pcm2, SORT_MERGE);

    // 测试堆排序
    printf("\ntest heap sort:\n");
    audio_pcm* pcm3 = vector_get_by_index(audio_data, 2);
    one_sort_algo(pcm3, SORT_HEAP);

    // 测试插入排序
    printf("\ntest insert sort:\n");
    audio_pcm* pcm4 = vector_get_by_index(audio_data, 3);
    one_sort_algo(pcm4, SORT_INSERT);

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
    // 初始化随机种子，确保每次运行都有不同的随机数
    srand((unsigned int)time(NULL));
    
    // printf("CSTL库基本功能测试示例\n");
    printf("version: %s\n", cstl_version());
    
    // 创建音频数据
    create_audio_data(1024*32);

    // // 测试排序算法
    // test_sort_algo();
    
    printf("\nall test end\n");
    
    return 0;
}


