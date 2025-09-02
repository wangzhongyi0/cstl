# CSTL库 Makefile
# 用于构建CSTL库及其示例程序

# 编译器设置
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2 -I./cstl/include
LDFLAGS = -L./cstl/lib -lpthread -lm
AR = ar
ARFLAGS = rcs

# 目录设置
SRC_DIR = cstl/src
INC_DIR = cstl/include
LIB_DIR = cstl/lib
OBJ_DIR = cstl/obj
EXAMPLE_DIR = cstl/examples
TEST_DIR = cstl/tests

# 目标文件
LIBRARY = $(LIB_DIR)/libcstl.a
STATIC_LIB = $(LIB_DIR)/libcstl.a
DYNAMIC_LIB = $(LIB_DIR)/libcstl.so

# 源文件
COMMON_SRC = $(SRC_DIR)/common.c
ITERATOR_SRC = $(SRC_DIR)/iterator.c
VECTOR_SRC = $(SRC_DIR)/vector.c
LIST_SRC = $(SRC_DIR)/list.c
STACK_SRC = $(SRC_DIR)/stack.c
QUEUE_SRC = $(SRC_DIR)/queue.c
ALGO_SRC = $(SRC_DIR)/algo.c

# 目标文件
COMMON_OBJ = $(OBJ_DIR)/common.o
ITERATOR_OBJ = $(OBJ_DIR)/iterator.o
VECTOR_OBJ = $(OBJ_DIR)/vector.o
LIST_OBJ = $(OBJ_DIR)/list.o
STACK_OBJ = $(OBJ_DIR)/stack.o
QUEUE_OBJ = $(OBJ_DIR)/queue.o
ALGO_OBJ = $(OBJ_DIR)/algo.o

# 所有目标文件
OBJS = $(COMMON_OBJ) $(ITERATOR_OBJ) $(VECTOR_OBJ) $(LIST_OBJ) $(STACK_OBJ) $(QUEUE_OBJ) $(ALGO_OBJ)

# 示例程序
VECTOR_TEST_EXE = $(EXAMPLE_DIR)/vector_test
THREAD_SAFE_TEST_EXE = $(EXAMPLE_DIR)/thread_safe_test
POOL_PERFORMANCE_TEST_EXE = $(EXAMPLE_DIR)/pool_performance_test
SORTING_PERFORMANCE_TEST_EXE = $(EXAMPLE_DIR)/sorting_performance_test

# 默认目标
all: dirs static_lib examples

# 创建目录
dirs:
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(OBJ_DIR)

# 静态库
static_lib: $(STATIC_LIB)

$(STATIC_LIB): $(OBJS)
	@echo "正在创建静态库 $@..."
	@$(AR) $(ARFLAGS) $@ $^

# 动态库
dynamic_lib: $(DYNAMIC_LIB)

$(DYNAMIC_LIB): $(OBJS)
	@echo "正在创建动态库 $@..."
	@$(CC) -shared -o $@ $^

# 目标文件规则
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "正在编译 $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

# 示例程序
examples: $(VECTOR_TEST_EXE) $(THREAD_SAFE_TEST_EXE) $(POOL_PERFORMANCE_TEST_EXE) $(SORTING_PERFORMANCE_TEST_EXE)

$(VECTOR_TEST_EXE): $(EXAMPLE_DIR)/vector_test.c $(STATIC_LIB)
	@echo "正在编译 $@..."
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lcstl

$(THREAD_SAFE_TEST_EXE): $(EXAMPLE_DIR)/thread_safe_test.c $(STATIC_LIB)
	@echo "正在编译 $@..."
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lcstl -lpthread

$(POOL_PERFORMANCE_TEST_EXE): $(EXAMPLE_DIR)/pool_performance_test.c $(STATIC_LIB)
	@echo "正在编译 $@..."
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lcstl

$(SORTING_PERFORMANCE_TEST_EXE): $(EXAMPLE_DIR)/sorting_performance_test.c $(STATIC_LIB)
	@echo "正在编译 $@..."
	@$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -lcstl -lrt

# 安装
install: $(STATIC_LIB)
	@echo "正在安装CSTL库到系统目录..."
	@mkdir -p /usr/local/include/cstl
	@cp -r $(INC_DIR)/cstl /usr/local/include/
	@cp $(STATIC_LIB) /usr/local/lib/
	@cp $(DYNAMIC_LIB) /usr/local/lib/ || true
	@ldconfig || true
	@echo "CSTL库安装完成"

# 卸载
uninstall:
	@echo "正在卸载CSTL库..."
	@rm -rf /usr/local/include/cstl
	@rm -f /usr/local/lib/libcstl.a
	@rm -f /usr/local/lib/libcstl.so
	@ldconfig || true
	@echo "CSTL库卸载完成"

# 清理
clean:
	@echo "正在清理生成的文件..."
	@rm -rf $(LIB_DIR)
	@rm -rf $(OBJ_DIR)
	@rm -f $(VECTOR_TEST_EXE)
	@rm -f $(THREAD_SAFE_TEST_EXE)
	@rm -f $(POOL_PERFORMANCE_TEST_EXE)
	@rm -f $(SORTING_PERFORMANCE_TEST_EXE)
	@rm -f sorting_performance.log
	@echo "清理完成"

# 测试
test: $(VECTOR_TEST_EXE)
	@echo "正在运行vector测试..."
	@$(VECTOR_TEST_EXE)

test_thread_safe: $(THREAD_SAFE_TEST_EXE)
	@echo "正在运行线程安全测试..."
	@$(THREAD_SAFE_TEST_EXE)

test_pool_performance: $(POOL_PERFORMANCE_TEST_EXE)
	@echo "正在运行内存池和对象池性能测试..."
	@$(POOL_PERFORMANCE_TEST_EXE)

test_sorting_performance: $(SORTING_PERFORMANCE_TEST_EXE)
	@echo "正在运行排序算法性能测试..."
	@$(SORTING_PERFORMANCE_TEST_EXE) -r

test_all: test test_thread_safe test_pool_performance test_sorting_performance

# 调试版本
debug: CFLAGS += -g -DDEBUG -O0
debug: dirs static_lib examples

# 发布版本
release: CFLAGS += -O3 -DNDEBUG
release: dirs static_lib examples

# 帮助信息
help:
	@echo "CSTL库 Makefile"
	@echo ""
	@echo "可用目标:"
	@echo "  all          - 构建静态库和示例程序 (默认)"
	@echo "  static_lib   - 构建静态库"
	@echo "  dynamic_lib  - 构建动态库"
	@echo "  examples     - 构建示例程序"
	@echo "  install      - 安装库到系统目录"
	@echo "  uninstall    - 从系统目录卸载库"
	@echo "  clean        - 清理生成的文件"
	@echo "  test         - 运行vector测试"
	@echo "  test_thread_safe - 运行线程安全测试"
	@echo "  test_pool_performance - 运行内存池和对象池性能测试"
	@echo "  test_sorting_performance - 运行排序算法性能测试"
	@echo "  test_all     - 运行所有测试"
	@echo "  debug        - 构建调试版本"
	@echo "  release      - 构建发布版本"
	@echo "  help         - 显示此帮助信息"

.PHONY: all dirs static_lib dynamic_lib examples install uninstall clean \
        test test_thread_safe test_pool_performance test_sorting_performance test_all debug release help
