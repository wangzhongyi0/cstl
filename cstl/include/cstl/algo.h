/**
 * @file algo.h
 * @brief CSTL库的算法模块头文件
 * 
 * 该文件定义了CSTL库的算法模块，包括排序、查找、
 * 比较和转换等通用算法。
 */

#ifndef CSTL_ALGO_H
#define CSTL_ALGO_H

#include "cstl/common.h"
#include "cstl/iterator.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 比较函数指针类型
 * 
 * @param a 第一个元素指针
 * @param b 第二个元素指针
 * @return int 如果a < b返回负数，如果a == b返回0，如果a > b返回正数
 */
typedef int (*compare_fn_t)(const void* a, const void* b);

/**
 * @brief 谓词函数指针类型
 * 
 * @param element 元素指针
 * @return int 如果满足条件返回非零，否则返回零
 */
typedef int (*predicate_fn_t)(const void* element);

/**
 * @brief 一元操作函数指针类型
 * 
 * @param element 元素指针
 */
typedef void (*unary_op_fn_t)(void* element);

/**
 * @brief 二元操作函数指针类型
 * 
 * @param a 第一个元素指针
 * @param b 第二个元素指针
 */
typedef void (*binary_op_fn_t)(void* a, const void* b);

/**
 * @brief 排序算法枚举
 */
typedef enum {
    SORT_QUICK = 0,    /**< 快速排序 */
    SORT_MERGE = 1,    /**< 归并排序 */
    SORT_HEAP = 2,     /**< 堆排序 */
    SORT_INSERT = 3    /**< 插入排序 */
} sort_algorithm_t;

/**
 * @brief 对容器进行排序
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param algorithm 排序算法
 * @return error_code_t 错误码
 */
error_code_t algo_sort(iterator_t* begin, iterator_t* end, compare_fn_t compare, sort_algorithm_t algorithm);

/**
 * @brief 对容器进行稳定排序
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @return error_code_t 错误码
 */
error_code_t algo_stable_sort(iterator_t* begin, iterator_t* end, compare_fn_t compare);

/**
 * @brief 检查容器是否已排序
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param is_sorted 输出参数，存储是否已排序
 * @return error_code_t 错误码
 */
error_code_t algo_is_sorted(iterator_t* begin, iterator_t* end, compare_fn_t compare, int* is_sorted);

/**
 * @brief 查找第一个等于给定值的元素
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param value 要查找的值指针
 * @param compare 比较函数指针
 * @param result 输出参数，存储找到的元素指针
 * @return error_code_t 错误码
 */
error_code_t algo_find(iterator_t* begin, iterator_t* end, const void* value, compare_fn_t compare, void** result);

/**
 * @brief 查找第一个满足谓词条件的元素
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param result 输出参数，存储找到的元素指针
 * @return error_code_t 错误码
 */
error_code_t algo_find_if(iterator_t* begin, iterator_t* end, predicate_fn_t predicate, void** result);

/**
 * @brief 查找第一个不满足谓词条件的元素
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param result 输出参数，存储找到的元素指针
 * @return error_code_t 错误码
 */
error_code_t algo_find_if_not(iterator_t* begin, iterator_t* end, predicate_fn_t predicate, void** result);

/**
 * @brief 计算等于给定值的元素数量
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param value 要查找的值指针
 * @param compare 比较函数指针
 * @param count 输出参数，存储元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_count(iterator_t* begin, iterator_t* end, const void* value, compare_fn_t compare, size_t* count);

/**
 * @brief 计算满足谓词条件的元素数量
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param count 输出参数，存储元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_count_if(iterator_t* begin, iterator_t* end, predicate_fn_t predicate, size_t* count);

/**
 * @brief 检查是否所有元素都满足谓词条件
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param result 输出参数，存储检查结果
 * @return error_code_t 错误码
 */
error_code_t algo_all_of(iterator_t* begin, iterator_t* end, predicate_fn_t predicate, int* result);

