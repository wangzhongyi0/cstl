/**
 * @file memory_management_example.c
 * @author CSTL Project Team
 * @brief 内存管理示例 - 演示复杂数据结构的正确内存管理
 * @version 1.0
 * @date 2025-09-01
 * 
 * 该文件演示了如何在CSTL库中正确管理复杂数据结构的内存，
 * 特别是当容器中存储包含动态内存指针的结构体时。
 */

#include "cstl.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 定义复杂数据结构示例

// 1. 包含动态分配字符串的结构体
typedef struct {
    char* name;
    int age;
    double score;
} person_t;

// 2. 包含嵌套容器的结构体（类似queue_test.c中的stl_audio_pcm）
typedef struct {
    vector_t* data;        // 嵌套的向量容器
    time_t timestamp;
    char* description;     // 动态分配的描述字符串
} data_frame_t;

// 3. 多层嵌套的复杂结构体
typedef struct {
    char* title;
    vector_t* tags;        // 字符串标签向量
    list_t* related_items; // 相关项目链表
    data_frame_t* frame;   // 嵌套的数据帧
} complex_item_t;

// 析构函数实现

// person_t的析构函数
void person_destructor(void* data) {
    if (data == NULL) return;
    
    person_t* person = (person_t*)data;
    
    // 释放动态分配的字符串
    if (person->name) {
        free(person->name);
        person->name = NULL;
    }
    
    printf("Person destructor called for: age=%d\n", person->age);
}

// data_frame_t的析构函数
void data_frame_destructor(void* data) {
    if (data == NULL) return;
    
    data_frame_t* frame = (data_frame_t*)data;
    
    // 释放嵌套的向量容器
    if (frame->data) {
        vector_destroy(frame->data);
        frame->data = NULL;
    }
    
    // 释放动态分配的描述字符串
    if (frame->description) {
        free(frame->description);
        frame->description = NULL;
    }
    
    printf("Data frame destructor called for timestamp: %ld\n", frame->timestamp);
}

// complex_item_t的析构函数（多层嵌套）
void complex_item_destructor(void* data) {
    if (data == NULL) return;
    
    complex_item_t* item = (complex_item_t*)data;
    
    // 释放标题字符串
    if (item->title) {
        free(item->title);
        item->title = NULL;
    }
    
    // 释放标签向量
    if (item->tags) {
        vector_destroy(item->tags);
        item->tags = NULL;
    }
    
    // 释放相关项目链表
    if (item->related_items) {
        list_destroy(item->related_items);
        item->related_items = NULL;
    }
    
    // 释放嵌套的数据帧
    if (item->frame) {
        data_frame_destructor(item->frame);
        free(item->frame);
        item->frame = NULL;
    }
    
    printf("Complex item destructor called\n");
}

// 辅助函数：创建person对象
person_t* create_person(const char* name, int age, double score) {
    person_t* person = malloc(sizeof(person_t));
    if (person == NULL) return NULL;
    
    person->name = strdup(name);  // 复制字符串
    person->age = age;
    person->score = score;
    
    return person;
}

// 辅助函数：创建data_frame对象
data_frame_t* create_data_frame(size_t data_size, const char* description) {
    data_frame_t* frame = malloc(sizeof(data_frame_t));
    if (frame == NULL) return NULL;
    
    frame->data = vector_create(sizeof(int), data_size, NULL, NULL);
    frame->description = strdup(description);
    time(&frame->timestamp);
    
    // 填充一些示例数据
    for (size_t i = 0; i < data_size; i++) {
        int value = (int)(rand() % 1000);
        vector_push_back(frame->data, &value);
    }
    
    return frame;
}

// 示例1：基本的析构函数使用
void example1_basic_destructor() {
    printf("\n=== 示例1：基本析构函数使用 ===\n");
    
    // 创建包含person_t的向量，提供析构函数
    vector_t* people = vector_create(sizeof(person_t), 0, NULL, person_destructor);
    
    // 添加一些人员数据
    person_t p1 = { .name = strdup("Alice"), .age = 25, .score = 85.5 };
    person_t p2 = { .name = strdup("Bob"), .age = 30, .score = 92.0 };
    person_t p3 = { .name = strdup("Charlie"), .age = 28, .score = 78.5 };
    
    vector_push_back(people, &p1);
    vector_push_back(people, &p2);
    vector_push_back(people, &p3);
    
    printf("向量中有 %zu 个人员\n", vector_size(people));
    
    // 销毁向量 - 会自动调用person_destructor释放所有人员的name字段
    vector_destroy(people);
    printf("向量已销毁，所有人员姓名内存已释放\n");
}

