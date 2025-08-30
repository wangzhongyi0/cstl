/**
 * @file common.h
 * @brief CSTL库的基础架构模块头文件
 * 
 * 该文件定义了CSTL库的基础架构，包括错误码、通用类型定义、
 * 分配器接口和内存管理基础函数。
 */

#ifndef CSTL_COMMON_H
#define CSTL_COMMON_H

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

/* 线程安全支持 */
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <pthread.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief CSTL库版本信息
 */
#define CSTL_VERSION_MAJOR 1
#define CSTL_VERSION_MINOR 0
#define CSTL_VERSION_PATCH 0

/**
 * @brief 错误码枚举定义
 */
typedef enum {
    CSTL_OK = 0,                /**< 操作成功 */
    CSTL_ERROR_NULL_POINTER,     /**< 空指针错误 */
    CSTL_ERROR_OUT_OF_MEMORY,   /**< 内存不足 */
    CSTL_ERROR_INVALID_INDEX,   /**< 无效索引 */
    CSTL_ERROR_CONTAINER_EMPTY, /**< 容器为空 */
    CSTL_ERROR_CONTAINER_FULL,  /**< 容器已满 */
    CSTL_ERROR_ITERATOR_END,     /**< 迭代器已到达末尾 */
    CSTL_ERROR_NOT_FOUND,       /**< 元素未找到 */
    CSTL_ERROR_ALREADY_EXISTS,  /**< 元素已存在 */
    CSTL_ERROR_INVALID_ARGUMENT,/**< 无效参数 */
    CSTL_ERROR_UNKNOWN          /**< 未知错误 */
} error_code_t;

/**
 * @brief 通用类型定义
 */
typedef void* any_t;

/**
 * @brief 比较函数指针类型
 * 
 * @param a 第一个元素指针
 * @param b 第二个元素指针
 * @return int 比较结果：负数表示a < b，0表示a == b，正数表示a > b
 */
typedef int (*comparator_fn_t)(const void* a, const void* b);

/**
 * @brief 析构函数指针类型
 * 
 * @param data 要销毁的数据指针
 */
typedef void (*destructor_fn_t)(void* data);

/**
 * @brief 分配器接口结构体
 */
typedef struct allocator_t {
    /**
     * @brief 内存分配函数
     * 
     * @param allocator 分配器指针
     * @param size 要分配的内存大小
     * @return void* 分配的内存指针，失败返回NULL
     */
    void* (*allocate)(struct allocator_t* allocator, size_t size);
    
    /**
     * @brief 内存释放函数
     * 
     * @param allocator 分配器指针
     * @param ptr 要释放的内存指针
     */
    void (*deallocate)(struct allocator_t* allocator, void* ptr);
    
    /**
     * @brief 内存重新分配函数
     * 
     * @param allocator 分配器指针
     * @param ptr 原内存指针
     * @param size 新的内存大小
     * @return void* 重新分配的内存指针，失败返回NULL
     */
    void* (*reallocate)(struct allocator_t* allocator, void* ptr, size_t size);
    
    /**
     * @brief 分配器私有数据
     */
    void* user_data;
} allocator_t;

/**
 * @brief 默认分配器实例
 */
extern allocator_t* default_allocator(void);

/**
 * @brief 创建默认分配器
 * 
 * @return allocator_t* 分配器指针，失败返回NULL
 */
allocator_t* allocator_create_default(void);

/**
 * @brief 销毁分配器
 * 
 * @param allocator 分配器指针
 */
void allocator_destroy(allocator_t* allocator);

/**
 * @brief 获取错误码对应的字符串描述
 *
 * @param error 错误码
 * @return const char* 错误描述字符串
 */
const char* error_string(error_code_t error);

/**
 * @brief 互斥锁类型定义
 */
#if defined(_WIN32) || defined(_WIN64)
typedef CRITICAL_SECTION mutex_t;
#else
typedef pthread_mutex_t mutex_t;
#endif

/**
 * @brief 读写锁类型定义
 */