/**
 * @brief 检查是否存在元素满足谓词条件
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param result 输出参数，存储检查结果
 * @return error_code_t 错误码
 */
error_code_t algo_any_of(iterator_t* begin, iterator_t* end, predicate_fn_t predicate, int* result);

/**
 * @brief 检查是否所有元素都不满足谓词条件
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param result 输出参数，存储检查结果
 * @return error_code_t 错误码
 */
error_code_t algo_none_of(iterator_t* begin, iterator_t* end, predicate_fn_t predicate, int* result);

/**
 * @brief 对容器中的每个元素应用一元操作
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param op 一元操作函数指针
 * @return error_code_t 错误码
 */
error_code_t algo_for_each(iterator_t* begin, iterator_t* end, unary_op_fn_t op);

/**
 * @brief 查找两个相邻的相等元素
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储找到的第一个元素指针
 * @return error_code_t 错误码
 */
error_code_t algo_adjacent_find(iterator_t* begin, iterator_t* end, compare_fn_t compare, void** result);

/**
 * @brief 查找第一个在第二个范围中出现的元素
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param end2 第二个范围的结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储找到的元素指针
 * @return error_code_t 错误码
 */
error_code_t algo_find_first_of(iterator_t* begin1, iterator_t* end1, 
                               iterator_t* begin2, iterator_t* end2, 
                               compare_fn_t compare, void** result);

/**
 * @brief 查找第一个不在第二个范围中出现的元素
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param end2 第二个范围的结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储找到的元素指针
 * @return error_code_t 错误码
 */
error_code_t algo_find_first_not_of(iterator_t* begin1, iterator_t* end1, 
                                    iterator_t* begin2, iterator_t* end2, 
                                    compare_fn_t compare, void** result);

/**
 * @brief 检查两个范围是否相等
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param compare 比较函数指针
 * @param equal 输出参数，存储是否相等
 * @return error_code_t 错误码
 */
error_code_t algo_equal(iterator_t* begin1, iterator_t* end1, 
                       iterator_t* begin2, compare_fn_t compare, int* equal);

/**
 * @brief 检查第一个范围是否是第二个范围的前缀
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param end2 第二个范围的结束迭代器
 * @param compare 比较函数指针
 * @param is_prefix 输出参数，存储是否是前缀
 * @return error_code_t 错误码
 */
error_code_t algo_starts_with(iterator_t* begin1, iterator_t* end1, 
                              iterator_t* begin2, iterator_t* end2, 
                              compare_fn_t compare, int* is_prefix);

/**
 * @brief 检查第一个范围是否是第二个范围的后缀
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param end2 第二个范围的结束迭代器
 * @param compare 比较函数指针
 * @param is_suffix 输出参数，存储是否是后缀
 * @return error_code_t 错误码
 */
error_code_t algo_ends_with(iterator_t* begin1, iterator_t* end1, 
                            iterator_t* begin2, iterator_t* end2, 
                            compare_fn_t compare, int* is_suffix);

/**
 * @brief 查找第一个子范围
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 子范围的起始迭代器
 * @param end2 子范围的结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储找到的子范围起始位置
 * @return error_code_t 错误码
 */
error_code_t algo_search(iterator_t* begin1, iterator_t* end1, 
                        iterator_t* begin2, iterator_t* end2, 
                        compare_fn_t compare, void** result);

/**
 * @brief 查找最后一个子范围
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 子范围的起始迭代器
 * @param end2 子范围的结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储找到的子范围起始位置
 * @return error_code_t 错误码
 */
error_code_t algo_find_end(iterator_t* begin1, iterator_t* end1, 
                           iterator_t* begin2, iterator_t* end2, 
                           compare_fn_t compare, void** result);

