/**
 * @file list.h
 * @brief CSTL库的双向链表容器头文件
 * 
 * 该文件定义了CSTL库的双向链表容器，包括节点结构、
 * 核心操作和迭代器支持。
 */

#ifndef CSTL_LIST_H
#define CSTL_LIST_H

#include "cstl/common.h"
#include "cstl/iterator.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 双向链表节点结构体
 */
typedef struct list_node_t {
    struct list_node_t* prev;  /**< 前一个节点指针 */
    struct list_node_t* next;  /**< 下一个节点指针 */
    void* data;               /**< 数据指针 */
} list_node_t;

/**
 * @brief 双向链表容器结构体
 */
typedef struct list_t {
    /**
     * @brief 头节点指针
     */
    list_node_t* head;
    
    /**
     * @brief 尾节点指针
     */
    list_node_t* tail;
    
    /**
     * @brief 节点数量
     */
    size_t size;
    
    /**
     * @brief 元素大小
     */
    size_t element_size;
    
    /**
     * @brief 分配器指针
     */
    allocator_t* allocator;
    
    /**
     * @brief 对象池指针（可选）
     */
    obj_pool_t* node_pool;
    
    /**
     * @brief 互斥锁（线程安全选项）
     */
    mutex_t lock;
    
    /**
     * @brief 是否启用线程安全
     */
    int thread_safe;
    
    /**
     * @brief 析构函数指针，用于释放元素内存
     */
    destructor_fn_t destructor;
} list_t;

/**
 * @brief 创建双向链表容器
 * 
 * @param element_size 元素大小
 * @param allocator 分配器指针，如果为NULL则使用默认分配器
 * @param destructor 析构函数指针，如果为NULL则使用默认释放方式
 * @return list_t* 双向链表容器指针，失败返回NULL
 */
list_t* list_create(size_t element_size, allocator_t* allocator, destructor_fn_t destructor);

/**
 * @brief 销毁双向链表容器
 * 
 * @param list 双向链表容器指针
 */
void list_destroy(list_t* list);

/**
 * @brief 初始化双向链表容器
 * 
 * @param list 双向链表容器指针
 * @param element_size 元素大小
 * @param allocator 分配器指针，如果为NULL则使用默认分配器
 * @return error_code_t 错误码
 */
error_code_t list_init(list_t* list, size_t element_size, allocator_t* allocator);

/**
 * @brief 清空双向链表容器
 * 
 * @param list 双向链表容器指针
 */
void list_clear(list_t* list);

/**
 * @brief 获取双向链表容器大小
 * 
 * @param list 双向链表容器指针
 * @return size_t 节点数量
 */
size_t list_size(const list_t* list);

/**
 * @brief 检查双向链表容器是否为空
 * 
 * @param list 双向链表容器指针
 * @return int 如果为空返回非零，否则返回零
 */
int list_empty(const list_t* list);

/**
 * @brief 在双向链表容器前端添加元素
 * 
 * @param list 双向链表容器指针
 * @param element 要添加的元素指针
 * @return error_code_t 错误码
 */
error_code_t list_push_front(list_t* list, const void* element);

/**
 * @brief 在双向链表容器后端添加元素
 * 
 * @param list 双向链表容器指针
 * @param element 要添加的元素指针
 * @return error_code_t 错误码
 */
error_code_t list_push_back(list_t* list, const void* element);

/**
 * @brief 移除双向链表容器前端元素
 * 
 * @param list 双向链表容器指针
 * @return error_code_t 错误码
 */
error_code_t list_pop_front(list_t* list);

/**
 * @brief 移除双向链表容器后端元素
 * 
 * @param list 双向链表容器指针
 * @return error_code_t 错误码
 */
error_code_t list_pop_back(list_t* list);

/**
 * @brief 获取双向链表容器前端元素
 * 
 * @param list 双向链表容器指针
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t list_front(const list_t* list, void** element);

/**
 * @brief 获取双向链表容器后端元素
 * 
 * @param list 双向链表容器指针
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t list_back(const list_t* list, void** element);

/**
 * @brief 在指定位置插入元素
 * 
 * @param list 双向链表容器指针
 * @param position 插入位置节点指针
 * @param element 要插入的元素指针
 * @return error_code_t 错误码
 */
error_code_t list_insert(list_t* list, list_node_t* position, const void* element);

