/**
 * @file vector.h
 * @brief CSTL库的向量容器头文件
 * 
 * 该文件定义了CSTL库的向量容器，包括动态数组实现、
 * 核心操作和迭代器支持。
 */

#ifndef CSTL_VECTOR_H
#define CSTL_VECTOR_H

#include "cstl/common.h"
#include "cstl/iterator.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 向量容器结构体
 */
typedef struct vector_t {
    /**
     * @brief 数据数组指针
     */
    void* data;
    
    /**
     * @brief 元素数量
     */
    size_t size;
    
    /**
     * @brief 容量
     */
    size_t capacity;
    
    /**
     * @brief 元素大小
     */
    size_t element_size;
    
    /**
     * @brief 分配器指针
     */
    allocator_t* allocator;
    
    /**
     * @brief 内存池指针（可选）
     */
    mem_pool_t* mem_pool;

    /**
     * @brief 对象池指针（可选） 
     * 
     */
    obj_pool_t* obj_pool;

    /**
     * @brief 互斥锁（线程安全选项）
     */
    mutex_t lock;
    
    /**
     * @brief 是否启用线程安全
     */
    int thread_safe;
    
    /**
     * @brief 增长因子
     */
    float growth_factor;
    
    /**
     * @brief 析构函数指针，用于释放元素内存
     */
    destructor_fn_t destructor;
} vector_t;

/**
 * @brief 创建向量容器
 * 
 * @param element_size 元素大小
 * @param initial_capacity 初始容量
 * @param allocator 分配器指针，如果为NULL则使用默认分配器
 * @param destructor 析构函数指针，如果为NULL则使用默认释放方式
 * @return vector_t* 向量容器指针，失败返回NULL
 */
vector_t* vector_create(size_t element_size, size_t initial_capacity, allocator_t* allocator, destructor_fn_t destructor);

/**
 * @brief 销毁向量容器
 * 
 * @param vector 向量容器指针
 */
void vector_destroy(vector_t* vector);

/**
 * @brief 初始化向量容器
 * 
 * @param vector 向量容器指针
 * @param element_size 元素大小
 * @param initial_capacity 初始容量
 * @param allocator 分配器指针，如果为NULL则使用默认分配器
 * @return error_code_t 错误码
 */
error_code_t vector_init(vector_t* vector, size_t element_size, size_t initial_capacity, allocator_t* allocator);

/**
 * @brief 清空向量容器
 * 
 * @param vector 向量容器指针
 */
void vector_clear(vector_t* vector);

/**
 * @brief 获取向量容器大小
 * 
 * @param vector 向量容器指针
 * @return size_t 元素数量
 */
size_t vector_size(const vector_t* vector);

/**
 * @brief 获取向量容器容量
 * 
 * @param vector 向量容器指针
 * @return size_t 容量
 */
size_t vector_capacity(const vector_t* vector);

/**
 * @brief 检查向量容器是否为空
 * 
 * @param vector 向量容器指针
 * @return int 如果为空返回非零，否则返回零
 */
int vector_empty(const vector_t* vector);

/**
 * @brief 设置向量容器的增长因子
 * 
 * @param vector 向量容器指针
 * @param growth_factor 增长因子，必须大于1.0
 * @return error_code_t 错误码
 */
error_code_t vector_set_growth_factor(vector_t* vector, float growth_factor);

/**
 * @brief 预留向量容器容量
 * 
 * @param vector 向量容器指针
 * @param new_capacity 新容量
 * @return error_code_t 错误码
 */
error_code_t vector_reserve(vector_t* vector, size_t new_capacity);

/**
 * @brief 调整向量容器大小
 * 
 * @param vector 向量容器指针
 * @param new_size 新大小
 * @return error_code_t 错误码
 */
error_code_t vector_resize(vector_t* vector, size_t new_size);

/**
 * @brief 在向量容器末尾添加元素
 * 
 * @param vector 向量容器指针
 * @param element 要添加的元素指针
 * @return error_code_t 错误码
 */
error_code_t vector_push_back(vector_t* vector, const void* element);

/**
 * @brief 移除向量容器末尾元素
 * 
 * @param vector 向量容器指针
 * @return error_code_t 错误码
 */
error_code_t vector_pop_back(vector_t* vector);

/**
 * @brief 在指定位置插入元素
 * 
 * @param vector 向量容器指针
 * @param index 插入位置索引
 * @param element 要插入的元素指针
 * @return error_code_t 错误码
 */
error_code_t vector_insert(vector_t* vector, size_t index, const void* element);

/**
 * @brief 移除指定位置元素
 * 
 * @param vector 向量容器指针
 * @param index 移除位置索引
 * @return error_code_t 错误码
 */
error_code_t vector_erase(vector_t* vector, size_t index);

/**
 * @brief 获取指定位置元素
 * 
 * @param vector 向量容器指针
 * @param index 元素索引
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t vector_at(const vector_t* vector, size_t index, void** element);

/**
 * @brief 返回指定位置元素
 * 
 * @param vector 向量容器指针
 * @param index 元素索引
 * @return 指定索引的元素指针，失败返回NULL
 * 
 */
void* vector_get_by_index(const vector_t* vector, size_t index);


/**
 * @brief 获取向量容器首元素
 * 
 * @param vector 向量容器指针
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t vector_front(const vector_t* vector, void** element);

/**
 * @brief 获取向量容器末尾元素
 * 
 * @param vector 向量容器指针
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t vector_back(const vector_t* vector, void** element);

/**
 * @brief 设置指定位置元素
 * 
 * @param vector 向量容器指针
 * @param index 元素索引
 * @param element 要设置的元素指针
 * @return error_code_t 错误码
 */
error_code_t vector_set(vector_t* vector, size_t index, const void* element);

/**
 * @brief 启用线程安全
 * 
 * @param vector 向量容器指针
 * @return error_code_t 错误码
 */
error_code_t vector_enable_thread_safety(vector_t* vector);

/**
 * @brief 禁用线程安全
 * 
 * @param vector 向量容器指针
 * @return error_code_t 错误码
 */
error_code_t vector_disable_thread_safety(vector_t* vector);

/**
 * @brief 设置内存池
 * 
 * @param vector 向量容器指针
 * @param mem_pool 内存池指针
 * @return error_code_t 错误码
 */
error_code_t vector_set_memory_pool(vector_t* vector, mem_pool_t* mem_pool);

/**
 * @brief 移除内存池
 * 
 * @param vector 向量容器指针
 * @return error_code_t 错误码
 */
error_code_t vector_remove_memory_pool(vector_t* vector);

/**
 * @brief 创建向量容器迭代器
 * 
 * @param vector 向量容器指针
 * @param direction 迭代方向
 * @return iterator_t* 迭代器指针，失败返回NULL
 */
iterator_t* vector_iterator_create(vector_t* vector, iter_direction_t direction);

/**
 * @brief 获取向量容器起始迭代器
 *
 * @param vector 向量容器指针
 * @return iterator_t* 起始迭代器指针，失败返回NULL
 */
iterator_t* vector_begin(vector_t* vector);

/**
 * @brief 获取向量容器结束迭代器
 *
 * @param vector 向量容器指针
 * @return iterator_t* 结束迭代器指针，失败返回NULL
 */
iterator_t* vector_end(vector_t* vector);

#ifdef __cplusplus
}
#endif

#endif /* CSTL_VECTOR_H */