/**
 * @brief 将源范围复制到目标范围
 * 
 * @param begin 源范围的起始迭代器
 * @param end 源范围的结束迭代器
 * @param dest 目标范围的起始迭代器
 * @param count 输出参数，存储复制的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_copy(iterator_t* begin, iterator_t* end, iterator_t* dest, size_t* count);

/**
 * @brief 将源范围反向复制到目标范围
 * 
 * @param begin 源范围的起始迭代器
 * @param end 源范围的结束迭代器
 * @param dest 目标范围的结束迭代器
 * @param count 输出参数，存储复制的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_copy_backward(iterator_t* begin, iterator_t* end, iterator_t* dest, size_t* count);

/**
 * @brief 将满足谓词条件的元素复制到目标范围
 * 
 * @param begin 源范围的起始迭代器
 * @param end 源范围的结束迭代器
 * @param dest 目标范围的起始迭代器
 * @param predicate 谓词函数指针
 * @param count 输出参数，存储复制的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_copy_if(iterator_t* begin, iterator_t* end, iterator_t* dest, 
                          predicate_fn_t predicate, size_t* count);

/**
 * @brief 交换两个元素的值
 * 
 * @param a 第一个元素指针
 * @param b 第二个元素指针
 * @param size 元素大小
 */
void algo_swap(void* a, void* b, size_t size);

/**
 * @brief 交换两个范围的元素
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param count 输出参数，存储交换的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_swap_ranges(iterator_t* begin1, iterator_t* end1, 
                              iterator_t* begin2, size_t* count);

/**
 * @brief 将元素转换为目标范围
 * 
 * @param begin 源范围的起始迭代器
 * @param end 源范围的结束迭代器
 * @param dest 目标范围的起始迭代器
 * @param op 一元操作函数指针
 * @param count 输出参数，存储转换的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_transform(iterator_t* begin, iterator_t* end, iterator_t* dest, 
                            unary_op_fn_t op, size_t* count);

/**
 * @brief 将两个范围的元素转换为目标范围
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param dest 目标范围的起始迭代器
 * @param op 二元操作函数指针
 * @param count 输出参数，存储转换的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_transform_binary(iterator_t* begin1, iterator_t* end1, 
                                   iterator_t* begin2, iterator_t* dest, 
                                   binary_op_fn_t op, size_t* count);

/**
 * @brief 替换等于给定值的元素
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param old_value 旧值指针
 * @param new_value 新值指针
 * @param compare 比较函数指针
 * @param count 输出参数，存储替换的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_replace(iterator_t* begin, iterator_t* end, 
                         const void* old_value, const void* new_value, 
                         compare_fn_t compare, size_t* count);

/**
 * @brief 替换满足谓词条件的元素
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param new_value 新值指针
 * @param count 输出参数，存储替换的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_replace_if(iterator_t* begin, iterator_t* end, 
                             predicate_fn_t predicate, const void* new_value, 
                             size_t* count);

/**
 * @brief 将满足谓词条件的元素复制到目标范围，并从源范围中移除
 * 
 * @param begin 源范围的起始迭代器
 * @param end 源范围的结束迭代器
 * @param dest 目标范围的起始迭代器
 * @param predicate 谓词函数指针
 * @param count 输出参数，存储移动的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_remove_copy_if(iterator_t* begin, iterator_t* end, iterator_t* dest, 
                                 predicate_fn_t predicate, size_t* count);

/**
 * @brief 填充范围
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param value 填充值指针
 * @param count 输出参数，存储填充的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_fill(iterator_t* begin, iterator_t* end, const void* value, size_t* count);

/**
 * @brief 填充指定数量的元素
 * 
 * @param begin 起始迭代器
 * @param count 填充数量
 * @param value 填充值指针
 * @return error_code_t 错误码
 */
error_code_t algo_fill_n(iterator_t* begin, size_t count, const void* value);

/**
 * @brief 生成范围
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param generator 生成函数指针
 * @param count 输出参数，存储生成的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_generate(iterator_t* begin, iterator_t* end, 
                           void (*generator)(void*), size_t* count);

/**
 * @brief 生成指定数量的元素
 * 
 * @param begin 起始迭代器
 * @param count 生成数量
 * @param generator 生成函数指针
 * @return error_code_t 错误码
 */
error_code_t algo_generate_n(iterator_t* begin, size_t count, 
                             void (*generator)(void*));

