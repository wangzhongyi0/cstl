/**
 * @file list.c
 * @brief CSTL库的双向链表容器实现
 * 
 * 该文件实现了CSTL库的双向链表容器，包括节点结构、
 * 核心操作和迭代器支持。
 */

#include "cstl/list.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/**
 * @brief 双向链表迭代器结构体
 */
typedef struct list_iterator_t {
    iterator_t base;         /**< 基础迭代器 */
    list_node_t* node;       /**< 当前节点 */
} list_iterator_t;

/**
 * @brief 双向链表迭代器next函数实现
 * 
 * @param iterator 迭代器指针
 * @return error_code_t 错误码
 */
static error_code_t list_iterator_next(iterator_t* iterator)
{
    if (iterator == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_iterator_t* list_iter = (list_iterator_t*)iterator;
    
    if (list_iter->node == NULL) {
        return CSTL_ERROR_ITERATOR_END;
    }
    
    list_iter->node = list_iter->node->next;
    iterator->current = list_iter->node ? list_iter->node->data : NULL;
    
    return CSTL_OK;
}

/**
 * @brief 双向链表迭代器prev函数实现
 * 
 * @param iterator 迭代器指针
 * @return error_code_t 错误码
 */
static error_code_t list_iterator_prev(iterator_t* iterator)
{
    if (iterator == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_iterator_t* list_iter = (list_iterator_t*)iterator;
    
    if (list_iter->node == NULL) {
        return CSTL_ERROR_ITERATOR_END;
    }
    
    list_iter->node = list_iter->node->prev;
    iterator->current = list_iter->node ? list_iter->node->data : NULL;
    
    return CSTL_OK;
}

/**
 * @brief 双向链表迭代器get函数实现
 * 
 * @param iterator 迭代器指针
 * @param data 输出参数，存储当前元素指针
 * @return error_code_t 错误码
 */
static error_code_t list_iterator_get(iterator_t* iterator, void** data)
{
    if (iterator == NULL || data == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_iterator_t* list_iter = (list_iterator_t*)iterator;
    
    if (list_iter->node == NULL) {
        return CSTL_ERROR_ITERATOR_END;
    }
    
    *data = list_iter->node->data;
    return CSTL_OK;
}

/**
 * @brief 双向链表迭代器valid函数实现
 * 
 * @param iterator 迭代器指针
 * @return int 如果有效返回非零，否则返回零
 */
static int list_iterator_valid(iterator_t* iterator)
{
    if (iterator == NULL) {
        return 0;
    }
    
    list_iterator_t* list_iter = (list_iterator_t*)iterator;
    return list_iter->node != NULL;
}

/**
 * @brief 双向链表迭代器destroy函数实现
 * 
 * @param iterator 迭代器指针
 */
static void list_iterator_destroy(iterator_t* iterator)
{
    /* 不需要特殊处理，迭代器将在iterator_destroy中释放 */
    (void)iterator;
}

/**
 * @brief 锁定双向链表容器（如果启用线程安全）
 * 
 * @param list 双向链表容器指针
 */
static void list_lock(list_t* list)
{
    if (list != NULL && list->thread_safe) {
        mutex_lock(&list->lock);
    }
}

/**
 * @brief 解锁双向链表容器（如果启用线程安全）
 * 
 * @param list 双向链表容器指针
 */
static void list_unlock(list_t* list)
{
    if (list != NULL && list->thread_safe) {
        mutex_unlock(&list->lock);
    }
}

/**
 * @brief 创建双向链表节点
 * 
 * @param list 双向链表容器指针
 * @param element 元素指针
 * @return list_node_t* 节点指针，失败返回NULL
 */
static list_node_t* list_create_node(list_t* list, const void* element)
{
    list_node_t* node;
    
    if (list->node_pool != NULL) {
        /* 使用对象池分配节点 */
        node = (list_node_t*)obj_pool_alloc(list->node_pool);
        if (node == NULL) {
            return NULL;
        }
        
        /* 分配数据内存 */
        node->data = list->allocator->allocate(list->allocator, list->element_size);
        if (node->data == NULL) {
            obj_pool_free(list->node_pool, node);
            return NULL;
        }
    } else {
        /* 直接分配节点和数据内存 */
        size_t total_size = sizeof(list_node_t) + list->element_size;
        node = (list_node_t*)list->allocator->allocate(list->allocator, total_size);
        if (node == NULL) {
            return NULL;
        }
        
        node->data = (char*)node + sizeof(list_node_t);
    }
    
    /* 初始化节点 */
    node->prev = NULL;
    node->next = NULL;
    
    /* 复制数据 */
    memcpy(node->data, element, list->element_size);
    
    return node;
}

/**
 * @brief 销毁双向链表节点
 * 
 * @param list 双向链表容器指针
 * @param node 节点指针
 */
static void list_destroy_node(list_t* list, list_node_t* node)
{
    if (list == NULL || node == NULL) {
        return;
    }
    
    /* 调用析构函数释放元素内存 */
    if (list->destructor != NULL) {
        list->destructor(node->data);
    }
    
    if (list->node_pool != NULL) {
        /* 释放数据内存 */
        list->allocator->deallocate(list->allocator, node->data);
        
        /* 将节点返回对象池 */
        obj_pool_free(list->node_pool, node);
    } else {
        /* 直接释放节点和数据内存 */
        list->allocator->deallocate(list->allocator, node);
    }
}

/**
 * @brief 创建双向链表容器
 * 
 * @param element_size 元素大小
 * @param allocator 分配器指针，如果为NULL则使用默认分配器
 * @param destructor 析构函数指针，如果为NULL则使用默认释放方式
 * @return list_t* 双向链表容器指针，失败返回NULL
 */
list_t* list_create(size_t element_size, allocator_t* allocator, destructor_fn_t destructor)
{
    if (element_size == 0) {
        return NULL;
    }
    
    /* 分配链表结构体 */
    list_t* list = (list_t*)malloc(sizeof(list_t));
    if (list == NULL) {
        return NULL;
    }
    
    /* 初始化链表 */
    error_code_t result = list_init(list, element_size, allocator);
    if (result != CSTL_OK) {
        free(list);
        return NULL;
    }
    
    /* 设置析构函数 */
    list->destructor = destructor;
    
    return list;
}

/**
 * @brief 销毁双向链表容器
 * 
 * @param list 双向链表容器指针
 */
void list_destroy(list_t* list)
{
    if (list == NULL) {
        return;
    }
    
    list_clear(list);
    
    /* 销毁互斥锁 */
    if (list->thread_safe) {
        mutex_destroy(&list->lock);
    }
    
    free(list);
}

/**
 * @brief 初始化双向链表容器
 * 
 * @param list 双向链表容器指针
 * @param element_size 元素大小
 * @param allocator 分配器指针，如果为NULL则使用默认分配器
 * @return error_code_t 错误码
 */
error_code_t list_init(list_t* list, size_t element_size, allocator_t* allocator)
{
    if (list == NULL || element_size == 0) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 设置分配器 */
    if (allocator == NULL) {
        allocator = default_allocator();
    }
    
    /* 初始化链表 */
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->element_size = element_size;
    list->allocator = allocator;
    list->node_pool = NULL;
    list->thread_safe = 0;
    
    return CSTL_OK;
}

/**
 * @brief 清空双向链表容器
 * 
 * @param list 双向链表容器指针
 */
void list_clear(list_t* list)
{
    if (list == NULL) {
        return;
    }
    
    list_lock(list);
    
    /* 释放所有节点 */
    list_node_t* node = list->head;
    while (node != NULL) {
        list_node_t* next = node->next;
        list_destroy_node(list, node);
        node = next;
    }
    
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    
    list_unlock(list);
}

/**
 * @brief 获取双向链表容器大小
 * 
 * @param list 双向链表容器指针
 * @return size_t 节点数量
 */
size_t list_size(const list_t* list)
{
    if (list == NULL) {
        return 0;
    }
    
    return list->size;
}

/**
 * @brief 检查双向链表容器是否为空
 * 
 * @param list 双向链表容器指针
 * @return int 如果为空返回非零，否则返回零
 */
int list_empty(const list_t* list)
{
    if (list == NULL) {
        return 1;
    }
    
    return list->size == 0;
}

/**
 * @brief 在双向链表容器前端添加元素
 * 
 * @param list 双向链表容器指针
 * @param element 要添加的元素指针
 * @return error_code_t 错误码
 */
error_code_t list_push_front(list_t* list, const void* element)
{
    if (list == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_lock(list);
    
    /* 创建新节点 */
    list_node_t* node = list_create_node(list, element);
    if (node == NULL) {
        list_unlock(list);
        return CSTL_ERROR_OUT_OF_MEMORY;
    }
    
    /* 添加到链表前端 */
    node->next = list->head;
    if (list->head != NULL) {
        list->head->prev = node;
    } else {
        list->tail = node;
    }
    list->head = node;
    
    list->size++;
    
    list_unlock(list);
    return CSTL_OK;
}

/**
 * @brief 在双向链表容器后端添加元素
 * 
 * @param list 双向链表容器指针
 * @param element 要添加的元素指针
 * @return error_code_t 错误码
 */
error_code_t list_push_back(list_t* list, const void* element)
{
    if (list == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_lock(list);
    
    /* 创建新节点 */
    list_node_t* node = list_create_node(list, element);
    if (node == NULL) {
        list_unlock(list);
        return CSTL_ERROR_OUT_OF_MEMORY;
    }
    
    /* 添加到链表后端 */
    node->prev = list->tail;
    if (list->tail != NULL) {
        list->tail->next = node;
    } else {
        list->head = node;
    }
    list->tail = node;
    
    list->size++;
    
    list_unlock(list);
    return CSTL_OK;
}

/**
 * @brief 移除双向链表容器前端元素
 * 
 * @param list 双向链表容器指针
 * @return error_code_t 错误码
 */
error_code_t list_pop_front(list_t* list)
{
    if (list == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_lock(list);
    
    if (list->head == NULL) {
        list_unlock(list);
        return CSTL_ERROR_CONTAINER_EMPTY;
    }
    
    /* 移除头节点 */
    list_node_t* node = list->head;
    list->head = node->next;
    
    if (list->head != NULL) {
        list->head->prev = NULL;
    } else {
        list->tail = NULL;
    }
    
    list_destroy_node(list, node);
    list->size--;
    
    list_unlock(list);
    return CSTL_OK;
}

/**
 * @brief 移除双向链表容器后端元素
 * 
 * @param list 双向链表容器指针
 * @return error_code_t 错误码
 */
error_code_t list_pop_back(list_t* list)
{
    if (list == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_lock(list);
    
    if (list->tail == NULL) {
        list_unlock(list);
        return CSTL_ERROR_CONTAINER_EMPTY;
    }
    
    /* 移除尾节点 */
    list_node_t* node = list->tail;
    list->tail = node->prev;
    
    if (list->tail != NULL) {
        list->tail->next = NULL;
    } else {
        list->head = NULL;
    }
    
    list_destroy_node(list, node);
    list->size--;
    
    list_unlock(list);
    return CSTL_OK;
}

/**
 * @brief 获取双向链表容器前端元素
 * 
 * @param list 双向链表容器指针
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t list_front(const list_t* list, void** element)
{
    if (list == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    if (list->head == NULL) {
        return CSTL_ERROR_CONTAINER_EMPTY;
    }
    
    *element = list->head->data;
    return CSTL_OK;
}

/**
 * @brief 获取双向链表容器后端元素
 * 
 * @param list 双向链表容器指针
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t list_back(const list_t* list, void** element)
{
    if (list == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    if (list->tail == NULL) {
        return CSTL_ERROR_CONTAINER_EMPTY;
    }
    
    *element = list->tail->data;
    return CSTL_OK;
}

/**
 * @brief 在指定位置插入元素
 * 
 * @param list 双向链表容器指针
 * @param position 插入位置节点指针
 * @param element 要插入的元素指针
 * @return error_code_t 错误码
 */
error_code_t list_insert(list_t* list, list_node_t* position, const void* element)
{
    return list_insert_before(list, position, element);
}

/**
 * @brief 在指定位置前插入元素
 * 
 * @param list 双向链表容器指针
 * @param position 插入位置节点指针
 * @param element 要插入的元素指针
 * @return error_code_t 错误码
 */
error_code_t list_insert_before(list_t* list, list_node_t* position, const void* element)
{
    if (list == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_lock(list);
    
    /* 创建新节点 */
    list_node_t* node = list_create_node(list, element);
    if (node == NULL) {
        list_unlock(list);
        return CSTL_ERROR_OUT_OF_MEMORY;
    }
    
    if (position == NULL) {
        /* 插入到链表后端 */
        node->prev = list->tail;
        if (list->tail != NULL) {
            list->tail->next = node;
        } else {
            list->head = node;
        }
        list->tail = node;
    } else {
        /* 插入到指定位置前 */
        node->prev = position->prev;
        node->next = position;
        
        if (position->prev != NULL) {
            position->prev->next = node;
        } else {
            list->head = node;
        }
        position->prev = node;
    }
    
    list->size++;
    
    list_unlock(list);
    return CSTL_OK;
}

/**
 * @brief 在指定位置后插入元素
 * 
 * @param list 双向链表容器指针
 * @param position 插入位置节点指针
 * @param element 要插入的元素指针
 * @return error_code_t 错误码
 */
error_code_t list_insert_after(list_t* list, list_node_t* position, const void* element)
{
    if (list == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_lock(list);
    
    /* 创建新节点 */
    list_node_t* node = list_create_node(list, element);
    if (node == NULL) {
        list_unlock(list);
        return CSTL_ERROR_OUT_OF_MEMORY;
    }
    
    if (position == NULL) {
        /* 插入到链表前端 */
        node->next = list->head;
        if (list->head != NULL) {
            list->head->prev = node;
        } else {
            list->tail = node;
        }
        list->head = node;
    } else {
        /* 插入到指定位置后 */
        node->prev = position;
        node->next = position->next;
        
        if (position->next != NULL) {
            position->next->prev = node;
        } else {
            list->tail = node;
        }
        position->next = node;
    }
    
    list->size++;
    
    list_unlock(list);
    return CSTL_OK;
}

/**
 * @brief 移除指定位置元素
 * 
 * @param list 双向链表容器指针
 * @param position 要移除的节点指针
 * @return error_code_t 错误码
 */
error_code_t list_erase(list_t* list, list_node_t* position)
{
    if (list == NULL || position == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_lock(list);
    
    /* 更新前驱节点的后继指针 */
    if (position->prev != NULL) {
        position->prev->next = position->next;
    } else {
        list->head = position->next;
    }
    
    /* 更新后继节点的前驱指针 */
    if (position->next != NULL) {
        position->next->prev = position->prev;
    } else {
        list->tail = position->prev;
    }
    
    /* 销毁节点 */
    list_destroy_node(list, position);
    list->size--;
    
    list_unlock(list);
    return CSTL_OK;
}

/**
 * @brief 移除指定元素
 * 
 * @param list 双向链表容器指针
 * @param element 要移除的元素指针
 * @param comparator 比较函数指针
 * @return error_code_t 错误码
 */
error_code_t list_remove(list_t* list, const void* element, comparator_fn_t comparator)
{
    if (list == NULL || element == NULL || comparator == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_lock(list);
    
    list_node_t* node = list->head;
    while (node != NULL) {
        if (comparator(node->data, element) == 0) {
            /* 找到匹配的元素，移除节点 */
            if (node->prev != NULL) {
                node->prev->next = node->next;
            } else {
                list->head = node->next;
            }
            
            if (node->next != NULL) {
                node->next->prev = node->prev;
            } else {
                list->tail = node->prev;
            }
            
            list_node_t* next = node->next;
            list_destroy_node(list, node);
            list->size--;
            
            node = next;
        } else {
            node = node->next;
        }
    }
    
    list_unlock(list);
    return CSTL_OK;
}

/**
 * @brief 查找指定元素
 * 
 * @param list 双向链表容器指针
 * @param element 要查找的元素指针
 * @param comparator 比较函数指针
 * @return list_node_t* 找到的节点指针，未找到返回NULL
 */
list_node_t* list_find(const list_t* list, const void* element, comparator_fn_t comparator)
{
    if (list == NULL || element == NULL || comparator == NULL) {
        return NULL;
    }
    
    list_node_t* node = list->head;
    while (node != NULL) {
        if (comparator(node->data, element) == 0) {
            return node;
        }
        node = node->next;
    }
    
    return NULL;
}

/**
 * @brief 反转双向链表容器
 * 
 * @param list 双向链表容器指针
 * @return error_code_t 错误码
 */
error_code_t list_reverse(list_t* list)
{
    if (list == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_lock(list);
    
    list_node_t* node = list->head;
    list_node_t* temp = NULL;
    
    /* 交换头尾指针 */
    list->head = list->tail;
    list->tail = node;
    
    /* 反转每个节点的指针 */
    while (node != NULL) {
        temp = node->prev;
        node->prev = node->next;
        node->next = temp;
        node = node->prev;
    }
    
    list_unlock(list);
    return CSTL_OK;
}

/**
 * @brief 合并两个双向链表容器
 * 
 * @param list1 第一个双向链表容器指针
 * @param list2 第二个双向链表容器指针
 * @return error_code_t 错误码
 */
error_code_t list_merge(list_t* list1, list_t* list2)
{
    if (list1 == NULL || list2 == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_lock(list1);
    list_lock(list2);
    
    if (list2->head == NULL) {
        /* 第二个链表为空，无需合并 */
        list_unlock(list2);
        list_unlock(list1);
        return CSTL_OK;
    }
    
    if (list1->head == NULL) {
        /* 第一个链表为空，直接使用第二个链表 */
        list1->head = list2->head;
        list1->tail = list2->tail;
    } else {
        /* 连接两个链表 */
        list1->tail->next = list2->head;
        list2->head->prev = list1->tail;
        list1->tail = list2->tail;
    }
    
    list1->size += list2->size;
    
    /* 清空第二个链表 */
    list2->head = NULL;
    list2->tail = NULL;
    list2->size = 0;
    
    list_unlock(list2);
    list_unlock(list1);
    return CSTL_OK;
}

/**
 * @brief 归并排序辅助函数
 * 
 * @param list 双向链表容器指针
 * @param comparator 比较函数指针
 * @return list_node_t* 排序后的链表头节点
 */
static list_node_t* list_merge_sort_helper(list_node_t* head, comparator_fn_t comparator)
{
    if (head == NULL || head->next == NULL) {
        return head;
    }
    
    /* 使用快慢指针找到中间节点 */
    list_node_t* slow = head;
    list_node_t* fast = head->next;
    
    while (fast != NULL && fast->next != NULL) {
        slow = slow->next;
        fast = fast->next->next;
    }
    
    /* 分割链表 */
    list_node_t* mid = slow;
    list_node_t* left = head;
    list_node_t* right = mid->next;
    mid->next = NULL;
    right->prev = NULL;
    
    /* 递归排序 */
    left = list_merge_sort_helper(left, comparator);
    right = list_merge_sort_helper(right, comparator);
    
    /* 合并排序后的链表 */
    list_node_t dummy = {0};
    list_node_t* tail = &dummy;
    
    while (left != NULL && right != NULL) {
        if (comparator(left->data, right->data) <= 0) {
            tail->next = left;
            left->prev = tail;
            left = left->next;
        } else {
            tail->next = right;
            right->prev = tail;
            right = right->next;
        }
        tail = tail->next;
    }
    
    /* 连接剩余节点 */
    if (left != NULL) {
        tail->next = left;
        left->prev = tail;
    } else {
        tail->next = right;
        right->prev = tail;
    }
    
    list_node_t* result = dummy.next;
    result->prev = NULL;
    
    return result;
}

/**
 * @brief 排序双向链表容器
 * 
 * @param list 双向链表容器指针
 * @param comparator 比较函数指针
 * @return error_code_t 错误码
 */
error_code_t list_sort(list_t* list, comparator_fn_t comparator)
{
    if (list == NULL || comparator == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_lock(list);
    
    if (list->head != NULL) {
        list->head = list_merge_sort_helper(list->head, comparator);
        
        /* 更新尾节点 */
        list_node_t* node = list->head;
        while (node->next != NULL) {
            node = node->next;
        }
        list->tail = node;
    }
    
    list_unlock(list);
    return CSTL_OK;
}

/**
 * @brief 启用线程安全
 * 
 * @param list 双向链表容器指针
 * @return error_code_t 错误码
 */
error_code_t list_enable_thread_safety(list_t* list)
{
    if (list == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_lock(list);
    
    if (!list->thread_safe) {
        error_code_t result = mutex_init(&list->lock);
        if (result != CSTL_OK) {
            list_unlock(list);
            return result;
        }
        list->thread_safe = 1;
    }
    
    list_unlock(list);
    return CSTL_OK;
}

/**
 * @brief 禁用线程安全
 * 
 * @param list 双向链表容器指针
 * @return error_code_t 错误码
 */
error_code_t list_disable_thread_safety(list_t* list)
{
    if (list == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_lock(list);
    
    if (list->thread_safe) {
        list->thread_safe = 0;
        mutex_unlock(&list->lock);
        mutex_destroy(&list->lock);
        return CSTL_OK;
    }
    
    list_unlock(list);
    return CSTL_OK;
}

/**
 * @brief 设置节点对象池
 * 
 * @param list 双向链表容器指针
 * @param node_pool 节点对象池指针
 * @return error_code_t 错误码
 */
error_code_t list_set_node_pool(list_t* list, obj_pool_t* node_pool)
{
    if (list == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_lock(list);
    list->node_pool = node_pool;
    list_unlock(list);
    
    return CSTL_OK;
}

/**
 * @brief 移除节点对象池
 * 
 * @param list 双向链表容器指针
 * @return error_code_t 错误码
 */
error_code_t list_remove_node_pool(list_t* list)
{
    if (list == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_lock(list);
    list->node_pool = NULL;
    list_unlock(list);
    
    return CSTL_OK;
}

/**
 * @brief 创建双向链表容器迭代器
 * 
 * @param list 双向链表容器指针
 * @param direction 迭代方向
 * @return iterator_t* 迭代器指针，失败返回NULL
 */
iterator_t* list_iterator_create(list_t* list, iter_direction_t direction)
{
    if (list == NULL) {
        return NULL;
    }
    
    /* 分配链表迭代器结构体 */
    list_iterator_t* list_iter = (list_iterator_t*)malloc(sizeof(list_iterator_t));
    if (list_iter == NULL) {
        return NULL;
    }
    
    /* 初始化迭代器 */
    list_iter->base.container = list;
    list_iter->base.current = NULL;
    list_iter->base.direction = direction;
    list_iter->base.element_size = list->element_size;
    list_iter->base.next = list_iterator_next;
    list_iter->base.prev = list_iterator_prev;
    list_iter->base.get = list_iterator_get;
    list_iter->base.valid = list_iterator_valid;
    list_iter->base.destroy = list_iterator_destroy;
    
    /* 设置初始位置 */
    if (direction == ITER_DIR_FORWARD) {
        list_iter->node = list->head;
        if (list_iter->node != NULL) {
            list_iter->base.current = list_iter->node->data;
        }
    } else {
        list_iter->node = list->tail;
        if (list_iter->node != NULL) {
            list_iter->base.current = list_iter->node->data;
        }
    }
    
    return (iterator_t*)list_iter;
}

/**
 * @brief 获取双向链表容器起始迭代器
 *
 * @param list 双向链表容器指针
 * @return iterator_t* 起始迭代器指针，失败返回NULL
 */
iterator_t* list_begin(list_t* list)
{
    if (list == NULL) {
        return NULL;
    }
    
    return list_iterator_create(list, ITER_DIR_FORWARD);
}

/**
 * @brief 获取双向链表容器结束迭代器
 *
 * @param list 双向链表容器指针
 * @return iterator_t* 结束迭代器指针，失败返回NULL
 */
iterator_t* list_end(list_t* list)
{
    if (list == NULL) {
        return NULL;
    }
    
    /* 创建一个指向末尾的迭代器 */
    iterator_t* iterator = list_iterator_create(list, ITER_DIR_FORWARD);
    if (iterator == NULL) {
        return NULL;
    }
    
    /* 将迭代器移动到末尾 */
    list_iterator_t* list_iter = (list_iterator_t*)iterator;
    list_iter->node = NULL;
    iterator->current = NULL;
    
    return iterator;
}

/**
 * @brief 获取双向链表容器指定位置的元素
 *
 * @param list 双向链表容器指针
 * @param index 元素索引
 * @param element 输出参数，存储元素指针
 * @return error_code_t 错误码
 */
error_code_t list_at(const list_t* list, size_t index, void** element)
{
    if (list == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    /* 检查索引是否有效 */
    if (index >= list->size) {
        return CSTL_ERROR_INVALID_INDEX;
    }
    
    /* 遍历到指定位置 */
    list_node_t* node = list->head;
    for (size_t i = 0; i < index; i++) {
        node = node->next;
    }
    
    *element = node->data;
    return CSTL_OK;
}

/**
 * @brief 设置双向链表容器指定位置的元素
 *
 * @param list 双向链表容器指针
 * @param index 元素索引
 * @param element 要设置的元素指针
 * @return error_code_t 错误码
 */
error_code_t list_set(list_t* list, size_t index, const void* element)
{
    if (list == NULL || element == NULL) {
        return CSTL_ERROR_NULL_POINTER;
    }
    
    list_lock(list);
    
    /* 检查索引是否有效 */
    if (index >= list->size) {
        list_unlock(list);
        return CSTL_ERROR_INVALID_INDEX;
    }
    
    /* 遍历到指定位置 */
    list_node_t* node = list->head;
    for (size_t i = 0; i < index; i++) {
        node = node->next;
    }
    
    /* 调用析构函数释放旧元素内存 */
    if (list->destructor != NULL) {
        list->destructor(node->data);
    }
    
    /* 设置元素 */
    memcpy(node->data, element, list->element_size);
    
    list_unlock(list);
    return CSTL_OK;
}
