# CSTL (C Standard Template Library)

CSTL 是一个用 C 语言实现的标准模板库，提供了类似于 C++ STL 的数据结构和算法功能。该库旨在为 C 开发者提供高效、可靠且易于使用的数据结构和算法实现。

## 特性

- **多种容器实现**：包括向量、双向链表、栈和队列等常用数据结构
- **算法模块**：提供排序、查找、变换等多种算法实现
- **迭代器框架**：统一的迭代器接口，便于遍历容器元素
- **线程安全支持**：可选的线程安全功能，支持多线程环境
- **内存池优化**：内置内存池和对象池，提高内存分配性能
- **跨平台支持**：支持 Windows、Linux 和 macOS 等多种平台
- **音频数据处理**：专门针对音频数据处理的示例和测试
- **多种排序算法**：支持快速排序、归并排序、堆排序、插入排序等
- **性能测试**：提供全面的性能测试和基准测试工具

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
│   ├── common/       # 通用工具和数据类型
│   │   ├── data_type.h    # 数据类型定义
│   │   ├── utils.c        # 工具函数
│   │   └── utils.h        # 工具函数头文件
│   ├── basic_test.c          # 基本功能测试示例
│   ├── thread_safe_test.c    # 线程安全测试示例
│   ├── pool_performance_test.c # 内存池和对象池性能测试示例
│   ├── vector_test.c         # 向量容器测试
│   └── queue_test.c          # 队列容器和音频数据处理测试
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
make test_queue        # 运行队列和音频数据处理测试
make test_vector       # 运行向量容器测试
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

## 实际应用示例

### 音频数据处理

CSTL 库专门提供了音频数据处理的示例，展示如何在音频处理场景中使用容器和算法：

```c
#include "cstl.h"
#include "data_type.h"

// 创建音频数据队列
queue_t* audio_queue = queue_create(sizeof(stl_audio_pcm), NULL, NULL);

// 处理音频数据
void process_audio_data() {
    // 生成音频PCM数据
    stl_audio_pcm pcm;
    pcm.data = vector_create(sizeof(int16_t), 1024, NULL);
    pcm.time = time(NULL);
    
    // 添加随机音频采样
    for(int i = 0; i < 1024; i++) {
        int16_t sample = random_int64(0, 16000);
        vector_push_back(pcm.data, &sample);
    }
    
    // 使用快速排序对音频数据进行排序
    iterator_t* begin = vector_begin(pcm.data);
    iterator_t* end = vector_end(pcm.data);
    algo_sort(begin, end, compare_a_b, SORT_QUICK);
    
    // 将处理后的数据加入队列
    queue_push(audio_queue, &pcm);
    
    // 清理迭代器
    iterator_destroy(begin);
    iterator_destroy(end);
}
```

### 多种排序算法测试

库提供了完整的排序算法实现和测试：

- **快速排序**：适用于大多数场景的通用排序算法
- **归并排序**：稳定排序，适合大数据集
- **堆排序**：原地排序，内存效率高
- **插入排序**：适合小数据集或基本有序的数据

详细示例请参考 `examples/queue_test.c` 文件。




## 内存管理

### 基础内存管理机制

CSTL库提供了完善的内存管理机制，包括：

1. **析构函数支持**：每个容器都支持自定义析构函数，用于释放元素的动态内存
2. **分配器接口**：支持自定义内存分配器，默认使用标准malloc/free
3. **内存池优化**：提供内存池和对象池，提高频繁分配/释放的性能
4. **线程安全**：支持多线程环境下的安全内存操作

### 析构函数机制

#### 析构函数类型定义
```c
typedef void (*destructor_fn_t)(void* data);
```

#### 析构函数调用时机
- `vector_clear()` - 清空容器时调用每个元素的析构函数
- `vector_destroy()` - 销毁容器时调用每个元素的析构函数
- `list_clear()` - 清空链表时调用每个节点的析构函数
- `list_destroy()` - 销毁链表时调用每个节点的析构函数
- `vector_erase()` - 删除元素时调用该元素的析构函数
- `list_erase()` - 删除节点时调用该节点的析构函数

### 复杂数据结构的内存管理

#### 1. 结构体包含动态分配内存