/**
 * @brief 在指定位置前插入元素
 * 
 * @param list 双向链表容器指针
 * @param position 插入位置节点指针
 * @param element 要插入的元素指针
 * @return error_code_t 错误码
 */
error_code_t list_insert_before(list_t* list, list_node_t* position, const void* element);

/**
 * @brief 在指定位置后插入元素
 * 
 * @param list 双向链表容器指针
 * @param position 插入位置节点指针
 * @param element 要插入的元素指针
 * @return error_code_t 错误码
 */
error_code_t list_insert_after(list_t* list, list_node_t* position, const void* element);

/**
 * @brief 移除指定位置元素
 * 
 * @param list 双向链表容器指针
 * @param position 要移除的节点指针
 * @return error_code_t 错误码
 */
error_code_t list_erase(list_t* list, list_node_t* position);

/**
 * @brief 移除指定元素
 * 
 * @param list 双向链表容器指针
 * @param element 要移除的元素指针
 * @param comparator 比较函数指针
 * @return error_code_t 错误码
 */
error_code_t list_remove(list_t* list, const void* element, comparator_fn_t comparator);

/**
 * @brief 查找指定元素
 * 
 * @param list 双向链表容器指针
 * @param element 要查找的元素指针
 * @param comparator 比较函数指针
 * @return list_node_t* 找到的节点指针，未找到返回NULL
 */
list_node_t* list_find(const list_t* list, const void* element, comparator_fn_t comparator);

/**
 * @brief 反转双向链表容器
 * 
 * @param list 双向链表容器指针
 * @return error_code_t 错误码
 */
error_code_t list_reverse(list_t* list);

/**
 * @brief 合并两个双向链表容器
 * 
 * @param list1 第一个双向链表容器指针
 * @param list2 第二个双向链表容器指针
 * @return error_code_t 错误码
 */
error_code_t list_merge(list_t* list1, list_t* list2);

/**
 * @brief 排序双向链表容器
 * 
 * @param list 双向链表容器指针
 * @param comparator 比较函数指针
 * @return error_code_t 错误码
 */
error_code_t list_sort(list_t* list, comparator_fn_t comparator);

/**
 * @brief 启用线程安全
 * 
 * @param list 双向链表容器指针
 * @return error_code_t 错误码
 */
error_code_t list_enable_thread_safety(list_t* list);

/**
 * @brief 禁用线程安全
 * 
 * @param list 双向链表容器指针
 * @return error_code_t 错误码
 */
error_code_t list_disable_thread_safety(list_t* list);

/**
 * @brief 设置节点对象池
 * 
 * @param list 双向链表容器指针
 * @param node_pool 节点对象池指针
 * @return error_code_t 错误码
 */
error_code_t list_set_node_pool(list_t* list, obj_pool_t* node_pool);

/**
 * @brief 移除节点对象池
 * 
 * @param list 双向链表容器指针
 * @return error_code_t 错误码
 */
error_code_t list_remove_node_pool(list_t* list);

/**
 * @brief 创建双向链表容器迭代器
 * 
 * @param list 双向链表容器指针
 * @param direction 迭代方向
 * @return iterator_t* 迭代器指针，失败返回NULL
 */
iterator_t* list_iterator_create(list_t* list, iter_direction_t direction);

/**
 * @brief 获取双向链表容器起始迭代器
 *
 * @param list 双向链表容器指针
 * @return iterator_t* 起始迭代器指针，失败返回NULL
 */
iterator_t* list_begin(list_t* list);

/**
 * @brief 获取双向链表容器结束迭代器
 *
 * @param list 双向链表容器指针
 * @return iterator_t* 结束迭代器指针，失败返回NULL
 */
iterator_t* list_end(list_t* list);

/**
 * @brief 获取双向链表容器指定位置的元素
 *
 * @param list 双向链表容器指针
 * @param index 元素索引
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t list_at(const list_t* list, size_t index, void** element);

/**
 * @brief 设置双向链表容器指定位置的元素
 *
 * @param list 双向链表容器指针
 * @param index 元素索引
 * @param element 要设置的元素指针
 * @return error_code_t 错误码
 */
error_code_t list_set(list_t* list, size_t index, const void* element);

#ifdef __cplusplus
}
#endif

#endif /* CSTL_LIST_H */
