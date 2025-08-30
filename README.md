# CSTL (C Standard Template Library)

CSTL 是一个用 C 语言实现的标准模板库，提供了类似于 C++ STL 的数据结构和算法功能。该库旨在为 C 开发者提供高效、可靠且易于使用的数据结构和算法实现。

## 特性

- **多种容器实现**：包括向量、双向链表、栈和队列等常用数据结构
- **算法模块**：提供排序、查找、变换等多种算法实现
- **迭代器框架**：统一的迭代器接口，便于遍历容器元素
- **线程安全支持**：可选的线程安全功能，支持多线程环境
- **内存池优化**：内置内存池和对象池，提高内存分配性能
- **跨平台支持**：支持 Windows、Linux 和 macOS 等多种平台

## 目录结构

```
cstl/
├── include/           # 头文件目录
│   └── cstl/         # CSTL 库头文件
│       ├── common.h   # 基础架构模块
│       ├── iterator.h # 迭代器框架
│       ├── vector.h   # 向量容器
│       ├── list.h     # 双向链表容器
│       ├── stack.h    # 栈适配器
│       ├── queue.h    # 队列适配器
│       ├── algo.h     # 算法模块
│       └── cstl.h     # 主头文件
├── src/              # 源文件目录
│   ├── common.c      # 基础架构模块实现
│   ├── iterator.c    # 迭代器框架实现
│   ├── vector.c      # 向量容器实现
│   ├── list.c        # 双向链表容器实现
│   ├── stack.c       # 栈适配器实现
│   ├── queue.c       # 队列适配器实现
│   └── algo.c        # 算法模块实现
├── examples/         # 示例程序
│   ├── basic_test.c          # 基本功能测试示例
│   ├── thread_safe_test.c    # 线程安全测试示例
│   └── pool_performance_test.c # 内存池和对象池性能测试示例
└── tests/            # 测试文件
```

## 构建和安装

### 前提条件

- C 编译器（如 GCC、Clang 等）
- Make 工具
- pthread 库（用于线程安全功能）

### 构建步骤

1. 克隆或下载项目代码
2. 进入项目根目录
3. 运行 `make` 命令构建库和示例程序

```bash
make
```

### 构建选项

- `make all`：构建静态库和示例程序（默认）
- `make static_lib`：仅构建静态库
- `make dynamic_lib`：仅构建动态库
- `make examples`：仅构建示例程序
- `make debug`：构建调试版本
- `make release`：构建发布版本
- `make clean`：清理生成的文件
- `make install`：安装库到系统目录
- `make uninstall`：从系统目录卸载库

### 运行测试

```bash
make test              # 运行基本功能测试
make test_thread_safe  # 运行线程安全测试
make test_pool_performance # 运行内存池和对象池性能测试
make test_all          # 运行所有测试
```

## 使用示例

### 基本使用

```c
#include "cstl.h"
#include <stdio.h>

int main() {
    // 创建一个整数向量
    vector_t* vec = vector_create(sizeof(int), 0, NULL);
    
    // 添加元素
    for (int i = 0; i < 10; i++) {
        vector_push_back(vec, &i);
    }
    
    // 访问元素
    int* value;
    for (size_t i = 0; i < vector_size(vec); i++) {
        vector_at(vec, i, (void**)&value);
        printf("%d ", *value);
    }
    printf("\n");
    
    // 销毁向量
    vector_destroy(vec);
    
    return 0;
}
```

### 线程安全使用

```c
#include "cstl.h"
#include <stdio.h>

int main() {
    // 创建一个线程安全的整数向量
    vector_t* vec = vector_create(sizeof(int), 0, NULL);
    vector_enable_thread_safety(vec);
    
    // 在多线程环境中安全使用...
    
    // 销毁向量
    vector_destroy(vec);
    
    return 0;
}
```

### 内存池优化使用

```c
#include "cstl.h"
#include <stdio.h>

int main() {
    // 创建内存池
    mem_pool_t* pool = mem_pool_create(sizeof(int), 1000, NULL);
    
    // 创建向量并设置内存池
    vector_t* vec = vector_create(sizeof(int), 0, NULL);
    vector_set_memory_pool(vec, pool);
    
    // 使用向量，内存分配将通过内存池进行...
    
    // 销毁向量和内存池
    vector_destroy(vec);
    mem_pool_destroy(pool);
    
    return 0;
}
```

## API 文档

### 容器

#### 向量 (vector)

动态数组实现，支持随机访问和在末尾高效插入/删除元素。

主要函数：
- `vector_create()` - 创建向量
- `vector_destroy()` - 销毁向量
- `vector_push_back()` - 在末尾添加元素
- `vector_pop_back()` - 移除末尾元素
- `vector_at()` - 访问指定位置的元素
- `vector_size()` - 获取元素数量
- `vector_capacity()` - 获取容量
- `vector_reserve()` - 预留容量
- `vector_resize()` - 调整大小

#### 双向链表 (list)

双向链表实现，支持在任何位置高效插入/删除元素。

