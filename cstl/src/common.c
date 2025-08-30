/**
 * @file common.c
 * @brief CSTL库的基础架构模块实现
 * 
 * 该文件实现了CSTL库的基础架构，包括错误码处理、
 * 分配器接口和内存管理基础函数。
 */

#include "cstl/common.h"
#include <stdlib.h>
#include <string.h>

/* 线程安全支持 */
#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

/**
 * @brief 默认分配器实例
 */
static allocator_t g_default_allocator = {0};

/**
 * @brief 默认内存分配函数
 * 
 * @param allocator 分配器指针
 * @param size 要分配的内存大小
 * @return void* 分配的内存指针，失败返回NULL
 */
static void* default_allocate(allocator_t* allocator, size_t size)
{
    (void)allocator; /* 避免未使用参数警告 */
    return malloc(size);
}

/**
 * @brief 默认内存释放函数
 * 
 * @param allocator 分配器指针
 * @param ptr 要释放的内存指针
 */
static void default_deallocate(allocator_t* allocator, void* ptr)
{
    (void)allocator; /* 避免未使用参数警告 */
    free(ptr);
}

/**
 * @brief 默认内存重新分配函数
 * 
 * @param allocator 分配器指针
 * @param ptr 原内存指针
 * @param size 新的内存大小
 * @return void* 重新分配的内存指针，失败返回NULL
 */
static void* default_reallocate(allocator_t* allocator, void* ptr, size_t size)
{
    (void)allocator; /* 避免未使用参数警告 */
    return realloc(ptr, size);
}

/**
 * @brief 获取默认分配器实例
 * 
 * @return allocator_t* 默认分配器指针
 */
allocator_t* default_allocator(void)
{
    if (g_default_allocator.allocate == NULL) {
        g_default_allocator.allocate = default_allocate;
        g_default_allocator.deallocate = default_deallocate;
        g_default_allocator.reallocate = default_reallocate;
        g_default_allocator.user_data = NULL;
    }
    return &g_default_allocator;
}

/**
 * @brief 创建默认分配器
 * 
 * @return allocator_t* 分配器指针，失败返回NULL
 */
allocator_t* allocator_create_default(void)
{
    allocator_t* allocator = (allocator_t*)malloc(sizeof(allocator_t));
    if (allocator == NULL) {
        return NULL;
    }
    
    allocator->allocate = default_allocate;
    allocator->deallocate = default_deallocate;
    allocator->reallocate = default_reallocate;
    allocator->user_data = NULL;
    
    return allocator;
}

/**
 * @brief 销毁分配器
 * 
 * @param allocator 分配器指针
 */
void allocator_destroy(allocator_t* allocator)
{
    if (allocator != NULL) {
        free(allocator);
    }
}

/**
 * @brief 错误码描述字符串数组
 */
static const char* g_error_strings[] = {
    "\xe6\x93\x8d\xe4\xbd\x9c\xe6\x88\x90\xe5\x8a\x9f",                    /* CSTL_OK */
    "\xe7\xa9\xba\xe6\x8c\x87\xe9\x92\x88\xe9\x94\x99\xe8\xaf\xaf",                  /* CSTL_ERROR_NULL_POINTER */
    "\xe5\x86\x85\xe5\xad\x98\xe4\xb8\x8d\xe8\xb6\xb3",                    /* CSTL_ERROR_OUT_OF_MEMORY */
    "\xe6\x97\xa0\xe6\x95\x88\xe7\xb4\xa2\xe5\xbc\x95",                    /* CSTL_ERROR_INVALID_INDEX */
    "\xe5\xae\xb9\xe5\x99\xa8\xe4\xb8\xba\xe7\xa9\xba",                    /* CSTL_ERROR_CONTAINER_EMPTY */
    "\xe5\xae\xb9\xe5\x99\xa8\xe5\xb7\xb2\xe6\xbb\xa1",                    /* CSTL_ERROR_CONTAINER_FULL */
    "\xe8\xbf\xad\xe4\xbb\xa3\xe5\x99\xa8\xe5\xb7\xb2\xe5\x88\xb0\xe8\xbe\xbe\xe6\x9c\xab\xe5\xb0\xbe",            /* CSTL_ERROR_ITERATOR_END */
    "\xe5\x85\x83\xe7\xb4\xa0\xe6\x9c\xaa\xe6\x89\xbe\xe5\x88\xb0",                  /* CSTL_ERROR_NOT_FOUND */
    "\xe5\x85\x83\xe7\xb4\xa0\xe5\xb7\xb2\xe5\xad\x98\xe5\x9c\xa8",                  /* CSTL_ERROR_ALREADY_EXISTS */
    "\xe6\x97\xa0\xe6\x95\x88\xe5\x8f\x82\xe6\x95\xb0",                    /* CSTL_ERROR_INVALID_ARGUMENT */
    "\xe6\x9c\xaa\xe7\x9f\xa5\xe9\x94\x99\xe8\xaf\xaf"                     /* CSTL_ERROR_UNKNOWN */
};