当容器存储的结构体包含动态分配的内存指针时，**必须**提供自定义析构函数：

```c
typedef struct {
    char* name;        // 动态分配的字符串
    int* scores;       // 动态分配的数组
    size_t score_count;
} student_t;

// 自定义析构函数
void student_destructor(void* data) {
    student_t* student = (student_t*)data;
    if (student->name) {
        free(student->name);
    }
    if (student->scores) {
        free(student->scores);
    }
}

// 创建容器时提供析构函数
vector_t* students = vector_create(sizeof(student_t), 0, NULL, student_destructor);
```

#### 2. 结构体包含其他容器

当结构体包含其他容器指针时，**必须**提供析构函数来释放嵌套容器：

```c
typedef struct {
    vector_t* data;    // 嵌套的向量容器
    time_t timestamp;
} audio_frame_t;

// 嵌套容器的析构函数
void audio_frame_destructor(void* data) {
    audio_frame_t* frame = (audio_frame_t*)data;
    if (frame->data) {
        vector_destroy(frame->data);  // 销毁嵌套的向量
    }
}

// 创建容器时提供析构函数
queue_t* audio_queue = queue_create(sizeof(audio_frame_t), NULL, audio_frame_destructor);
```

#### 3. 多层嵌套容器

对于多层嵌套的复杂结构，需要递归释放：

```c
typedef struct {
    list_t* sub_items;    // 嵌套的链表
    vector_t* tags;       // 嵌套的向量
    char* description;    // 动态字符串
} complex_item_t;

// 多层嵌套的析构函数
void complex_item_destructor(void* data) {
    complex_item_t* item = (complex_item_t*)data;
    if (item->sub_items) {
        list_destroy(item->sub_items);    // 释放嵌套链表
    }
    if (item->tags) {
        vector_destroy(item->tags);       // 释放嵌套向量
    }
    if (item->description) {
        free(item->description);          // 释放动态字符串
    }
}
```

### 内存管理最佳实践

#### 1. 总是提供析构函数

当元素类型包含以下内容时，必须提供析构函数：
- 动态分配的内存指针（malloc/calloc/realloc）
- 其他容器指针
- 文件描述符或系统资源
- 需要特殊清理的资源

#### 2. 析构函数设计原则

```c
// 正确的析构函数设计
void safe_destructor(void* data) {
    if (data == NULL) return;  // 空指针检查
    
    my_type_t* obj = (my_type_t*)data;
    
    // 1. 检查指针是否为NULL再释放
    if (obj->ptr1) {
        free(obj->ptr1);
        obj->ptr1 = NULL;  // 避免悬空指针
    }
    
    // 2. 释放嵌套容器
    if (obj->nested_container) {
        container_destroy(obj->nested_container);
        obj->nested_container = NULL;
    }
    
    // 3. 关闭文件描述符
    if (obj->file_handle != -1) {
        close(obj->file_handle);
        obj->file_handle = -1;
    }
}
```

#### 3. 避免内存泄漏的常见场景

**场景1：队列中的音频数据帧**
```c
// 错误示例：会导致内存泄漏
queue_t* queue = queue_create(sizeof(audio_frame_t), NULL, NULL);

// 正确示例：提供析构函数
queue_t* queue = queue_create(sizeof(audio_frame_t), NULL, audio_frame_destructor);
```

**场景2：向量中的结构体数组**
```c
// 错误示例：结构体中的动态内存不会被释放
vector_t* vec = vector_createsizeof(my_struct_t), 0, NULL, NULL);

// 正确示例：提供完整的析构函数
vector_t* vec = vector_create(sizeof(my_struct_t), 0, NULL, my_struct_destructor);
```

#### 4. 线程安全考虑

```c
// 线程安全的析构函数
void thread_safe_destructor(void* data) {
    my_type_t* obj = (my_type_t*)data;
    
    // 使用原子操作或锁来保护共享资源
    if (obj->shared_resource) {
        pthread_mutex_lock(&obj->mutex);
        release_shared_resource(obj->shared_resource);
        pthread_mutex_unlock(&obj->mutex);
    }
}
```

### 内存池与对象池

#### 内存池使用