#if defined(_WIN32) || defined(_WIN64)
typedef struct {
    CRITICAL_SECTION lock;
    int readers;
    int writers;
    HANDLE write_event;
} rwlock_t;
#else
typedef pthread_rwlock_t rwlock_t;
#endif

/**
 * @brief 初始化互斥锁
 *
 * @param mutex 互斥锁指针
 * @return error_code_t 错误码
 */
error_code_t mutex_init(mutex_t* mutex);

/**
 * @brief 销毁互斥锁
 *
 * @param mutex 互斥锁指针
 * @return error_code_t 错误码
 */
error_code_t mutex_destroy(mutex_t* mutex);

/**
 * @brief 锁定互斥锁
 *
 * @param mutex 互斥锁指针
 * @return error_code_t 错误码
 */
error_code_t mutex_lock(mutex_t* mutex);

/**
 * @brief 解锁互斥锁
 *
 * @param mutex 互斥锁指针
 * @return error_code_t 错误码
 */
error_code_t mutex_unlock(mutex_t* mutex);

/**
 * @brief 初始化读写锁
 *
 * @param rwlock 读写锁指针
 * @return error_code_t 错误码
 */
error_code_t rwlock_init(rwlock_t* rwlock);

/**
 * @brief 销毁读写锁
 *
 * @param rwlock 读写锁指针
 * @return error_code_t 错误码
 */
error_code_t rwlock_destroy(rwlock_t* rwlock);

/**
 * @brief 获取读锁
 *
 * @param rwlock 读写锁指针
 * @return error_code_t 错误码
 */
error_code_t rwlock_rdlock(rwlock_t* rwlock);

/**
 * @brief 获取写锁
 *
 * @param rwlock 读写锁指针
 * @return error_code_t 错误码
 */
error_code_t rwlock_wrlock(rwlock_t* rwlock);

/**
 * @brief 释放读写锁
 *
 * @param rwlock 读写锁指针
 * @return error_code_t 错误码
 */
error_code_t rwlock_unlock(rwlock_t* rwlock);

/**
 * @brief 原子操作类型定义
 */
typedef struct {
    volatile long value;
#if defined(_WIN32) || defined(_WIN64)
    /* Windows平台不需要额外成员 */
#else
    pthread_mutex_t lock;
#endif
} atomic_t;

/**
 * @brief 初始化原子变量
 *
 * @param atomic 原子变量指针
 * @param value 初始值
 * @return error_code_t 错误码
 */
error_code_t atomic_init(atomic_t* atomic, long value);

/**
 * @brief 销毁原子变量
 *
 * @param atomic 原子变量指针
 * @return error_code_t 错误码
 */
error_code_t atomic_destroy(atomic_t* atomic);

/**
 * @brief 原子读取
 *
 * @param atomic 原子变量指针
 * @return long 读取的值
 */
long atomic_read(atomic_t* atomic);

/**
 * @brief 原子写入
 *
 * @param atomic 原子变量指针
 * @param value 要写入的值
 * @return error_code_t 错误码
 */
error_code_t atomic_write(atomic_t* atomic, long value);

/**
 * @brief 原子增加
 *
 * @param atomic 原子变量指针
 * @param value 增加的值
 * @return long 增加后的值
 */
long atomic_add(atomic_t* atomic, long value);

/**
 * @brief 原子比较并交换
 *
 * @param atomic 原子变量指针
 * @param old_value 期望的旧值
 * @param new_value 要设置的新值
 * @return int 如果交换成功返回非零，否则返回零
 */
int atomic_compare_and_swap(atomic_t* atomic, long old_value, long new_value);

/**
 * @brief 内存池块结构体
 */
typedef struct mem_pool_block_t {
    struct mem_pool_block_t* next;  /**< 下一个块指针 */
    size_t size;                    /**< 块大小 */
    char data[1];                   /**< 数据起始位置 */
} mem_pool_block_t;

/**
 * @brief 内存池结构体
 */
