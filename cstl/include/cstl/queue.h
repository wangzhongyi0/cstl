/**
 * @file queue.h
 * @brief CSTL库的队列适配器头文件
 * 
 * 该文件定义了CSTL库的队列适配器，基于双向链表容器实现，
 * 提供先进先出(FIFO)的数据结构操作。
 */

#ifndef CSTL_QUEUE_H
#define CSTL_QUEUE_H

#include "cstl/common.h"
#include "cstl/list.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 队列适配器结构体
 */
typedef struct queue_t {
    /**
     * @brief 底层双向链表容器指针
     */
    list_t* list;
    
    /**
     * @brief 是否拥有底层容器
     */
    int own_list;
} queue_t;

/**
 * @brief 创建队列适配器
 * 
 * @param element_size 元素大小
 * @param allocator 分配器指针，如果为NULL则使用默认分配器
 * @param destructor 析构函数指针，如果为NULL则使用默认释放方式
 * @return queue_t* 队列适配器指针，失败返回NULL
 */
queue_t* queue_create(size_t element_size, allocator_t* allocator, destructor_fn_t destructor);

/**
 * @brief 基于现有双向链表容器创建队列适配器
 * 
 * @param list 双向链表容器指针
 * @param own_list 是否拥有底层容器
 * @return queue_t* 队列适配器指针，失败返回NULL
 */
queue_t* queue_create_from_list(list_t* list, int own_list);

/**
 * @brief 销毁队列适配器
 * 
 * @param queue 队列适配器指针
 */
void queue_destroy(queue_t* queue);

/**
 * @brief 初始化队列适配器
 * 
 * @param queue 队列适配器指针
 * @param element_size 元素大小
 * @param allocator 分配器指针，如果为NULL则使用默认分配器
 * @return error_code_t 错误码
 */
error_code_t queue_init(queue_t* queue, size_t element_size, allocator_t* allocator);

/**
 * @brief 基于现有双向链表容器初始化队列适配器
 * 
 * @param queue 队列适配器指针
 * @param list 双向链表容器指针
 * @param own_list 是否拥有底层容器
 * @return error_code_t 错误码
 */
error_code_t queue_init_from_list(queue_t* queue, list_t* list, int own_list);

/**
 * @brief 获取队列适配器大小
 * 
 * @param queue 队列适配器指针
 * @return size_t 元素数量
 */
size_t queue_size(const queue_t* queue);

/**
 * @brief 检查队列适配器是否为空
 * 
 * @param queue 队列适配器指针
 * @return int 如果为空返回非零，否则返回零
 */
int queue_empty(const queue_t* queue);

/**
 * @brief 入队操作
 * 
 * @param queue 队列适配器指针
 * @param element 要入队的元素指针
 * @return error_code_t 错误码
 */
error_code_t queue_push(queue_t* queue, const void* element);

/**
 * @brief 出队操作
 * 
 * @param queue 队列适配器指针
 * @return error_code_t 错误码
 */
error_code_t queue_pop(queue_t* queue);

/**
 * @brief 获取队首元素
 * 
 * @param queue 队列适配器指针
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t queue_front(const queue_t* queue, void** element);

/**
 * @brief 获取队尾元素
 * 
 * @param queue 队列适配器指针
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t queue_back(const queue_t* queue, void** element);

/**
 * @brief 清空队列适配器
 * 
 * @param queue 队列适配器指针
 */
void queue_clear(queue_t* queue);

/**
 * @brief 启用线程安全
 * 
 * @param queue 队列适配器指针
 * @return error_code_t 错误码
 */
error_code_t queue_enable_thread_safety(queue_t* queue);

/**
 * @brief 禁用线程安全
 * 
 * @param queue 队列适配器指针
 * @return error_code_t 错误码
 */
error_code_t queue_disable_thread_safety(queue_t* queue);

/**
 * @brief 设置节点对象池
 * 
 * @param queue 队列适配器指针
 * @param node_pool 节点对象池指针
 * @return error_code_t 错误码
 */
error_code_t queue_set_node_pool(queue_t* queue, obj_pool_t* node_pool);

/**
 * @brief 移除节点对象池
 * 
 * @param queue 队列适配器指针
 * @return error_code_t 错误码
 */
error_code_t queue_remove_node_pool(queue_t* queue);

/**
 * @brief 获取底层双向链表容器
 * 
 * @param queue 队列适配器指针
 * @return list_t* 双向链表容器指针
 */
list_t* queue_get_list(queue_t* queue);

#ifdef __cplusplus
}
#endif

#endif /* CSTL_QUEUE_H */
