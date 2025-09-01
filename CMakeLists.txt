cmake_minimum_required(VERSION 3.10)
project(CSTL C)

# 设置C标准
set(CMAKE_C_STANDARD 99)

# 包含目录
include_directories(cstl/include cstl/examples/common)

# 源文件
set(SOURCES
    cstl/src/common.c
    cstl/src/iterator.c
    cstl/src/vector.c
    cstl/src/list.c
    cstl/src/stack.c
    cstl/src/queue.c
    cstl/src/algo.c
    "./cstl/examples/common/utils.c"
)

# 创建静态库
add_library(cstl STATIC ${SOURCES})

# 测试案例
add_executable(vector_test cstl/examples/vector_test.c)
target_link_libraries(vector_test cstl)

add_executable(thread_safe_test cstl/examples/thread_safe_test.c)
target_link_libraries(thread_safe_test cstl)

add_executable(pool_performance_test cstl/examples/pool_performance_test.c)
target_link_libraries(pool_performance_test cstl)

add_executable(queue_test cstl/examples/queue_test.c)
target_link_libraries(queue_test cstl)


# 在非Windows系统上链接pthread库
if(UNIX)
    target_link_libraries(vector_test pthread)
    target_link_libraries(thread_safe_test pthread)
    target_link_libraries(pool_performance_test pthread)
    target_link_libraries(queue_test pthread)
endif()

# 设置输出目录
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/cstl/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/cstl/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/cstl/bin)

# 安装规则
install(TARGETS cstl vector_test thread_safe_test pool_performance_test queue_test
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib)

install(DIRECTORY cstl/include/cstl DESTINATION include)