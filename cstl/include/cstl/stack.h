/**
 * @file stack.h
 * @brief CSTL库的栈适配器头文件
 * 
 * 该文件定义了CSTL库的栈适配器，基于向量容器实现，
 * 提供后进先出(LIFO)的数据结构操作。
 */

#ifndef CSTL_STACK_H
#define CSTL_STACK_H

#include "cstl/common.h"
#include "cstl/vector.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 栈适配器结构体
 */
typedef struct stack_t {
    /**
     * @brief 底层向量容器指针
     */
    vector_t* vector;
    
    /**
     * @brief 是否拥有底层容器
     */
    int own_vector;
} stack_t;

/**
 * @brief 创建栈适配器
 * 
 * @param element_size 元素大小
 * @param initial_capacity 初始容量
 * @param allocator 分配器指针，如果为NULL则使用默认分配器
 * @param destructor 析构函数指针，如果为NULL则使用默认释放方式
 * @return stack_t* 栈适配器指针，失败返回NULL
 */
stack_t* stack_create(size_t element_size, size_t initial_capacity, allocator_t* allocator, destructor_fn_t destructor);

/**
 * @brief 基于现有向量容器创建栈适配器
 * 
 * @param vector 向量容器指针
 * @param own_vector 是否拥有底层容器
 * @return stack_t* 栈适配器指针，失败返回NULL
 */
stack_t* stack_create_from_vector(vector_t* vector, int own_vector);

/**
 * @brief 销毁栈适配器
 * 
 * @param stack 栈适配器指针
 */
void stack_destroy(stack_t* stack);

/**
 * @brief 初始化栈适配器
 * 
 * @param stack 栈适配器指针
 * @param element_size 元素大小
 * @param initial_capacity 初始容量
 * @param allocator 分配器指针，如果为NULL则使用默认分配器
 * @return error_code_t 错误码
 */
error_code_t stack_init(stack_t* stack, size_t element_size, size_t initial_capacity, allocator_t* allocator);

/**
 * @brief 基于现有向量容器初始化栈适配器
 * 
 * @param stack 栈适配器指针
 * @param vector 向量容器指针
 * @param own_vector 是否拥有底层容器
 * @return error_code_t 错误码
 */
error_code_t stack_init_from_vector(stack_t* stack, vector_t* vector, int own_vector);

/**
 * @brief 获取栈适配器大小
 * 
 * @param stack 栈适配器指针
 * @return size_t 元素数量
 */
size_t stack_size(const stack_t* stack);

/**
 * @brief 检查栈适配器是否为空
 * 
 * @param stack 栈适配器指针
 * @return int 如果为空返回非零，否则返回零
 */
int stack_empty(const stack_t* stack);

/**
 * @brief 获取栈适配器容量
 * 
 * @param stack 栈适配器指针
 * @return size_t 容量
 */
size_t stack_capacity(const stack_t* stack);

/**
 * @brief 设置栈适配器的增长因子
 * 
 * @param stack 栈适配器指针
 * @param growth_factor 增长因子，必须大于1.0
 * @return error_code_t 错误码
 */
error_code_t stack_set_growth_factor(stack_t* stack, float growth_factor);

/**
 * @brief 预留栈适配器容量
 * 
 * @param stack 栈适配器指针
 * @param new_capacity 新容量
 * @return error_code_t 错误码
 */
error_code_t stack_reserve(stack_t* stack, size_t new_capacity);

/**
 * @brief 入栈操作
 * 
 * @param stack 栈适配器指针
 * @param element 要入栈的元素指针
 * @return error_code_t 错误码
 */
error_code_t stack_push(stack_t* stack, const void* element);

/**
 * @brief 出栈操作
 * 
 * @param stack 栈适配器指针
 * @return error_code_t 错误码
 */
error_code_t stack_pop(stack_t* stack);

/**
 * @brief 获取栈顶元素
 * 
 * @param stack 栈适配器指针
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t stack_top(const stack_t* stack, void** element);

/**
 * @brief 清空栈适配器
 * 
 * @param stack 栈适配器指针
 */
void stack_clear(stack_t* stack);

/**
 * @brief 启用线程安全
 * 
 * @param stack 栈适配器指针
 * @return error_code_t 错误码
 */
error_code_t stack_enable_thread_safety(stack_t* stack);

/**
 * @brief 禁用线程安全
 * 
 * @param stack 栈适配器指针
 * @return error_code_t 错误码
 */
error_code_t stack_disable_thread_safety(stack_t* stack);

/**
 * @brief 设置内存池
 * 
 * @param stack 栈适配器指针
 * @param mem_pool 内存池指针
 * @return error_code_t 错误码
 */
error_code_t stack_set_memory_pool(stack_t* stack, mem_pool_t* mem_pool);

/**
 * @brief 移除内存池
 * 
 * @param stack 栈适配器指针
 * @return error_code_t 错误码
 */
error_code_t stack_remove_memory_pool(stack_t* stack);

/**
 * @brief 获取底层向量容器
 * 
 * @param stack 栈适配器指针
 * @return vector_t* 向量容器指针
 */
vector_t* stack_get_vector(stack_t* stack);

#ifdef __cplusplus
}
#endif

#endif /* CSTL_STACK_H */