主要函数：
- `list_create()` - 创建链表
- `list_destroy()` - 销毁链表
- `list_push_front()` - 在前端添加元素
- `list_push_back()` - 在后端添加元素
- `list_pop_front()` - 移除前端元素
- `list_pop_back()` - 移除后端元素
- `list_insert()` - 在指定位置插入元素
- `list_erase()` - 移除指定位置的元素
- `list_size()` - 获取元素数量

#### 栈 (stack)

基于向量实现的栈适配器，提供后进先出(LIFO)的数据结构操作。

主要函数：
- `stack_create()` - 创建栈
- `stack_destroy()` - 销毁栈
- `stack_push()` - 入栈操作
- `stack_pop()` - 出栈操作
- `stack_top()` - 获取栈顶元素
- `stack_size()` - 获取元素数量
- `stack_empty()` - 检查是否为空

#### 队列 (queue)

基于双向链表实现的队列适配器，提供先进先出(FIFO)的数据结构操作。

主要函数：
- `queue_create()` - 创建队列
- `queue_destroy()` - 销毁队列
- `queue_push()` - 入队操作
- `queue_pop()` - 出队操作
- `queue_front()` - 获取队首元素
- `queue_back()` - 获取队尾元素
- `queue_size()` - 获取元素数量
- `queue_empty()` - 检查是否为空

### 算法

#### 排序算法

- `algo_sort()` - 排序算法，支持多种排序方式（快速排序、归并排序、堆排序、插入排序）

#### 查找算法

- `algo_find()` - 查找指定元素
- `algo_find_if()` - 查找满足条件的第一个元素

#### 变换算法

- `algo_for_each()` - 对每个元素执行指定操作
- `algo_transform()` - 变换每个元素
- `algo_fill()` - 用指定值填充元素
- `algo_generate()` - 生成元素

#### 其他算法

- `algo_count()` - 计算指定元素的数量
- `algo_count_if()` - 计算满足条件的元素数量
- `algo_all_of()` - 检查是否所有元素都满足条件
- `algo_any_of()` - 检查是否有元素满足条件
- `algo_none_of()` - 检查是否没有元素满足条件
- `algo_min_element()` - 查找最小元素
- `algo_max_element()` - 查找最大元素
- `algo_minmax_element()` - 查找最小和最大元素
- `algo_reverse()` - 反转元素顺序
- `algo_rotate()` - 旋转元素顺序
- `algo_shuffle()` - 随机打乱元素顺序
- `algo_partition()` - 分割元素
- `algo_unique()` - 移除重复元素
- `algo_replace()` - 替换指定元素
- `algo_swap()` - 交换两个元素
- `algo_swap_ranges()` - 交换两个范围内的元素

### 内存管理

#### 内存池

- `mem_pool_create()` - 创建内存池
- `mem_pool_destroy()` - 销毁内存池
- `mem_pool_alloc()` - 从内存池分配内存
- `mem_pool_free()` - 释放内存回内存池
- `mem_pool_get_stats()` - 获取内存池统计信息

#### 对象池

- `obj_pool_create()` - 创建对象池
- `obj_pool_destroy()` - 销毁对象池
- `obj_pool_alloc()` - 从对象池分配对象
- `obj_pool_free()` - 释放对象回对象池
- `obj_pool_get_stats()` - 获取对象池统计信息

## 线程安全

CSTL 库支持可选的线程安全功能。要启用线程安全，可以使用以下函数：

- `vector_enable_thread_safety()` - 启用向量的线程安全
- `list_enable_thread_safety()` - 启用链表的线程安全
- `stack_enable_thread_safety()` - 启用栈的线程安全
- `queue_enable_thread_safety()` - 启用队列的线程安全

类似地，可以使用以下函数禁用线程安全：

- `vector_disable_thread_safety()` - 禁用向量的线程安全
- `list_disable_thread_safety()` - 禁用链表的线程安全
- `stack_disable_thread_safety()` - 禁用栈的线程安全
- `queue_disable_thread_safety()` - 禁用队列的线程安全

## 性能优化

### 内存池优化

CSTL 库提供了内存池功能，可以显著提高内存分配和释放的性能。要使用内存池优化，可以按照以下步骤操作：

1. 创建内存池：`mem_pool_create()`
2. 为容器设置内存池：`vector_set_memory_pool()`
3. 正常使用容器
4. 销毁内存池：`mem_pool_destroy()`

### 对象池优化

对于频繁创建和销毁的对象，可以使用对象池来提高性能：

1. 创建对象池：`obj_pool_create()`
2. 为容器设置对象池：`list_set_node_pool()`
3. 正常使用容器
4. 销毁对象池：`obj_pool_destroy()`




## 版本历史

- 1.0.0
  - 初始版本
  - 实现基本的容器和算法
  - 支持线程安全和内存池优化

## 联系方式

如有任何问题或建议，请通过以下方式联系：

- 邮箱：[1340130745@qq.com]
- 项目主页：[https://github.com/wangzhongyi0/cstl/tree/master]