typedef struct mem_pool_t {
    mem_pool_block_t* free_list;    /**< 空闲块链表 */
    size_t block_size;              /**< 块大小 */
    size_t grow_size;               /**< 增长大小 */
    mutex_t lock;                   /**< 互斥锁 */
    atomic_t allocated_blocks;      /**< 已分配块计数 */
    atomic_t free_blocks;           /**< 空闲块计数 */
    allocator_t* allocator;         /**< 底层分配器 */
} mem_pool_t;

/**
 * @brief 对象池结构体
 */
typedef struct obj_pool_t {
    void** free_list;               /**< 空闲对象指针数组 */
    size_t obj_size;                /**< 对象大小 */
    size_t capacity;                /**< 池容量 */
    size_t count;                   /**< 当前对象数量 */
    size_t grow_size;               /**< 增长大小 */
    mutex_t lock;                   /**< 互斥锁 */
    atomic_t allocated_objects;     /**< 已分配对象计数 */
    atomic_t free_objects;          /**< 空闲对象计数 */
    allocator_t* allocator;         /**< 底层分配器 */
    destructor_fn_t destructor;     /**< 对象析构函数 */
} obj_pool_t;

/**
 * @brief 创建内存池
 *
 * @param block_size 块大小
 * @param grow_size 增长大小
 * @param allocator 底层分配器，如果为NULL则使用默认分配器
 * @return mem_pool_t* 内存池指针，失败返回NULL
 */
mem_pool_t* mem_pool_create(size_t block_size, size_t grow_size, allocator_t* allocator);

/**
 * @brief 销毁内存池
 *
 * @param pool 内存池指针
 */
void mem_pool_destroy(mem_pool_t* pool);

/**
 * @brief 从内存池分配内存
 *
 * @param pool 内存池指针
 * @return void* 分配的内存指针，失败返回NULL
 */
void* mem_pool_alloc(mem_pool_t* pool);

/**
 * @brief 释放内存回内存池
 *
 * @param pool 内存池指针
 * @param ptr 要释放的内存指针
 */
void mem_pool_free(mem_pool_t* pool, void* ptr);

/**
 * @brief 获取内存池统计信息
 *
 * @param pool 内存池指针
 * @param allocated_blocks 输出参数，已分配块数量
 * @param free_blocks 输出参数，空闲块数量
 * @return error_code_t 错误码
 */
error_code_t mem_pool_get_stats(mem_pool_t* pool, size_t* allocated_blocks, size_t* free_blocks);

/**
 * @brief 创建对象池
 *
 * @param obj_size 对象大小
 * @param initial_size 初始大小
 * @param grow_size 增长大小
 * @param destructor 对象析构函数
 * @param allocator 底层分配器，如果为NULL则使用默认分配器
 * @return obj_pool_t* 对象池指针，失败返回NULL
 */
obj_pool_t* obj_pool_create(size_t obj_size, size_t initial_size, size_t grow_size,
                           destructor_fn_t destructor, allocator_t* allocator);

/**
 * @brief 销毁对象池
 *
 * @param pool 对象池指针
 */
void obj_pool_destroy(obj_pool_t* pool);

/**
 * @brief 从对象池分配对象
 *
 * @param pool 对象池指针
 * @return void* 分配的对象指针，失败返回NULL
 */
void* obj_pool_alloc(obj_pool_t* pool);

/**
 * @brief 释放对象回对象池
 *
 * @param pool 对象池指针
 * @param obj 要释放的对象指针
 */
void obj_pool_free(obj_pool_t* pool, void* obj);

/**
 * @brief 获取对象池统计信息
 *
 * @param pool 对象池指针
 * @param allocated_objects 输出参数，已分配对象数量
 * @param free_objects 输出参数，空闲对象数量
 * @return error_code_t 错误码
 */
error_code_t obj_pool_get_stats(obj_pool_t* pool, size_t* allocated_objects, size_t* free_objects);

#ifdef __cplusplus
}
#endif

#endif /* CSTL_COMMON_H */