// 示例2：嵌套容器的内存管理
void example2_nested_containers() {
    printf("\n=== 示例2：嵌套容器内存管理 ===\n");
    
    // 创建包含data_frame_t的队列，提供析构函数
    queue_t* frame_queue = queue_create(sizeof(data_frame_t), NULL, data_frame_destructor);
    
    // 创建并添加几个数据帧
    for (int i = 0; i < 3; i++) {
        char desc[100];
        snprintf(desc, sizeof(desc), "数据帧%d", i);
        
        data_frame_t frame = {
            .data = vector_create(sizeof(int), 10, NULL, NULL),
            .description = strdup(desc),
            .timestamp = time(NULL) + i
        };
        
        // 填充数据
        for (int j = 0; j < 10; j++) {
            int value = i * 100 + j;
            vector_push_back(frame.data, &value);
        }
        
        queue_push(frame_queue, &frame);
    }
    
    printf("队列中有 %zu 个数据帧\n", queue_size(frame_queue));
    
    // 销毁队列 - 会自动调用data_frame_destructor释放所有嵌套资源
    queue_destroy(frame_queue);
    printf("队列已销毁，所有嵌套的向量和字符串已释放\n");
}

// 示例3：错误示例（不提供析构函数）
void example3_memory_leak_demonstration() {
    printf("\n=== 示例3：内存泄漏演示 ===\n");
    
    // 错误：不提供析构函数
    queue_t* leaky_queue = queue_create(sizeof(data_frame_t), NULL, NULL);
    
    data_frame_t frame = {
        .data = vector_create(sizeof(int), 5, NULL, NULL),
        .description = strdup("这个会造成内存泄漏"),
        .timestamp = time(NULL)
    };
    
    queue_push(leaky_queue, &frame);
    printf("创建了包含动态内存的数据帧，但没有提供析构函数\n");
    
    // 这里应该调用 queue_destroy(leaky_queue);
    // 但由于没有析构函数，frame.data和frame.description会内存泄漏
    // 所以我们需要手动清理
    if (frame.data) vector_destroy(frame.data);
    if (frame.description) free(frame.description);
    queue_destroy(leaky_queue);
    
    printf("手动清理了内存泄漏，但应该使用析构函数来自动处理\n");
}

// 示例4：完整的复杂结构管理
void example4_complex_structure() {
    printf("\n=== 示例4：复杂结构管理 ===\n");
    
    vector_t* complex_items = vector_create(sizeof(complex_item_t), 0, NULL, complex_item_destructor);
    
    // 创建一个复杂项目
    complex_item_t item;
    
    item.title = strdup("复杂项目示例");
    
    // 创建标签向量
    item.tags = vector_create(sizeof(char*), 3, NULL, string_destructor);
    const char* tags[] = {"重要", "高优先级", "测试"};
    for (int i = 0; i < 3; i++) {
        char* tag = strdup(tags[i]);
        vector_push_back(item.tags, &tag);
    }
    
    // 创建相关项目链表
    item.related_items = list_create(sizeof(char*), NULL, string_destructor);
    const char* related[] = {"项目A", "项目B"};
    for (int i = 0; i < 2; i++) {
        char* related_item = strdup(related[i]);
        list_push_back(item.related_items, &related_item);
    }
    
    // 创建嵌套的数据帧
    item.frame = create_data_frame(5, "嵌套数据帧");
    
    vector_push_back(complex_items, &item);
    
    printf("创建了复杂项目，包含多层嵌套结构\n");
    
    // 销毁向量 - complex_item_destructor会递归释放所有资源
    vector_destroy(complex_items);
    printf("复杂项目已销毁，所有嵌套资源已释放\n");
}

// 字符串析构函数（用于示例4）
void string_destructor(void* data) {
    if (data == NULL) return;
    char** str_ptr = (char**)data;
    if (*str_ptr) {
        free(*str_ptr);
        *str_ptr = NULL;
    }
}

int main() {
    printf("CSTL内存管理示例程序\n");
    printf("========================\n");
    
    srand((unsigned int)time(NULL));
    
    // 运行各种示例
    example1_basic_destructor();
    example2_nested_containers();
    example3_memory_leak_demonstration();
    example4_complex_structure();
    
    printf("\n所有示例完成\n");
    printf("关键要点：\n");
    printf("1. 为包含动态内存的结构体提供析构函数\n");
    printf("2. 析构函数应该递归释放所有嵌套资源\n");
    printf("3. 在创建容器时提供析构函数参数\n");
    printf("4. 使用vector_destroy/list_destroy等函数自动调用析构函数\n");
    
    return 0;
}