/**
 * @brief 移除连续的重复元素
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param count 输出参数，存储移除的元素数量
 * @return error_code_t 错误码
 */
error_code_t algo_unique(iterator_t* begin, iterator_t* end, 
                         compare_fn_t compare, size_t* count);

/**
 * @brief 反转范围
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @return error_code_t 错误码
 */
error_code_t algo_reverse(iterator_t* begin, iterator_t* end);

/**
 * @brief 旋转范围
 * 
 * @param begin 起始迭代器
 * @param middle 旋转点迭代器
 * @param end 结束迭代器
 * @return error_code_t 错误码
 */
error_code_t algo_rotate(iterator_t* begin, iterator_t* middle, iterator_t* end);

/**
 * @brief 随机打乱范围
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @return error_code_t 错误码
 */
error_code_t algo_shuffle(iterator_t* begin, iterator_t* end);

/**
 * @brief 对范围进行分区
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param result 输出参数，存储分区点迭代器
 * @return error_code_t 错误码
 */
error_code_t algo_partition(iterator_t* begin, iterator_t* end, 
                           predicate_fn_t predicate, void** result);

/**
 * @brief 检查范围是否已分区
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param predicate 谓词函数指针
 * @param is_partitioned 输出参数，存储是否已分区
 * @return error_code_t 错误码
 */
error_code_t algo_is_partitioned(iterator_t* begin, iterator_t* end, 
                               predicate_fn_t predicate, int* is_partitioned);

/**
 * @brief 计算范围的最小值
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储最小值指针
 * @return error_code_t 错误码
 */
error_code_t algo_min_element(iterator_t* begin, iterator_t* end, 
                             compare_fn_t compare, void** result);

/**
 * @brief 计算范围的最大值
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储最大值指针
 * @return error_code_t 错误码
 */
error_code_t algo_max_element(iterator_t* begin, iterator_t* end, 
                             compare_fn_t compare, void** result);

/**
 * @brief 计算范围的最小值和最大值
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param min_result 输出参数，存储最小值指针
 * @param max_result 输出参数，存储最大值指针
 * @return error_code_t 错误码
 */
error_code_t algo_minmax_element(iterator_t* begin, iterator_t* end, 
                                compare_fn_t compare, void** min_result, void** max_result);

/**
 * @brief 计算两个范围的字典序最小值
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param end2 第二个范围的结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储最小值范围（1或2）
 * @return error_code_t 错误码
 */
error_code_t algo_lexicographical_compare(iterator_t* begin1, iterator_t* end1, 
                                         iterator_t* begin2, iterator_t* end2, 
                                         compare_fn_t compare, int* result);

/**
 * @brief 检查是否一个范围是另一个范围的排列
 * 
 * @param begin1 第一个范围的起始迭代器
 * @param end1 第一个范围的结束迭代器
 * @param begin2 第二个范围的起始迭代器
 * @param end2 第二个范围的结束迭代器
 * @param compare 比较函数指针
 * @param is_permutation 输出参数，存储是否是排列
 * @return error_code_t 错误码
 */
error_code_t algo_is_permutation(iterator_t* begin1, iterator_t* end1, 
                                iterator_t* begin2, iterator_t* end2, 
                                compare_fn_t compare, int* is_permutation);

/**
 * @brief 生成下一个排列
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储是否生成了下一个排列
 * @return error_code_t 错误码
 */
error_code_t algo_next_permutation(iterator_t* begin, iterator_t* end, 
                                  compare_fn_t compare, int* result);

/**
 * @brief 生成上一个排列
 * 
 * @param begin 起始迭代器
 * @param end 结束迭代器
 * @param compare 比较函数指针
 * @param result 输出参数，存储是否生成了上一个排列
 * @return error_code_t 错误码
 */
error_code_t algo_prev_permutation(iterator_t* begin, iterator_t* end, 
                                  compare_fn_t compare, int* result);

#ifdef __cplusplus
}
#endif

#endif /* CSTL_ALGO_H */