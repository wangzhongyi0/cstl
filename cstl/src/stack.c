/**
 * @file stack.c
 * @brief CSTL库的栈适配器实现
 * 
 * 该文件实现了CSTL库的栈适配器，基于向量容器实现，
 * 提供后进先出(LIFO)的数据结构操作。
 */

#include "cstl/stack.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**
 * @brief 创建栈适配器
 * 
 * @param element_size 元素大小
 * @param initial_capacity 初始容量
 * @param allocator 分配器指针，如果为NULL则使用默认分配器
 * @param destructor 析构函数指针，如果为NULL则使用默认释放方式
 * @return stack_t* 栈适配器指针，失败返回NULL
 */
stack_t* stack_create(size_t element_size, size_t initial_capacity, allocator_t* allocator, destructor_fn_t destructor)
{
    if (element_size == 0) {
        return NULL;
    }
    
    /* 分配栈结构体 */
    stack_t* stack = (stack_t*)malloc(sizeof(stack_t));
    if (stack == NULL) {
        return NULL;
    }
    
    /* 初始化栈 */
    error_code_t result = stack_init(stack, element_size, initial_capacity, allocator);
    if (result != CSTL_OK) {
        free(stack);
        return NULL;
    }
    
    /* 设置析构函数 */
    if (stack->vector != NULL) {
        stack->vector->destructor = destructor;
    }
    
    return stack;
}

/**
 * @brief 基于现有向量容器创建栈适配器
 * 
 * @param vector 向量容器指针
 * @param own_vector 是否拥有底层容器
 * @return stack_t* 栈适配器指针，失败返回NULL
 */
stack_t* stack_create_from_vector(vector_t* vector, int own_vector)
{
    if (vector == NULL) {
        return NULL;
    }
    
    /* 分配栈结构体 */
    stack_t* stack = (stack_t*)malloc(sizeof(stack_t));
    if (stack == NULL) {
        return NULL;
    }
    
    /* 初始化栈 */
    error_code_t result = stack_init_from_vector(stack, vector, own_vector);
    if (result != CSTL_OK) {
        free(stack);
        return NULL;
    }
    
    return stack;
}

/**
 * @brief 销毁栈适配器
 * 
 * @param stack 栈适配器指针
 */
void stack_destroy(stack_t* stack)
{
    if (stack == NULL) {
        return;
    }
    
    /* 如果拥有底层容器，则销毁它 */
    if (stack->own_vector && stack->vector != NULL) {
        vector_destroy(stack->vector);
    }
    
    free(stack);
}

/**
 * @brief 初始化栈适配器
 * 
 * @param stack 栈适配器指针
 * @param element_size 元素大小
 * @param initial_capacity 初始容量
 * @param allocator 分配器指针，如果为NULL则使用默认分配器
 * @return error_code_t 错误码
 */
error_code_t stack_init(stack_t* stack, size_t element_size, size_t initial_capacity, allocator_t* allocator)
{
    if (stack == NULL || element_size == 0) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 创建底层向量容器 */
    vector_t* vector = vector_create(element_size, initial_capacity, allocator, NULL);
    if (vector == NULL) {
        return CSTL_ERROR_OUT_OF_MEMORY;
    }
    
    /* 初始化栈 */
    stack->vector = vector;
    stack->own_vector = 1;
    
    return CSTL_OK;
}

/**
 * @brief 基于现有向量容器初始化栈适配器
 * 
 * @param stack 栈适配器指针
 * @param vector 向量容器指针
 * @param own_vector 是否拥有底层容器
 * @return error_code_t 错误码
 */