```c
// 创建内存池
mem_pool_t* pool = mem_pool_create(sizeof(int), 1000, NULL);

// 创建容器并设置内存池
vector_t* vec = vector_create(sizeof(int), 0, NULL, NULL);
vector_set_memory_pool(vec, pool);

// 使用容器...
vector_destroy(vec);
mem_pool_destroy(pool);
```

#### 对象池使用

```c
// 创建对象池
obj_pool_t* pool = obj_pool_create(sizeof(list_node_t), 100, 50, NULL, NULL);

// 创建链表并设置节点池
list_t* list = list_create(sizeof(my_data_t), NULL, my_data_destructor);
list_set_node_pool(list, pool);

// 使用链表...
list_destroy(list);
obj_pool_destroy(pool);
```

### 内存管理调试

#### 1. 内存泄漏检测

```c
// 在开发阶段可以使用调试版本的分配器
#ifdef DEBUG
    allocator_t* debug_allocator = create_debug_allocator();
    vector_t* vec = vector_create(sizeof(my_type_t), 0, debug_allocator, my_type_destructor);
#else
    vector_t* vec = vector_create(sizeof(my_type_t), 0, NULL, my_type_destructor);
#endif
```

#### 2. 内存使用统计

```c
// 获取内存池统计信息
size_t allocated, free;
mem_pool_get_stats(pool, &allocated, &free);
printf("内存池使用情况: 已分配=%zu, 空闲=%zu\n", allocated, free);
```

### 常见错误及解决方案

#### 1. 忘记提供析构函数

**问题**：容器中的结构体包含动态内存，但没有提供析构函数
**解决**：始终为包含动态内存的结构体提供析构函数

#### 2. 析构函数中的重复释放

**问题**：析构函数试图释放已经释放的内存
**解决**：在析构函数中检查指针是否为NULL，释放后置为NULL

#### 3. 循环引用

**问题**：两个结构体相互引用，导致无法正确释放
**解决**：使用弱引用或手动管理引用计数

#### 4. 线程竞争

**问题**：多线程环境下析构函数被同时调用
**解决**：使用适当的同步机制保护析构函数

### 性能优化建议

1. **对于小型简单类型**：可以不提供析构函数，减少函数调用开销
2. **对于大型复杂结构**：提供完整的析构函数，避免内存泄漏
3. **使用内存池**：对于频繁分配/释放的场景，使用内存池提高性能
4. **批量操作**：尽量使用批量操作而不是逐个元素操作

### 示例：完整的内存管理

```c
#include "cstl.h"

// 定义复杂数据结构
typedef struct {
    char* name;
    vector_t* scores;
    list_t* friends;
    time_t created_at;
} user_profile_t;

// 用户资料的析构函数
void user_profile_destructor(void* data) {
    user_profile_t* profile = (user_profile_t*)data;
    
    // 释放动态分配的字符串
    if (profile->name) {
        free(profile->name);
        profile->name = NULL;
    }
    
    // 释放嵌套的向量容器
    if (profile->scores) {
        vector_destroy(profile->scores);
        profile->scores = NULL;
    }
    
    // 释放嵌套的链表容器
    if (profile->friends) {
        list_destroy(profile->friends);
        profile->friends = NULL;
    }
}

int main() {
    // 创建用户资料向量，提供完整的析构函数
    vector_t* users = vector_create(sizeof(user_profile_t), 0, NULL, user_profile_destructor);
    
    // 启用线程安全（如果需要）
    vector_enable_thread_safety(users);
    
    // 使用用户资料...
    
    // 销毁向量，所有嵌套资源会被自动释放
    vector_destroy(users);
    
    return 0;
}
```

## 版本历史

- 1.0.0 (2025-09-01)
  - 初始版本
  - 实现基本的容器和算法
  - 支持线程安全和内存池优化
  - 新增音频数据处理示例和队列测试
  - 完善多种排序算法实现和性能测试
  - 添加跨平台构建支持（CMake和Makefile）
  - 提供完整的示例程序和测试用例

## 联系方式

如有任何问题或建议，请通过以下方式联系：

- 邮箱：[1340130745@qq.com]
- 项目主页：[https://github.com/wangzhongyi0/cstl/tree/master]