/**
 * @brief 获取错误码对应的字符串描述
 * 
 * @param error 错误码
 * @return const char* 错误描述字符串
 */
const char* error_string(error_code_t error)
{
    if (error >= 0 && error < sizeof(g_error_strings) / sizeof(g_error_strings[0])) {
        return g_error_strings[error];
    }
    return g_error_strings[CSTL_ERROR_UNKNOWN];
}

/* 线程安全支持函数实现 */

/**
 * @brief 初始化互斥锁
 *
 * @param mutex 互斥锁指针
 * @return error_code_t 错误码
 */
error_code_t mutex_init(mutex_t* mutex)
{
    if (mutex == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
#if defined(_WIN32) || defined(_WIN64)
    InitializeCriticalSection(mutex);
    return CSTL_OK;
#else
    int result = pthread_mutex_init(mutex, NULL);
    return (result == 0) ? CSTL_OK : CSTL_ERROR_UNKNOWN;
#endif
}

/**
 * @brief 销毁互斥锁
 *
 * @param mutex 互斥锁指针
 * @return error_code_t 错误码
 */
error_code_t mutex_destroy(mutex_t* mutex)
{
    if (mutex == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
#if defined(_WIN32) || defined(_WIN64)
    DeleteCriticalSection(mutex);
    return CSTL_OK;
#else
    int result = pthread_mutex_destroy(mutex);
    return (result == 0) ? CSTL_OK : CSTL_ERROR_UNKNOWN;
#endif
}

/**
 * @brief 锁定互斥锁
 *
 * @param mutex 互斥锁指针
 * @return error_code_t 错误码
 */
error_code_t mutex_lock(mutex_t* mutex)
{
    if (mutex == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
#if defined(_WIN32) || defined(_WIN64)
    EnterCriticalSection(mutex);
    return CSTL_OK;
#else
    int result = pthread_mutex_lock(mutex);
    return (result == 0) ? CSTL_OK : CSTL_ERROR_UNKNOWN;
#endif
}

/**
 * @brief 解锁互斥锁
 *
 * @param mutex 互斥锁指针
 * @return error_code_t 错误码
 */
error_code_t mutex_unlock(mutex_t* mutex)
{
    if (mutex == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
#if defined(_WIN32) || defined(_WIN64)
    LeaveCriticalSection(mutex);
    return CSTL_OK;
#else
    int result = pthread_mutex_unlock(mutex);
    return (result == 0) ? CSTL_OK : CSTL_ERROR_UNKNOWN;
#endif
}

/**
 * @brief 初始化读写锁
 *
 * @param rwlock 读写锁指针
 * @return error_code_t 错误码
 */
error_code_t rwlock_init(rwlock_t* rwlock)
{
    if (rwlock == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
#if defined(_WIN32) || defined(_WIN64)
    InitializeCriticalSection(&rwlock->lock);
    rwlock->readers = 0;
    rwlock->writers = 0;
    rwlock->write_event = CreateEvent(NULL, TRUE, TRUE, NULL);
    if (rwlock->write_event == NULL) {
        DeleteCriticalSection(&rwlock->lock);
        return CSTL_ERROR_OUT_OF_MEMORY;
    }
    return CSTL_OK;
#else
    int result = pthread_rwlock_init(rwlock, NULL);
    return (result == 0) ? CSTL_OK : CSTL_ERROR_UNKNOWN;
#endif
}

/**
 * @brief 销毁读写锁
 *
 * @param rwlock 读写锁指针
 * @return error_code_t 错误码
 */
error_code_t rwlock_destroy(rwlock_t* rwlock)
{
    if (rwlock == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
#if defined(_WIN32) || defined(_WIN64)
    DeleteCriticalSection(&rwlock->lock);
    CloseHandle(rwlock->write_event);
    return CSTL_OK;
#else
    int result = pthread_rwlock_destroy(rwlock);
    return (result == 0) ? CSTL_OK : CSTL_ERROR_UNKNOWN;
#endif
}

/**
 * @brief 获取读锁
 *
 * @param rwlock 读写锁指针
 * @return error_code_t 错误码
 */
error_code_t rwlock_rdlock(rwlock_t* rwlock)
{
    if (rwlock == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
#if defined(_WIN32) || defined(_WIN64)
    EnterCriticalSection(&rwlock->lock);
    while (rwlock->writers > 0) {
        LeaveCriticalSection(&rwlock->lock);
        WaitForSingleObject(rwlock->write_event, INFINITE);
        EnterCriticalSection(&rwlock->lock);
    }
    rwlock->readers++;
    LeaveCriticalSection(&rwlock->lock);
    return CSTL_OK;
#else
    int result = pthread_rwlock_rdlock(rwlock);
    return (result == 0) ? CSTL_OK : CSTL_ERROR_UNKNOWN;
#endif
}

/**
 * @brief 获取写锁
 *
 * @param rwlock 读写锁指针
 * @return error_code_t 错误码
 */
error_code_t rwlock_wrlock(rwlock_t* rwlock)
{
    if (rwlock == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
#if defined(_WIN32) || defined(_WIN64)
    EnterCriticalSection(&rwlock->lock);
    rwlock->writers++;
    while (rwlock->readers > 0 || rwlock->writers > 1) {
        LeaveCriticalSection(&rwlock->lock);
        WaitForSingleObject(rwlock->write_event, INFINITE);
        EnterCriticalSection(&rwlock->lock);
    }
    ResetEvent(rwlock->write_event);
    LeaveCriticalSection(&rwlock->lock);
    return CSTL_OK;
#else
    int result = pthread_rwlock_wrlock(rwlock);
    return (result == 0) ? CSTL_OK : CSTL_ERROR_UNKNOWN;
#endif
}

/**
 * @brief 释放读写锁
 *
 * @param rwlock 读写锁指针
 * @return error_code_t 错误码
 */
error_code_t rwlock_unlock(rwlock_t* rwlock)
{
    if (rwlock == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
#if defined(_WIN32) || defined(_WIN64)
    EnterCriticalSection(&rwlock->lock);
    if (rwlock->writers > 0) {
        rwlock->writers--;
        if (rwlock->writers == 0) {
            SetEvent(rwlock->write_event);
        }
    } else {
        rwlock->readers--;
        if (rwlock->readers == 0) {
            SetEvent(rwlock->write_event);
        }
    }
    LeaveCriticalSection(&rwlock->lock);
    return CSTL_OK;
#else
    int result = pthread_rwlock_unlock(rwlock);
    return (result == 0) ? CSTL_OK : CSTL_ERROR_UNKNOWN;
#endif
}

/**
 * @brief 初始化原子变量
 *
 * @param atomic 原子变量指针
 * @param value 初始值
 * @return error_code_t 错误码
 */
error_code_t atomic_init(atomic_t* atomic, long value)
{
    if (atomic == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    atomic->value = value;
    
#if !defined(_WIN32) && !defined(_WIN64)
    int result = pthread_mutex_init(&atomic->lock, NULL);
    if (result != 0) {
        return CSTL_ERROR_UNKNOWN;
    }
#endif
    
    return CSTL_OK;
}

/**
 * @brief 销毁原子变量
 *
 * @param atomic 原子变量指针
 * @return error_code_t 错误码
 */
error_code_t atomic_destroy(atomic_t* atomic)
{
    if (atomic == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
#if !defined(_WIN32) && !defined(_WIN64)
    pthread_mutex_destroy(&atomic->lock);
#endif
    
    return CSTL_OK;
}

/**
 * @brief 原子读取
 *
 * @param atomic 原子变量指针
 * @return long 读取的值
 */
long atomic_read(atomic_t* atomic)
{
    if (atomic == NULL) {
        return 0;
    }
    
#if defined(_WIN32) || defined(_WIN64)
    return InterlockedExchangeAdd(&atomic->value, 0);
#else
    long value;
    pthread_mutex_lock(&atomic->lock);
    value = atomic->value;
    pthread_mutex_unlock(&atomic->lock);
    return value;
#endif
}

/**
 * @brief 原子写入
 *
 * @param atomic 原子变量指针
 * @param value 要写入的值
 * @return error_code_t 错误码
 */
error_code_t atomic_write(atomic_t* atomic, long value)
{
    if (atomic == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
#if defined(_WIN32) || defined(_WIN64)
    InterlockedExchange(&atomic->value, value);
#else
    pthread_mutex_lock(&atomic->lock);
    atomic->value = value;
    pthread_mutex_unlock(&atomic->lock);
#endif
    
    return CSTL_OK;
}

/**
 * @brief 原子增加
 *
 * @param atomic 原子变量指针
 * @param value 增加的值
 * @return long 增加后的值
 */
long atomic_add(atomic_t* atomic, long value)
{
    if (atomic == NULL) {
        return 0;
    }
    
#if defined(_WIN32) || defined(_WIN64)
    return InterlockedExchangeAdd(&atomic->value, value) + value;
#else
    long old_value, new_value;
    pthread_mutex_lock(&atomic->lock);
    old_value = atomic->value;
    new_value = old_value + value;
    atomic->value = new_value;
    pthread_mutex_unlock(&atomic->lock);
    return new_value;
#endif
}

/**
 * @brief 原子比较并交换
 *
 * @param atomic 原子变量指针
 * @param old_value 期望的旧值
 * @param new_value 要设置的新值
 * @return int 如果交换成功返回非零，否则返回零
 */
int atomic_compare_and_swap(atomic_t* atomic, long old_value, long new_value)
{
    if (atomic == NULL) {
        return 0;
    }
    
#if defined(_WIN32) || defined(_WIN64)
    return (InterlockedCompareExchange(&atomic->value, new_value, old_value) == old_value) ? 1 : 0;
#else
    int result = 0;
    pthread_mutex_lock(&atomic->lock);
    if (atomic->value == old_value) {
        atomic->value = new_value;
        result = 1;
    }
    pthread_mutex_unlock(&atomic->lock);
    return result;
#endif
}

/* 内存池和对象池支持函数实现 */

/**
 * @brief 创建内存池
 *
 * @param block_size 块大小
 * @param grow_size 增长大小
 * @param allocator 底层分配器，如果为NULL则使用默认分配器
 * @return mem_pool_t* 内存池指针，失败返回NULL
 */
mem_pool_t* mem_pool_create(size_t block_size, size_t grow_size, allocator_t* allocator)
{
    if (block_size == 0 || grow_size == 0) {
        return NULL;
    }
    
    /* 如果未提供分配器，使用默认分配器 */
    if (allocator == NULL) {
        allocator = default_allocator();
    }
    
    /* 分配内存池结构体 */
    mem_pool_t* pool = (mem_pool_t*)allocator->allocate(allocator, sizeof(mem_pool_t));
    if (pool == NULL) {
        return NULL;
    }
    
    /* 初始化内存池 */
    pool->free_list = NULL;
    pool->block_size = block_size;
    pool->grow_size = grow_size;
    pool->allocator = allocator;
    
    /* 初始化互斥锁和原子计数器 */
    if (mutex_init(&pool->lock) != CSTL_OK) {
        allocator->deallocate(allocator, pool);
        return NULL;
    }
    
    if (atomic_init(&pool->allocated_blocks, 0) != CSTL_OK) {
        mutex_destroy(&pool->lock);
        allocator->deallocate(allocator, pool);
        return NULL;
    }
    
    if (atomic_init(&pool->free_blocks, 0) != CSTL_OK) {
        atomic_destroy(&pool->allocated_blocks);
        mutex_destroy(&pool->lock);
        allocator->deallocate(allocator, pool);
        return NULL;
    }
    
    return pool;
}

/**
 * @brief 销毁内存池
 *
 * @param pool 内存池指针
 */
void mem_pool_destroy(mem_pool_t* pool)
{
    if (pool == NULL) {
        return;
    }
    
    /* 锁定内存池 */
    mutex_lock(&pool->lock);
    
    /* 释放所有块 */
    mem_pool_block_t* block = pool->free_list;
    while (block != NULL) {
        mem_pool_block_t* next = block->next;
        pool->allocator->deallocate(pool->allocator, block);
        block = next;
    }
    
    /* 解锁内存池 */
    mutex_unlock(&pool->lock);
    
    /* 销毁互斥锁和原子计数器 */
    atomic_destroy(&pool->allocated_blocks);
    atomic_destroy(&pool->free_blocks);
    mutex_destroy(&pool->lock);
    
    /* 释放内存池结构体 */
    pool->allocator->deallocate(pool->allocator, pool);
}

/**
 * @brief 从内存池分配内存
 *
 * @param pool 内存池指针
 * @return void* 分配的内存指针，失败返回NULL
 */
void* mem_pool_alloc(mem_pool_t* pool)
{
    if (pool == NULL) {
        return NULL;
    }
    
    /* 锁定内存池 */
    mutex_lock(&pool->lock);
    
    /* 检查空闲链表 */
    mem_pool_block_t* block = pool->free_list;
    if (block != NULL) {
        /* 从空闲链表中移除 */
        pool->free_list = block->next;
        atomic_add(&pool->free_blocks, -1);
    } else {
        /* 分配新块 */
        size_t total_size = sizeof(mem_pool_block_t) + pool->block_size - 1;
        block = (mem_pool_block_t*)pool->allocator->allocate(pool->allocator, total_size);
        if (block == NULL) {
            mutex_unlock(&pool->lock);
            return NULL;
        }
        block->size = pool->block_size;
        
        /* 预分配更多块以提高性能 */
        for (size_t i = 1; i < pool->grow_size; i++) {
            mem_pool_block_t* new_block = (mem_pool_block_t*)pool->allocator->allocate(pool->allocator, total_size);
            if (new_block != NULL) {
                new_block->size = pool->block_size;
                new_block->next = pool->free_list;
                pool->free_list = new_block;
                atomic_add(&pool->free_blocks, 1);
            }
        }
    }
    
    /* 更新已分配块计数 */
    atomic_add(&pool->allocated_blocks, 1);
    
    /* 解锁内存池 */
    mutex_unlock(&pool->lock);
    
    return block->data;
}

/**
 * @brief 释放内存回内存池
 *
 * @param pool 内存池指针
 * @param ptr 要释放的内存指针
 */
void mem_pool_free(mem_pool_t* pool, void* ptr)
{
    if (pool == NULL || ptr == NULL) {
        return;
    }
    
    /* 计算块指针 */
    mem_pool_block_t* block = (mem_pool_block_t*)((char*)ptr - offsetof(mem_pool_block_t, data));
    
    /* 锁定内存池 */
    mutex_lock(&pool->lock);
    
    /* 将块添加到空闲链表 */
    block->next = pool->free_list;
    pool->free_list = block;
    
    /* 更新计数 */
    atomic_add(&pool->allocated_blocks, -1);
    atomic_add(&pool->free_blocks, 1);
    
    /* 解锁内存池 */
    mutex_unlock(&pool->lock);
}

/**
 * @brief 获取内存池统计信息
 *
 * @param pool 内存池指针
 * @param allocated_blocks 输出参数，已分配块数量
 * @param free_blocks 输出参数，空闲块数量
 * @return error_code_t 错误码
 */
error_code_t mem_pool_get_stats(mem_pool_t* pool, size_t* allocated_blocks, size_t* free_blocks)
{
    if (pool == NULL || allocated_blocks == NULL || free_blocks == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *allocated_blocks = (size_t)atomic_read(&pool->allocated_blocks);
    *free_blocks = (size_t)atomic_read(&pool->free_blocks);
    
    return CSTL_OK;
}

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
                           destructor_fn_t destructor, allocator_t* allocator)
{
    if (obj_size == 0 || initial_size == 0 || grow_size == 0) {
        return NULL;
    }
    
    /* 如果未提供分配器，使用默认分配器 */
    if (allocator == NULL) {
        allocator = default_allocator();
    }
    
    /* 分配对象池结构体 */
    obj_pool_t* pool = (obj_pool_t*)allocator->allocate(allocator, sizeof(obj_pool_t));
    if (pool == NULL) {
        return NULL;
    }
    
    /* 分配空闲对象指针数组 */
    void** free_list = (void**)allocator->allocate(allocator, initial_size * sizeof(void*));
    if (free_list == NULL) {
        allocator->deallocate(allocator, pool);
        return NULL;
    }
    
    /* 初始化对象池 */
    pool->free_list = free_list;
    pool->obj_size = obj_size;
    pool->capacity = initial_size;
    pool->count = 0;
    pool->grow_size = grow_size;
    pool->destructor = destructor;
    pool->allocator = allocator;
    
    /* 初始化互斥锁和原子计数器 */
    if (mutex_init(&pool->lock) != CSTL_OK) {
        allocator->deallocate(allocator, free_list);
        allocator->deallocate(allocator, pool);
        return NULL;
    }
    
    if (atomic_init(&pool->allocated_objects, 0) != CSTL_OK) {
        mutex_destroy(&pool->lock);
        allocator->deallocate(allocator, free_list);
        allocator->deallocate(allocator, pool);
        return NULL;
    }
    
    if (atomic_init(&pool->free_objects, 0) != CSTL_OK) {
        atomic_destroy(&pool->allocated_objects);
        mutex_destroy(&pool->lock);
        allocator->deallocate(allocator, free_list);
        allocator->deallocate(allocator, pool);
        return NULL;
    }
    
    /* 预分配对象 */
    for (size_t i = 0; i < initial_size; i++) {
        void* obj = allocator->allocate(allocator, obj_size);
        if (obj != NULL) {
            pool->free_list[pool->count++] = obj;
            atomic_add(&pool->free_objects, 1);
        }
    }
    
    return pool;
}

/**
 * @brief 销毁对象池
 *
 * @param pool 对象池指针
 */
void obj_pool_destroy(obj_pool_t* pool)
{
    if (pool == NULL) {
        return;
    }
    
    /* 锁定对象池 */
    mutex_lock(&pool->lock);
    
    /* 释放所有对象 */
    for (size_t i = 0; i < pool->count; i++) {
        if (pool->destructor != NULL) {
            pool->destructor(pool->free_list[i]);
        }
        pool->allocator->deallocate(pool->allocator, pool->free_list[i]);
    }
    
    /* 释放空闲对象指针数组 */
    pool->allocator->deallocate(pool->allocator, pool->free_list);
    
    /* 解锁对象池 */
    mutex_unlock(&pool->lock);
    
    /* 销毁互斥锁和原子计数器 */
    atomic_destroy(&pool->allocated_objects);
    atomic_destroy(&pool->free_objects);
    mutex_destroy(&pool->lock);
    
    /* 释放对象池结构体 */
    pool->allocator->deallocate(pool->allocator, pool);
}

/**
 * @brief 从对象池分配对象
 *
 * @param pool 对象池指针
 * @return void* 分配的对象指针，失败返回NULL
 */
void* obj_pool_alloc(obj_pool_t* pool)
{
    if (pool == NULL) {
        return NULL;
    }
    
    /* 锁定对象池 */
    mutex_lock(&pool->lock);
    
    void* obj = NULL;
    
    /* 检查是否有空闲对象 */
    if (pool->count > 0) {
        /* 从空闲列表中获取对象 */
        obj = pool->free_list[--pool->count];
        atomic_add(&pool->free_objects, -1);
    } else {
        /* 扩展对象池 */
        size_t new_capacity = pool->capacity + pool->grow_size;
        void** new_free_list = (void**)pool->allocator->reallocate(pool->allocator, pool->free_list, new_capacity * sizeof(void*));
        if (new_free_list == NULL) {
            mutex_unlock(&pool->lock);
            return NULL;
        }
        
        pool->free_list = new_free_list;
        
        /* 分配新对象 */
        for (size_t i = 0; i < pool->grow_size; i++) {
            void* new_obj = pool->allocator->allocate(pool->allocator, pool->obj_size);
            if (new_obj != NULL) {
                pool->free_list[pool->count++] = new_obj;
                atomic_add(&pool->free_objects, 1);
            }
        }
        
        pool->capacity = new_capacity;
        
        /* 再次尝试获取对象 */
        if (pool->count > 0) {
            obj = pool->free_list[--pool->count];
            atomic_add(&pool->free_objects, -1);
        }
    }
    
    /* 更新已分配对象计数 */
    if (obj != NULL) {
        atomic_add(&pool->allocated_objects, 1);
    }
    
    /* 解锁对象池 */
    mutex_unlock(&pool->lock);
    
    return obj;
}

/**
 * @brief 释放对象回对象池
 *
 * @param pool 对象池指针
 * @param obj 要释放的对象指针
 */
void obj_pool_free(obj_pool_t* pool, void* obj)
{
    if (pool == NULL || obj == NULL) {
        return;
    }
    
    /* 锁定对象池 */
    mutex_lock(&pool->lock);
    
    /* 检查是否需要扩展空闲列表 */
    if (pool->count >= pool->capacity) {
        size_t new_capacity = pool->capacity + pool->grow_size;
        void** new_free_list = (void**)pool->allocator->reallocate(pool->allocator, pool->free_list, new_capacity * sizeof(void*));
        if (new_free_list != NULL) {
            pool->free_list = new_free_list;
            pool->capacity = new_capacity;
        }
    }
    
    /* 将对象添加到空闲列表 */
    if (pool->count < pool->capacity) {
        pool->free_list[pool->count++] = obj;
        atomic_add(&pool->allocated_objects, -1);
        atomic_add(&pool->free_objects, 1);
    } else {
        /* 如果无法添加到空闲列表，直接释放对象 */
        if (pool->destructor != NULL) {
            pool->destructor(obj);
        }
        pool->allocator->deallocate(pool->allocator, obj);
    }
    
    /* 解锁对象池 */
    mutex_unlock(&pool->lock);
}

/**
 * @brief 获取对象池统计信息
 *
 * @param pool 对象池指针
 * @param allocated_objects 输出参数，已分配对象数量
 * @param free_objects 输出参数，空闲对象数量
 * @return error_code_t 错误码
 */
error_code_t obj_pool_get_stats(obj_pool_t* pool, size_t* allocated_objects, size_t* free_objects)
{
    if (pool == NULL || allocated_objects == NULL || free_objects == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    *allocated_objects = (size_t)atomic_read(&pool->allocated_objects);
    *free_objects = (size_t)atomic_read(&pool->free_objects);
    
    return CSTL_OK;
}