/**
 * @file iterator.h
 * @brief CSTL库的迭代器框架头文件
 * 
 * 该文件定义了CSTL库的迭代器框架，包括迭代器接口定义、
 * 通用操作和类型安全迭代器宏。
 */

#ifndef CSTL_ITERATOR_H
#define CSTL_ITERATOR_H

#include "cstl/common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 迭代器方向枚举
 */
typedef enum {
    ITER_DIR_FORWARD = 0,    /**< 前向迭代 */
    ITER_DIR_BACKWARD = 1    /**< 后向迭代 */
} iter_direction_t;

/**
 * @brief 迭代器接口结构体
 */
typedef struct iterator_t {
    /**
     * @brief 容器指针
     */
    void* container;
    
    /**
     * @brief 当前位置指针
     */
    void* current;
    
    /**
     * @brief 迭代器方向
     */
    iter_direction_t direction;
    
    /**
     * @brief 元素大小
     */
    size_t element_size;
    
    /**
     * @brief 移动到下一个元素
     * 
     * @param iterator 迭代器指针
     * @return error_code_t 错误码
     */
    error_code_t (*next)(struct iterator_t* iterator);
    
    /**
     * @brief 移动到上一个元素
     * 
     * @param iterator 迭代器指针
     * @return error_code_t 错误码
     */
    error_code_t (*prev)(struct iterator_t* iterator);
    
    /**
     * @brief 获取当前元素
     * 
     * @param iterator 迭代器指针
     * @param data 输出参数，存储当前元素指针
     * @return error_code_t 错误码
     */
    error_code_t (*get)(struct iterator_t* iterator, void** data);
    
    /**
     * @brief 检查迭代器是否有效
     * 
     * @param iterator 迭代器指针
     * @return int 如果有效返回非零，否则返回零
     */
    int (*valid)(struct iterator_t* iterator);
    
    /**
     * @brief 销毁迭代器
     *
     * @param iterator 迭代器指针
     */
    void (*destroy)(struct iterator_t* iterator);
    
    /**
     * @brief 克隆迭代器
     *
     * @param iterator 迭代器指针
     * @return iterator_t* 克隆的迭代器指针，失败返回NULL
     */
    struct iterator_t* (*clone)(struct iterator_t* iterator);
} iterator_t;

/**
 * @brief 创建迭代器
 *
 * @param container 容器指针
 * @param element_size 元素大小
 * @param direction 迭代方向
 * @param next_func next函数指针
 * @param prev_func prev函数指针
 * @param get_func get函数指针
 * @param valid_func valid函数指针
 * @param destroy_func destroy函数指针
 * @param clone_func clone函数指针
 * @return iterator_t* 迭代器指针，失败返回NULL
 */
iterator_t* iterator_create(
    void* container,
    size_t element_size,
    iter_direction_t direction,
    error_code_t (*next_func)(iterator_t* iterator),
    error_code_t (*prev_func)(iterator_t* iterator),
    error_code_t (*get_func)(iterator_t* iterator, void** data),
    int (*valid_func)(iterator_t* iterator),
    void (*destroy_func)(iterator_t* iterator),
    iterator_t* (*clone_func)(iterator_t* iterator)
);

/**
 * @brief 销毁迭代器
 * 
 * @param iterator 迭代器指针
 */
void iterator_destroy(iterator_t* iterator);

/**
 * @brief 移动到下一个元素
 * 
 * @param iterator 迭代器指针
 * @return error_code_t 错误码
 */
error_code_t iterator_next(iterator_t* iterator);

/**
 * @brief 移动到上一个元素
 * 
 * @param iterator 迭代器指针
 * @return error_code_t 错误码
 */
error_code_t iterator_prev(iterator_t* iterator);

/**
 * @brief 获取当前元素
 * 
 * @param iterator 迭代器指针
 * @param data 输出参数，存储当前元素指针
 * @return error_code_t 错误码
 */
error_code_t iterator_get(iterator_t* iterator, void** data);

/**
 * @brief 检查迭代器是否有效
 * 
 * @param iterator 迭代器指针
 * @return int 如果有效返回非零，否则返回零
 */
int iterator_valid(iterator_t* iterator);

/**
 * @brief 克隆迭代器
 *
 * @param iterator 迭代器指针
 * @return iterator_t* 克隆的迭代器指针，失败返回NULL
 */
iterator_t* iterator_clone(iterator_t* iterator);

/**
 * @brief 检查两个迭代器是否相等
 *
 * @param iter1 第一个迭代器指针
 * @param iter2 第二个迭代器指针
 * @return int 如果相等返回非零，否则返回零
 */
int iterator_equal(iterator_t* iter1, iterator_t* iter2);

/**
 * @brief 类型安全迭代器宏
 * 
 * 这些宏提供了类型安全的迭代器操作，避免手动类型转换。
 */

/**
 * @brief 创建类型安全迭代器
 * 
 * @param type 元素类型
 * @param container 容器指针
 * @param direction 迭代方向
 * @param next_func next函数指针
 * @param prev_func prev函数指针
 * @param get_func get函数指针
 * @param valid_func valid函数指针
 * @param destroy_func destroy函数指针
 * @return iterator_t* 迭代器指针，失败返回NULL
 */
#define ITERATOR_CREATE(type, container, direction, next_func, prev_func, get_func, valid_func, destroy_func) \
    iterator_create(container, sizeof(type), direction, next_func, prev_func, get_func, valid_func, destroy_func)

/**
 * @brief 获取类型安全的当前元素
 * 
 * @param type 元素类型
 * @param iterator 迭代器指针
 * @param data 输出参数，存储当前元素指针
 * @return error_code_t 错误码
 */
#define ITERATOR_GET(type, iterator, data) \
    iterator_get(iterator, (void**)(data))

/**
 * @brief 遍历容器的宏
 * 
 * @param type 元素类型
 * @param iterator 迭代器指针
 * @param item 循环变量，存储当前元素
 */
#define ITERATOR_FOREACH(type, iterator, item) \
    for (type* item = NULL; iterator_valid(iterator) && (ITERATOR_GET(type, iterator, (void**)&item) == CSTL_OK); iterator_next(iterator))

/**
 * @brief 反向遍历容器的宏
 * 
 * @param type 元素类型
 * @param iterator 迭代器指针
 * @param item 循环变量，存储当前元素
 */
#define ITERATOR_FOREACH_REVERSE(type, iterator, item) \
    for (type* item = NULL; iterator_valid(iterator) && (ITERATOR_GET(type, iterator, (void**)&item) == CSTL_OK); iterator_prev(iterator))

#ifdef __cplusplus
}
#endif

#endif /* CSTL_ITERATOR_H */