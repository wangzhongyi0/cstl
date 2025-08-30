/**
 * @file vector.c
 * @brief CSTL库的向量容器实现
 * 
 * 该文件实现了CSTL库的向量容器，包括动态数组实现、
 * 核心操作和迭代器支持。
 */

#include "cstl/vector.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**
 * @brief 默认增长因子
 */
#define DEFAULT_GROWTH_FACTOR 2.0f


/**
 * @brief 向量迭代器结构体
 */
typedef struct vector_iterator_t {
    iterator_t base;         /**< 基础迭代器 */
    size_t index;             /**< 当前索引 */
} vector_iterator_t;

/**
 * @brief 向量迭代器next函数实现
 * 
 * @param iterator 迭代器指针
 * @return error_code_t 错误码
 */
static error_code_t vector_iterator_next(iterator_t* iterator)
{
    if (iterator == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    vector_iterator_t* vec_iter = (vector_iterator_t*)iterator;
    vector_t* vector = (vector_t*)iterator->container;
    
    if (vec_iter->index >= vector->size) {
        return CSTL_ERROR_ITERATOR_END;
    }
    
    vec_iter->index++;
    iterator->current = (char*)vector->data + vec_iter->index * vector->element_size;
    
    return CSTL_OK;
}

/**
 * @brief 向量迭代器prev函数实现
 * 
 * @param iterator 迭代器指针
 * @return error_code_t 错误码
 */
static error_code_t vector_iterator_prev(iterator_t* iterator)
{
    if (iterator == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    vector_iterator_t* vec_iter = (vector_iterator_t*)iterator;
    vector_t* vector = (vector_t*)iterator->container;
    
    if (vec_iter->index == 0) {
        return CSTL_ERROR_ITERATOR_END;
    }
    
    vec_iter->index--;
    iterator->current = (char*)vector->data + vec_iter->index * vector->element_size;
    
    return CSTL_OK;
}

/**
 * @brief 向量迭代器get函数实现
 * 
 * @param iterator 迭代器指针
 * @param data 输出参数，存储当前元素指针
 * @return error_code_t 错误码
 */
static error_code_t vector_iterator_get(iterator_t* iterator, void** data)
{
    if (iterator == NULL || data == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    vector_iterator_t* vec_iter = (vector_iterator_t*)iterator;
    vector_t* vector = (vector_t*)iterator->container;
    
    if (vec_iter->index >= vector->size) {
        return CSTL_ERROR_ITERATOR_END;
    }
    
    *data = iterator->current;
    return CSTL_OK;
}

/**
 * @brief 向量迭代器valid函数实现
 * 
 * @param iterator 迭代器指针
 * @return int 如果有效返回非零，否则返回零
 */
static int vector_iterator_valid(iterator_t* iterator)
{
    if (iterator == NULL) {
        return 0;
    }
    
    vector_iterator_t* vec_iter = (vector_iterator_t*)iterator;
    vector_t* vector = (vector_t*)iterator->container;
    
    return vec_iter->index < vector->size;
}

/**
 * @brief 向量迭代器destroy函数实现
 *
 * @param iterator 迭代器指针
 */
static void vector_iterator_destroy(iterator_t* iterator)
{
    /* 不需要特殊处理，迭代器将在iterator_destroy中释放 */
    (void)iterator;
}

/**
 * @brief 向量迭代器clone函数实现
 *
 * @param iterator 迭代器指针
 * @return iterator_t* 克隆的迭代器指针，失败返回NULL
 */
static iterator_t* vector_iterator_clone(iterator_t* iterator)
{
    if (iterator == NULL) {
        return NULL;
    }
    
    /* 分配新的向量迭代器结构体 */
    vector_iterator_t* vec_iter = (vector_iterator_t*)iterator;
    vector_iterator_t* new_vec_iter = (vector_iterator_t*)malloc(sizeof(vector_iterator_t));
    if (new_vec_iter == NULL) {
        return NULL;
    }
    
    /* 复制向量迭代器的所有字段 */
    new_vec_iter->base.container = vec_iter->base.container;
    new_vec_iter->base.current = vec_iter->base.current;
    new_vec_iter->base.direction = vec_iter->base.direction;
    new_vec_iter->base.element_size = vec_iter->base.element_size;
    new_vec_iter->base.next = vec_iter->base.next;
    new_vec_iter->base.prev = vec_iter->base.prev;
    new_vec_iter->base.get = vec_iter->base.get;
    new_vec_iter->base.valid = vec_iter->base.valid;
    new_vec_iter->base.destroy = vec_iter->base.destroy;
    new_vec_iter->base.clone = vec_iter->base.clone;
    new_vec_iter->index = vec_iter->index;  /* 关键：复制索引字段 */
    
    return (iterator_t*)new_vec_iter;
}

/**
 * @brief 锁定向量容器（如果启用线程安全）
 * 
 * @param vector 向量容器指针
 */
static void vector_lock(vector_t* vector)
{
    if (vector != NULL && vector->thread_safe) {
        mutex_lock(&vector->lock);
    }
}

/**
 * @brief 解锁向量容器（如果启用线程安全）
 * 
 * @param vector 向量容器指针
 */
static void vector_unlock(vector_t* vector)
{
    if (vector != NULL && vector->thread_safe) {
        mutex_unlock(&vector->lock);
    }
}

/**
 * @brief 确保向量容器有足够的容量
 * 
 * @param vector 向量容器指针
 * @param min_capacity 最小所需容量
 * @return error_code_t 错误码
 */
static error_code_t vector_ensure_capacity(vector_t* vector, size_t min_capacity)
{
    if (vector->capacity >= min_capacity) {
        return CSTL_OK;
    }
    
    /* 计算新容量 */
    size_t new_capacity = vector->capacity;
    if(new_capacity <= 128){
        new_capacity += 32;
    }else if(new_capacity <= 8*1024){
        new_capacity *= 2;
    }else if(new_capacity <= 128 * 1024){
        new_capacity += 1024*4;
    }else{
        new_capacity += 64*1024;
    }

    
    /* 重新分配内存 */
    void* new_data;
    if (vector->mem_pool != NULL) {
        /* 使用内存池分配 */
        new_data = vector->allocator->allocate(vector->allocator, new_capacity * vector->element_size);
        if (new_data == NULL) {
            return CSTL_ERROR_OUT_OF_MEMORY;
        }
        
        /* 复制数据 */
        memcpy(new_data, vector->data, vector->size * vector->element_size);
        
        /* 释放旧数据 */
        vector->allocator->deallocate(vector->allocator, vector->data);
    } else {
        /* 直接重新分配 */
        new_data = vector->allocator->reallocate(vector->allocator, vector->data, new_capacity * vector->element_size);
        if (new_data == NULL) {
            return CSTL_ERROR_OUT_OF_MEMORY;
        }
    }
    
    vector->data = new_data;
    vector->capacity = new_capacity;
    
    return CSTL_OK;
}

/**
 * @brief 创建向量容器
 * 
 * @param element_size 元素大小
 * @param initial_capacity 初始容量
 * @param allocator 分配器指针，如果为NULL则使用默认分配器
 * @param destructor 析构函数指针，如果为NULL则使用默认释放方式
 * @return vector_t* 向量容器指针，失败返回NULL
 */
vector_t* vector_create(size_t element_size, size_t initial_capacity, allocator_t* allocator, destructor_fn_t destructor)
{
    if (element_size == 0) {
        return NULL;
    }
    
    /* 分配向量结构体 */
    vector_t* vector = (vector_t*)malloc(sizeof(vector_t));
    if (vector == NULL) {
        return NULL;
    }
    
    /* 初始化向量 */
    error_code_t result = vector_init(vector, element_size, initial_capacity, allocator);
    if (result != CSTL_OK) {
        free(vector);
        return NULL;
    }
    
    /* 设置析构函数 */
    vector->destructor = destructor;
    
    return vector;
}

/**
 * @brief 销毁向量容器
 * 
 * @param vector 向量容器指针
 */
void vector_destroy(vector_t* vector)
{
    if (vector == NULL) {
        return;
    }
    
    vector_clear(vector);
    
    /* 销毁互斥锁 */
    if (vector->thread_safe) {
        mutex_destroy(&vector->lock);
    }
    
    free(vector);
}

/**
 * @brief 初始化向量容器
 * 
 * @param vector 向量容器指针
 * @param element_size 元素大小
 * @param initial_capacity 初始容量
 * @param allocator 分配器指针，如果为NULL则使用默认分配器
 * @return error_code_t 错误码
 */
error_code_t vector_init(vector_t* vector, size_t element_size, size_t initial_capacity, allocator_t* allocator)
{
    if (vector == NULL || element_size == 0) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 设置分配器 */
    if (allocator == NULL) {
        allocator = default_allocator();
    }
    
    /* 分配数据数组 */
    void* data = allocator->allocate(allocator, initial_capacity * element_size);
    if (data == NULL && initial_capacity > 0) {
        return CSTL_ERROR_OUT_OF_MEMORY;
    }
    
    /* 初始化向量 */
    vector->data = data;
    vector->size = 0;
    vector->capacity = initial_capacity;
    vector->element_size = element_size;
    vector->allocator = allocator;
    vector->mem_pool = NULL;
    vector->thread_safe = 0;
    vector->growth_factor = DEFAULT_GROWTH_FACTOR;
    
    return CSTL_OK;
}

/**
 * @brief 清空向量容器
 * 
 * @param vector 向量容器指针
 */
void vector_clear(vector_t* vector)
{
    if (vector == NULL) {
        return;
    }
    
    vector_lock(vector);
    
    /* 调用析构函数释放元素内存 */
    if (vector->destructor != NULL && vector->data != NULL) {
        size_t i;
        for (i = 0; i < vector->size; i++) {
            void* element = (char*)vector->data + i * vector->element_size;
            vector->destructor(element);
        }
    }
    
    /* 释放数据数组 */
    if (vector->data != NULL) {
        vector->allocator->deallocate(vector->allocator, vector->data);
        vector->data = NULL;
    }
    
    vector->size = 0;
    vector->capacity = 0;
    
    vector_unlock(vector);
}

/**
 * @brief 获取向量容器大小
 * 
 * @param vector 向量容器指针
 * @return size_t 元素数量
 */
size_t vector_size(const vector_t* vector)
{
    if (vector == NULL) {
        return 0;
    }
    
    return vector->size;
}

/**
 * @brief 获取向量容器容量
 * 
 * @param vector 向量容器指针
 * @return size_t 容量
 */
size_t vector_capacity(const vector_t* vector)
{
    if (vector == NULL) {
        return 0;
    }
    
    return vector->capacity;
}

/**
 * @brief 检查向量容器是否为空
 * 
 * @param vector 向量容器指针
 * @return int 如果为空返回非零，否则返回零
 */
int vector_empty(const vector_t* vector)
{
    if (vector == NULL) {
        return 1;
    }
    
    return vector->size == 0;
}

/**
 * @brief 设置向量容器的增长因子
 * 
 * @param vector 向量容器指针
 * @param growth_factor 增长因子，必须大于1.0
 * @return error_code_t 错误码
 */
error_code_t vector_set_growth_factor(vector_t* vector, float growth_factor)
{
    if (vector == NULL || growth_factor <= 1.0f) {
        return CSTL_ERROR_INVALID_ARGUMENT;
    }
    
    vector_lock(vector);
    vector->growth_factor = growth_factor;
    vector_unlock(vector);
    
    return CSTL_OK;
}

/**
 * @brief 预留向量容器容量
 * 
 * @param vector 向量容器指针
 * @param new_capacity 新容量
 * @return error_code_t 错误码
 */
error_code_t vector_reserve(vector_t* vector, size_t new_capacity)
{
    if (vector == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    vector_lock(vector);
    error_code_t result = vector_ensure_capacity(vector, new_capacity);
    vector_unlock(vector);
    
    return result;
}

/**
 * @brief 调整向量容器大小
 * 
 * @param vector 向量容器指针
 * @param new_size 新大小
 * @return error_code_t 错误码
 */
error_code_t vector_resize(vector_t* vector, size_t new_size)
{
    if (vector == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    vector_lock(vector);
    
    /* 确保有足够的容量 */
    error_code_t result = vector_ensure_capacity(vector, new_size);
    if (result != CSTL_OK) {
        vector_unlock(vector);
        return result;
    }
    
    /* 如果新大小小于当前大小，截断数据 */
    if (new_size < vector->size) {
        /* 调用析构函数释放被截断的元素内存 */
        if (vector->destructor != NULL) {
            size_t i;
            for (i = new_size; i < vector->size; i++) {
                void* element = (char*)vector->data + i * vector->element_size;
                vector->destructor(element);
            }
        }
        vector->size = new_size;
    } else {
        /* 如果新大小大于当前大小，填充零 */
        if (new_size > vector->size) {
            memset((char*)vector->data + vector->size * vector->element_size, 0, 
                   (new_size - vector->size) * vector->element_size);
            vector->size = new_size;
        }
    }
    
    vector_unlock(vector);
    return CSTL_OK;
}

/**
 * @brief 在向量容器末尾添加元素
 * 
 * @param vector 向量容器指针
 * @param element 要添加的元素指针
 * @return error_code_t 错误码
 */
error_code_t vector_push_back(vector_t* vector, const void* element)
{
    if (vector == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    vector_lock(vector);
    
    /* 确保有足够的容量 */
    error_code_t result = vector_ensure_capacity(vector, vector->size + 1);
    if (result != CSTL_OK) {
        vector_unlock(vector);
        return result;
    }
    
    /* 复制元素 */
    memcpy((char*)vector->data + vector->size * vector->element_size, element, vector->element_size);
    vector->size++;
    
    vector_unlock(vector);
    return CSTL_OK;
}

/**
 * @brief 移除向量容器末尾元素
 * 
 * @param vector 向量容器指针
 * @return error_code_t 错误码
 */
error_code_t vector_pop_back(vector_t* vector)
{
    if (vector == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    vector_lock(vector);
    
    if (vector->size == 0) {
        vector_unlock(vector);
        return CSTL_ERROR_CONTAINER_EMPTY;
    }
    
    /* 调用析构函数释放元素内存 */
    if (vector->destructor != NULL) {
        void* element = (char*)vector->data + (vector->size - 1) * vector->element_size;
        vector->destructor(element);
    }
    
    vector->size--;
    
    vector_unlock(vector);
    return CSTL_OK;
}

/**
 * @brief 在指定位置插入元素
 * 
 * @param vector 向量容器指针
 * @param index 插入位置索引
 * @param element 要插入的元素指针
 * @return error_code_t 错误码
 */
error_code_t vector_insert(vector_t* vector, size_t index, const void* element)
{
    if (vector == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    vector_lock(vector);
    
    /* 检查索引是否有效 */
    if (index > vector->size) {
        vector_unlock(vector);
        return CSTL_ERROR_INVALID_INDEX;
    }
    
    /* 确保有足够的容量 */
    error_code_t result = vector_ensure_capacity(vector, vector->size + 1);
    if (result != CSTL_OK) {
        vector_unlock(vector);
        return result;
    }
    
    /* 移动元素 */
    if (index < vector->size) {
        memmove((char*)vector->data + (index + 1) * vector->element_size,
                (char*)vector->data + index * vector->element_size,
                (vector->size - index) * vector->element_size);
    }
    
    /* 复制元素 */
    memcpy((char*)vector->data + index * vector->element_size, element, vector->element_size);
    vector->size++;
    
    vector_unlock(vector);
    return CSTL_OK;
}

/**
 * @brief 移除指定位置元素
 * 
 * @param vector 向量容器指针
 * @param index 移除位置索引
 * @return error_code_t 错误码
 */
error_code_t vector_erase(vector_t* vector, size_t index)
{
    if (vector == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    vector_lock(vector);
    
    /* 检查索引是否有效 */
    if (index >= vector->size) {
        vector_unlock(vector);
        return CSTL_ERROR_INVALID_INDEX;
    }
    
    /* 调用析构函数释放元素内存 */
    if (vector->destructor != NULL) {
        void* element = (char*)vector->data + index * vector->element_size;
        vector->destructor(element);
    }
    
    /* 移动元素 */
    if (index < vector->size - 1) {
        memmove((char*)vector->data + index * vector->element_size,
                (char*)vector->data + (index + 1) * vector->element_size,
                (vector->size - index - 1) * vector->element_size);
    }
    
    vector->size--;
    
    vector_unlock(vector);
    return CSTL_OK;
}

/**
 * @brief 获取指定位置元素
 * 
 * @param vector 向量容器指针
 * @param index 元素索引
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t vector_at(const vector_t* vector, size_t index, void** element)
{
    if (vector == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 检查索引是否有效 */
    if (index >= vector->size) {
        return CSTL_ERROR_INVALID_INDEX;
    }
    
    *element = (char*)vector->data + index * vector->element_size;
    return CSTL_OK;
}


/**
 * @brief 返回指定位置元素
 * 
 * @param vector 向量容器指针
 * @param index 元素索引
 * @return 指定索引的元素指针，失败返回NULL
 * 
 */
void* vector_get_by_index(const vector_t* vector, size_t index){
    void* element = NULL;
    error_code_t res = vector_at(vector, index, &element);
    
    return element;
}


/**
 * @brief 获取向量容器首元素
 * 
 * @param vector 向量容器指针
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t vector_front(const vector_t* vector, void** element)
{
    if (vector == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    if (vector->size == 0) {
        return CSTL_ERROR_CONTAINER_EMPTY;
    }
    
    *element = vector->data;
    return CSTL_OK;
}

/**
 * @brief 获取向量容器末尾元素
 * 
 * @param vector 向量容器指针
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t vector_back(const vector_t* vector, void** element)
{
    if (vector == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    if (vector->size == 0) {
        return CSTL_ERROR_CONTAINER_EMPTY;
    }
    
    *element = (char*)vector->data + (vector->size - 1) * vector->element_size;
    return CSTL_OK;
}

/**
 * @brief 设置指定位置元素
 * 
 * @param vector 向量容器指针
 * @param index 元素索引
 * @param element 要设置的元素指针
 * @return error_code_t 错误码
 */
error_code_t vector_set(vector_t* vector, size_t index, const void* element)
{
    if (vector == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    vector_lock(vector);
    
    /* 检查索引是否有效 */
    if (index >= vector->size) {
        vector_unlock(vector);
        return CSTL_ERROR_INVALID_INDEX;
    }
    
    /* 调用析构函数释放旧元素内存 */
    if (vector->destructor != NULL) {
        void* old_element = (char*)vector->data + index * vector->element_size;
        vector->destructor(old_element);
    }
    
    /* 设置元素 */
    memcpy((char*)vector->data + index * vector->element_size, element, vector->element_size);
    
    vector_unlock(vector);
    return CSTL_OK;
}

/**
 * @brief 启用线程安全
 * 
 * @param vector 向量容器指针
 * @return error_code_t 错误码
 */
error_code_t vector_enable_thread_safety(vector_t* vector)
{
    if (vector == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    vector_lock(vector);
    
    if (!vector->thread_safe) {
        error_code_t result = mutex_init(&vector->lock);
        if (result != CSTL_OK) {
            vector_unlock(vector);
            return result;
        }
        vector->thread_safe = 1;
    }
    
    vector_unlock(vector);
    return CSTL_OK;
}

/**
 * @brief 禁用线程安全
 * 
 * @param vector 向量容器指针
 * @return error_code_t 错误码
 */
error_code_t vector_disable_thread_safety(vector_t* vector)
{
    if (vector == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    vector_lock(vector);
    
    if (vector->thread_safe) {
        vector->thread_safe = 0;
        mutex_unlock(&vector->lock);
        mutex_destroy(&vector->lock);
        return CSTL_OK;
    }
    
    vector_unlock(vector);
    return CSTL_OK;
}

/**
 * @brief 设置内存池
 * 
 * @param vector 向量容器指针
 * @param mem_pool 内存池指针
 * @return error_code_t 错误码
 */
error_code_t vector_set_memory_pool(vector_t* vector, mem_pool_t* mem_pool)
{
    if (vector == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    vector_lock(vector);
    vector->mem_pool = mem_pool;
    vector_unlock(vector);
    
    return CSTL_OK;
}

/**
 * @brief 移除内存池
 * 
 * @param vector 向量容器指针
 * @return error_code_t 错误码
 */
error_code_t vector_remove_memory_pool(vector_t* vector)
{
    if (vector == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    vector_lock(vector);
    vector->mem_pool = NULL;
    vector_unlock(vector);
    
    return CSTL_OK;
}

/**
 * @brief 创建向量容器迭代器
 * 
 * @param vector 向量容器指针
 * @param direction 迭代方向
 * @return iterator_t* 迭代器指针，失败返回NULL
 */
iterator_t* vector_iterator_create(vector_t* vector, iter_direction_t direction)
{
    if (vector == NULL) {
        return NULL;
    }
    
    /* 分配向量迭代器结构体 */
    vector_iterator_t* vec_iter = (vector_iterator_t*)malloc(sizeof(vector_iterator_t));
    if (vec_iter == NULL) {
        return NULL;
    }
    
    /* 初始化迭代器 */
    vec_iter->base.container = vector;
    vec_iter->base.current = NULL;
    vec_iter->base.direction = direction;
    vec_iter->base.element_size = vector->element_size;
    vec_iter->base.next = vector_iterator_next;
    vec_iter->base.prev = vector_iterator_prev;
    vec_iter->base.get = vector_iterator_get;
    vec_iter->base.valid = vector_iterator_valid;
    vec_iter->base.destroy = vector_iterator_destroy;
    vec_iter->base.clone = vector_iterator_clone;
    
    /* 设置初始位置 */
    if (direction == ITER_DIR_FORWARD) {
        vec_iter->index = 0;
        if (vector->size > 0) {
            vec_iter->base.current = vector->data;
        }
    } else {
        vec_iter->index = vector->size > 0 ? vector->size - 1 : 0;
        if (vector->size > 0) {
            vec_iter->base.current = (char*)vector->data + (vector->size - 1) * vector->element_size;
        }
    }
    
    return (iterator_t*)vec_iter;
}

/**
 * @brief 获取向量容器起始迭代器
 *
 * @param vector 向量容器指针
 * @return iterator_t* 起始迭代器指针，失败返回NULL
 */
iterator_t* vector_begin(vector_t* vector)
{
    if (vector == NULL) {
        return NULL;
    }
    
    return vector_iterator_create(vector, ITER_DIR_FORWARD);
}

/**
 * @brief 获取向量容器结束迭代器
 *
 * @param vector 向量容器指针
 * @return iterator_t* 结束迭代器指针，失败返回NULL
 */
iterator_t* vector_end(vector_t* vector)
{
    if (vector == NULL) {
        return NULL;
    }
    
    /* 创建一个指向末尾的迭代器 */
    iterator_t* iterator = vector_iterator_create(vector, ITER_DIR_FORWARD);
    if (iterator == NULL) {
        return NULL;
    }
    
    /* 将迭代器移动到末尾 */
    vector_iterator_t* vec_iter = (vector_iterator_t*)iterator;
    vec_iter->index = vector->size;
    iterator->current = NULL;
    
    return iterator;
}
