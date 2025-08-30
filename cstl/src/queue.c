/**
 * @file queue.c
 * @brief CSTL库的队列适配器实现
 * 
 * 该文件实现了CSTL库的队列适配器，基于双向链表容器实现，
 * 提供先进先出(FIFO)的数据结构操作。
 */

#include "cstl/queue.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**
 * @brief 创建队列适配器
 * 
 * @param element_size 元素大小
 * @param allocator 分配器指针，如果为NULL则使用默认分配器
 * @param destructor 析构函数指针，如果为NULL则使用默认释放方式
 * @return queue_t* 队列适配器指针，失败返回NULL
 */
queue_t* queue_create(size_t element_size, allocator_t* allocator, destructor_fn_t destructor)
{
    if (element_size == 0) {
        return NULL;
    }
    
    /* 分配队列结构体 */
    queue_t* queue = (queue_t*)malloc(sizeof(queue_t));
    if (queue == NULL) {
        return NULL;
    }
    
    /* 初始化队列 */
    error_code_t result = queue_init(queue, element_size, allocator);
    if (result != CSTL_OK) {
        free(queue);
        return NULL;
    }
    
    /* 设置析构函数 */
    if (queue->list != NULL) {
        queue->list->destructor = destructor;
    }
    
    return queue;
}

/**
 * @brief 基于现有双向链表容器创建队列适配器
 * 
 * @param list 双向链表容器指针
 * @param own_list 是否拥有底层容器
 * @return queue_t* 队列适配器指针，失败返回NULL
 */
queue_t* queue_create_from_list(list_t* list, int own_list)
{
    if (list == NULL) {
        return NULL;
    }
    
    /* 分配队列结构体 */
    queue_t* queue = (queue_t*)malloc(sizeof(queue_t));
    if (queue == NULL) {
        return NULL;
    }
    
    /* 初始化队列 */
    error_code_t result = queue_init_from_list(queue, list, own_list);
    if (result != CSTL_OK) {
        free(queue);
        return NULL;
    }
    
    return queue;
}

/**
 * @brief 销毁队列适配器
 * 
 * @param queue 队列适配器指针
 */
void queue_destroy(queue_t* queue)
{
    if (queue == NULL) {
        return;
    }
    
    /* 如果拥有底层容器，则销毁它 */
    if (queue->own_list && queue->list != NULL) {
        list_destroy(queue->list);
    }
    
    free(queue);
}

/**
 * @brief 初始化队列适配器
 * 
 * @param queue 队列适配器指针
 * @param element_size 元素大小
 * @param allocator 分配器指针，如果为NULL则使用默认分配器
 * @return error_code_t 错误码
 */
error_code_t queue_init(queue_t* queue, size_t element_size, allocator_t* allocator)
{
    if (queue == NULL || element_size == 0) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 创建底层双向链表容器 */
    list_t* list = list_create(element_size, allocator, NULL);
    if (list == NULL) {
        return CSTL_ERROR_OUT_OF_MEMORY;
    }
    
    /* 初始化队列 */
    queue->list = list;
    queue->own_list = 1;
    
    return CSTL_OK;
}

/**
 * @brief 基于现有双向链表容器初始化队列适配器
 * 
 * @param queue 队列适配器指针
 * @param list 双向链表容器指针
 * @param own_list 是否拥有底层容器
 * @return error_code_t 错误码
 */
error_code_t queue_init_from_list(queue_t* queue, list_t* list, int own_list)
{
    if (queue == NULL || list == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 初始化队列 */
    queue->list = list;
    queue->own_list = own_list;
    
    return CSTL_OK;
}

/**
 * @brief 获取队列适配器大小
 * 
 * @param queue 队列适配器指针
 * @return size_t 元素数量
 */
size_t queue_size(const queue_t* queue)
{
    if (queue == NULL || queue->list == NULL) {
        return 0;
    }
    
    return list_size(queue->list);
}

/**
 * @brief 检查队列适配器是否为空
 * 
 * @param queue 队列适配器指针
 * @return int 如果为空返回非零，否则返回零
 */
int queue_empty(const queue_t* queue)
{
    if (queue == NULL || queue->list == NULL) {
        return 1;
    }
    
    return list_empty(queue->list);
}

/**
 * @brief 入队操作
 * 
 * @param queue 队列适配器指针
 * @param element 要入队的元素指针
 * @return error_code_t 错误码
 */
error_code_t queue_push(queue_t* queue, const void* element)
{
    if (queue == NULL || queue->list == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return list_push_back(queue->list, element);
}

/**
 * @brief 出队操作
 * 
 * @param queue 队列适配器指针
 * @return error_code_t 错误码
 */
error_code_t queue_pop(queue_t* queue)
{
    if (queue == NULL || queue->list == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return list_pop_front(queue->list);
}

/**
 * @brief 获取队首元素
 * 
 * @param queue 队列适配器指针
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t queue_front(const queue_t* queue, void** element)
{
    if (queue == NULL || queue->list == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return list_front(queue->list, element);
}

/**
 * @brief 获取队尾元素
 * 
 * @param queue 队列适配器指针
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t queue_back(const queue_t* queue, void** element)
{
    if (queue == NULL || queue->list == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return list_back(queue->list, element);
}

/**
 * @brief 清空队列适配器
 * 
 * @param queue 队列适配器指针
 */
void queue_clear(queue_t* queue)
{
    if (queue == NULL || queue->list == NULL) {
        return;
    }
    
    list_clear(queue->list);
}

/**
 * @brief 启用线程安全
 * 
 * @param queue 队列适配器指针
 * @return error_code_t 错误码
 */
error_code_t queue_enable_thread_safety(queue_t* queue)
{
    if (queue == NULL || queue->list == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return list_enable_thread_safety(queue->list);
}

/**
 * @brief 禁用线程安全
 * 
 * @param queue 队列适配器指针
 * @return error_code_t 错误码
 */
error_code_t queue_disable_thread_safety(queue_t* queue)
{
    if (queue == NULL || queue->list == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return list_disable_thread_safety(queue->list);
}

/**
 * @brief 设置节点对象池
 * 
 * @param queue 队列适配器指针
 * @param node_pool 节点对象池指针
 * @return error_code_t 错误码
 */
error_code_t queue_set_node_pool(queue_t* queue, obj_pool_t* node_pool)
{
    if (queue == NULL || queue->list == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return list_set_node_pool(queue->list, node_pool);
}

/**
 * @brief 移除节点对象池
 * 
 * @param queue 队列适配器指针
 * @return error_code_t 错误码
 */
error_code_t queue_remove_node_pool(queue_t* queue)
{
    if (queue == NULL || queue->list == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return list_remove_node_pool(queue->list);
}

/**
 * @brief 获取底层双向链表容器
 * 
 * @param queue 队列适配器指针
 * @return list_t* 双向链表容器指针
 */
list_t* queue_get_list(queue_t* queue)
{
    if (queue == NULL) {
        return NULL;
    }
    
    return queue->list;
}
