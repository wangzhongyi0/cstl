/**
 * @file iterator.c
 * @brief CSTL库的迭代器框架实现
 * 
 * 该文件实现了CSTL库的迭代器框架，包括迭代器接口和通用操作。
 */

#include "cstl/iterator.h"
#include <stdlib.h>
#include <string.h>

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
) {
    if (container == NULL || element_size == 0 || 
        next_func == NULL || get_func == NULL || valid_func == NULL) {
        return NULL;
    }
    
    /* 分配迭代器结构体 */
    iterator_t* iterator = (iterator_t*)malloc(sizeof(iterator_t));
    if (iterator == NULL) {
        return NULL;
    }
    
    /* 初始化迭代器 */
    iterator->container = container;
    iterator->current = NULL;
    iterator->direction = direction;
    iterator->element_size = element_size;
    iterator->next = next_func;
    iterator->prev = prev_func;
    iterator->get = get_func;
    iterator->valid = valid_func;
    iterator->destroy = destroy_func;
    iterator->clone = clone_func;
    
    return iterator;
}

/**
 * @brief 销毁迭代器
 * 
 * @param iterator 迭代器指针
 */
void iterator_destroy(iterator_t* iterator)
{
    if (iterator == NULL) {
        return;
    }
    
    /* 调用自定义销毁函数（如果有） */
    if (iterator->destroy != NULL) {
        iterator->destroy(iterator);
    }
    
    /* 释放迭代器结构体 */
    free(iterator);
}

/**
 * @brief 移动到下一个元素
 * 
 * @param iterator 迭代器指针
 * @return error_code_t 错误码
 */
error_code_t iterator_next(iterator_t* iterator)
{
    if (iterator == NULL || iterator->next == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return iterator->next(iterator);
}

/**
 * @brief 移动到上一个元素
 * 
 * @param iterator 迭代器指针
 * @return error_code_t 错误码
 */
error_code_t iterator_prev(iterator_t* iterator)
{
    if (iterator == NULL || iterator->prev == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return iterator->prev(iterator);
}

/**
 * @brief 获取当前元素
 * 
 * @param iterator 迭代器指针
 * @param data 输出参数，存储当前元素指针
 * @return error_code_t 错误码
 */
error_code_t iterator_get(iterator_t* iterator, void** data)
{
    if (iterator == NULL || iterator->get == NULL || data == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return iterator->get(iterator, data);
}

/**
 * @brief 检查迭代器是否有效
 * 
 * @param iterator 迭代器指针
 * @return int 如果有效返回非零，否则返回零
 */
int iterator_valid(iterator_t* iterator)
{
    if (iterator == NULL || iterator->valid == NULL) {
        return 0;
    }
    
    return iterator->valid(iterator);
}

/**
 * @brief 克隆迭代器
 *
 * @param iterator 迭代器指针
 * @return iterator_t* 克隆的迭代器指针，失败返回NULL
 */
iterator_t* iterator_clone(iterator_t* iterator)
{
    if (iterator == NULL) {
        return NULL;
    }
    
    /* 如果迭代器有自己的克隆函数，使用它 */
    if (iterator->clone != NULL) {
        return iterator->clone(iterator);
    }
    
    /* 否则，使用默认的克隆逻辑 */
    iterator_t* new_iterator = (iterator_t*)malloc(sizeof(iterator_t));
    if (new_iterator == NULL) {
        return NULL;
    }
    
    /* 复制迭代器属性 */
    new_iterator->container = iterator->container;
    new_iterator->current = iterator->current;
    new_iterator->direction = iterator->direction;
    new_iterator->element_size = iterator->element_size;
    new_iterator->next = iterator->next;
    new_iterator->prev = iterator->prev;
    new_iterator->get = iterator->get;
    new_iterator->valid = iterator->valid;
    new_iterator->destroy = iterator->destroy;
    new_iterator->clone = iterator->clone;
    
    return new_iterator;
}

/**
 * @brief 检查两个迭代器是否相等
 *
 * @param iter1 第一个迭代器指针
 * @param iter2 第二个迭代器指针
 * @return int 如果相等返回非零，否则返回零
 */
int iterator_equal(iterator_t* iter1, iterator_t* iter2)
{
    if (iter1 == NULL || iter2 == NULL) {
        return 0;
    }
    
    /* 检查迭代器是否指向同一个容器和当前位置 */
    return (iter1->container == iter2->container &&
            iter1->current == iter2->current);
}