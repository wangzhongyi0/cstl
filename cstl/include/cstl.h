/**
 * @file cstl.h
 * @brief CSTL库的主头文件
 * 
 * 该文件是CSTL库的主头文件，包含了所有模块的头文件，
 * 用户只需包含此文件即可使用CSTL库的所有功能。
 */

#ifndef CSTL_H
#define CSTL_H

/* 版本信息 */
#define CSTL_MAJOR_VERSION 1
#define CSTL_MINOR_VERSION 0
#define CSTL_PATCH_VERSION 0

/* 将版本号转换为字符串 */
#define CSTL_STRINGIFY(x) #x
#define CSTL_TOSTRING(x) CSTL_STRINGIFY(x)
#define CSTL_VERSION CSTL_TOSTRING(CSTL_MAJOR_VERSION) "." \
                     CSTL_TOSTRING(CSTL_MINOR_VERSION) "." \
                     CSTL_TOSTRING(CSTL_PATCH_VERSION)

/* 包含基础架构模块 */
#include "cstl/common.h"

/* 包含迭代器框架 */
#include "cstl/iterator.h"

/* 包含容器 */
#include "cstl/vector.h"
#include "cstl/list.h"

/* 包含适配器 */
#include "cstl/stack.h"
#include "cstl/queue.h"

/* 包含算法模块 */
#include "cstl/algo.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief 获取CSTL库的版本字符串
 * 
 * @return const char* 版本字符串
 */
static inline const char* cstl_version(void)
{
    return CSTL_VERSION;
}

/**
 * @brief 获取CSTL库的主版本号
 * 
 * @return int 主版本号
 */
static inline int cstl_major_version(void)
{
    return CSTL_MAJOR_VERSION;
}

/**
 * @brief 获取CSTL库的次版本号
 * 
 * @return int 次版本号
 */
static inline int cstl_minor_version(void)
{
    return CSTL_MINOR_VERSION;
}

/**
 * @brief 获取CSTL库的补丁版本号
 * 
 * @return int 补丁版本号
 */
static inline int cstl_patch_version(void)
{
    return CSTL_PATCH_VERSION;
}

#ifdef __cplusplus
}
#endif

#endif /* CSTL_H */