error_code_t stack_init_from_vector(stack_t* stack, vector_t* vector, int own_vector)
{
    if (stack == NULL || vector == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 初始化栈 */
    stack->vector = vector;
    stack->own_vector = own_vector;
    
    return CSTL_OK;
}

/**
 * @brief 获取栈适配器大小
 * 
 * @param stack 栈适配器指针
 * @return size_t 元素数量
 */
size_t stack_size(const stack_t* stack)
{
    if (stack == NULL || stack->vector == NULL) {
        return 0;
    }
    
    return vector_size(stack->vector);
}

/**
 * @brief 检查栈适配器是否为空
 * 
 * @param stack 栈适配器指针
 * @return int 如果为空返回非零，否则返回零
 */
int stack_empty(const stack_t* stack)
{
    if (stack == NULL || stack->vector == NULL) {
        return 1;
    }
    
    return vector_empty(stack->vector);
}

/**
 * @brief 获取栈适配器容量
 * 
 * @param stack 栈适配器指针
 * @return size_t 容量
 */
size_t stack_capacity(const stack_t* stack)
{
    if (stack == NULL || stack->vector == NULL) {
        return 0;
    }
    
    return vector_capacity(stack->vector);
}

/**
 * @brief 设置栈适配器的增长因子
 * 
 * @param stack 栈适配器指针
 * @param growth_factor 增长因子，必须大于1.0
 * @return error_code_t 错误码
 */
error_code_t stack_set_growth_factor(stack_t* stack, float growth_factor)
{
    if (stack == NULL || stack->vector == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return vector_set_growth_factor(stack->vector, growth_factor);
}

/**
 * @brief 预留栈适配器容量
 * 
 * @param stack 栈适配器指针
 * @param new_capacity 新容量
 * @return error_code_t 错误码
 */
error_code_t stack_reserve(stack_t* stack, size_t new_capacity)
{
    if (stack == NULL || stack->vector == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return vector_reserve(stack->vector, new_capacity);
}

/**
 * @brief 入栈操作
 * 
 * @param stack 栈适配器指针
 * @param element 要入栈的元素指针
 * @return error_code_t 错误码
 */
error_code_t stack_push(stack_t* stack, const void* element)
{
    if (stack == NULL || stack->vector == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return vector_push_back(stack->vector, element);
}

/**
 * @brief 出栈操作
 * 
 * @param stack 栈适配器指针
 * @return error_code_t 错误码
 */
error_code_t stack_pop(stack_t* stack)
{
    if (stack == NULL || stack->vector == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return vector_pop_back(stack->vector);
}

/**
 * @brief 获取栈顶元素
 * 
 * @param stack 栈适配器指针
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t stack_top(const stack_t* stack, void** element)
{
    if (stack == NULL || stack->vector == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return vector_back(stack->vector, element);
}

/**
 * @brief 清空栈适配器
 * 
 * @param stack 栈适配器指针
 */
void stack_clear(stack_t* stack)
{
    if (stack == NULL || stack->vector == NULL) {
        return;
    }
    
    vector_clear(stack->vector);
}

/**
 * @brief 启用线程安全
 * 
 * @param stack 栈适配器指针
 * @return error_code_t 错误码
 */
error_code_t stack_enable_thread_safety(stack_t* stack)
{
    if (stack == NULL || stack->vector == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return vector_enable_thread_safety(stack->vector);
}

/**
 * @brief 禁用线程安全
 * 
 * @param stack 栈适配器指针
 * @return error_code_t 错误码
 */
error_code_t stack_disable_thread_safety(stack_t* stack)
{
    if (stack == NULL || stack->vector == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return vector_disable_thread_safety(stack->vector);
}

/**
 * @brief 设置内存池
 * 
 * @param stack 栈适配器指针
 * @param mem_pool 内存池指针
 * @return error_code_t 错误码
 */
error_code_t stack_set_memory_pool(stack_t* stack, mem_pool_t* mem_pool)
{
    if (stack == NULL || stack->vector == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return vector_set_memory_pool(stack->vector, mem_pool);
}

/**
 * @brief 移除内存池
 * 
 * @param stack 栈适配器指针
 * @return error_code_t 错误码
 */
error_code_t stack_remove_memory_pool(stack_t* stack)
{
    if (stack == NULL || stack->vector == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    return vector_remove_memory_pool(stack->vector);
}

/**
 * @brief 获取底层向量容器
 * 
 * @param stack 栈适配器指针
 * @return vector_t* 向量容器指针
 */
vector_t* stack_get_vector(stack_t* stack)
{
    if (stack == NULL) {
        return NULL;
    }
    
    return stack->vector;
}
