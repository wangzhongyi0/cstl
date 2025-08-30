# Windows平台编译指南

本文档提供了在Windows平台上编译和运行CSTL库及其测试案例的详细说明。

## 前提条件

1. **C编译器**：需要安装C编译器，推荐以下选项之一：
   - [MinGW-w64](http://mingw-w64.org/doku.php) - 包含GCC编译器和GNU工具链
   - [Visual Studio](https://visualstudio.microsoft.com/) - 包含MSVC编译器
   - [Clang for Windows](https://releases.llvm.org/download.html) - LLVM编译器

2. **构建工具**：
   - 如果使用MinGW-w64，可以安装[MSYS2](https://www.msys2.org/)来获取make工具
   - 如果使用Visual Studio，可以使用其内置的构建系统

## 使用MinGW-w64和MSYS2编译

### 安装MinGW-w64和MSYS2

1. 从[MSYS2官网](https://www.msys2.org/)下载并安装MSYS2
2. 打开MSYS2终端，更新包数据库和基础包：
   ```
   pacman -Syu
   ```
3. 安装MinGW-w64工具链和make：
   ```
   pacman -S mingw-w64-x86_64-toolchain make
   ```

### 编译步骤

1. 打开MSYS2 MinGW 64-bit终端
2. 进入项目根目录：
   ```
   cd /path/to/your/project
   ```
3. 创建必要的目录：
   ```
   mkdir -p cstl/lib cstl/obj
   ```
4. 编译所有源文件为目标文件：
   ```
   gcc -c cstl/src/common.c -o cstl/obj/common.o -I cstl/include -Wall -Wextra -std=c99 -O2
   gcc -c cstl/src/iterator.c -o cstl/obj/iterator.o -I cstl/include -Wall -Wextra -std=c99 -O2
   gcc -c cstl/src/vector.c -o cstl/obj/vector.o -I cstl/include -Wall -Wextra -std=c99 -O2
   gcc -c cstl/src/list.c -o cstl/obj/list.o -I cstl/include -Wall -Wextra -std=c99 -O2
   gcc -c cstl/src/stack.c -o cstl/obj/stack.o -I cstl/include -Wall -Wextra -std=c99 -O2
   gcc -c cstl/src/queue.c -o cstl/obj/queue.o -I cstl/include -Wall -Wextra -std=c99 -O2
   gcc -c cstl/src/algo.c -o cstl/obj/algo.o -I cstl/include -Wall -Wextra -std=c99 -O2
   ```
5. 创建静态库：
   ```
   ar rcs cstl/lib/libcstl.a cstl/obj/common.o cstl/obj/iterator.o cstl/obj/vector.o cstl/obj/list.o cstl/obj/stack.o cstl/obj/queue.o cstl/obj/algo.o
   ```
6. 编译测试案例：
   ```
   gcc cstl/examples/basic_test.c -o cstl/examples/basic_test.exe -I cstl/include -L cstl/lib -lcstl -lpthread
   gcc cstl/examples/thread_safe_test.c -o cstl/examples/thread_safe_test.exe -I cstl/include -L cstl/lib -lcstl -lpthread
   gcc cstl/examples/pool_performance_test.c -o cstl/examples/pool_performance_test.exe -I cstl/include -L cstl/lib -lcstl -lpthread
   ```
7. 运行测试案例：
   ```
   ./cstl/examples/basic_test.exe
   ./cstl/examples/thread_safe_test.exe
   ./cstl/examples/pool_performance_test.exe
   ```

## 使用Visual Studio编译

### 创建项目

1. 打开Visual Studio
2. 选择"创建新项目"
3. 选择"静态库(.lib)"项目模板，点击"下一步"
4. 输入项目名称（如"CSTL"），选择保存位置，点击"创建"

### 添加源文件

1. 在解决方案资源管理器中，右键点击"源文件"文件夹，选择"添加" -> "现有项"
2. 浏览并添加以下文件：
   - `cstl/src/common.c`
   - `cstl/src/iterator.c`
   - `cstl/src/vector.c`
   - `cstl/src/list.c`
   - `cstl/src/stack.c`
   - `cstl/src/queue.c`
   - `cstl/src/algo.c`

### 配置项目

1. 右键点击项目，选择"属性"
2. 在"C/C++" -> "常规" -> "附加包含目录"中，添加`cstl/include`
3. 在"C/C++" -> "预处理器" -> "预处理器定义"中，添加`_CRT_SECURE_NO_WARNINGS`（如果需要）
4. 在"链接器" -> "常规" -> "附加库目录"中，添加`cstl/lib`
5. 点击"应用"和"确定"

### 构建库

1. 选择"生成" -> "生成解决方案"或按F7
2. 构建完成后，静态库将生成在项目目录下的`x64/Debug`或`x64/Release`文件夹中

### 创建和编译测试案例

1. 创建新的"控制台应用"项目
2. 在项目中添加测试案例源文件（如`cstl/examples/basic_test.c`）
3. 配置项目属性，添加CSTL库的包含目录和库目录
4. 在"链接器" -> "输入" -> "附加依赖项"中，添加`libcstl.lib`
5. 构建并运行测试案例

## 使用CMake编译（跨平台方案）

### 安装CMake

从[CMake官网](https://cmake.org/download/)下载并安装CMake。

### 创建CMakeLists.txt

在项目根目录创建`CMakeLists.txt`文件：

```cmake
cmake_minimum_required(VERSION 3.10)
project(CSTL C)

# 设置C标准
set(CMAKE_C_STANDARD 99)

# 包含目录
include_directories(cstl/include)

# 源文件
set(SOURCES
    cstl/src/common.c
    cstl/src/iterator.c
    cstl/src/vector.c
    cstl/src/list.c
    cstl/src/stack.c
    cstl/src/queue.c
    cstl/src/algo.c
)

# 创建静态库
add_library(cstl STATIC ${SOURCES})

# 测试案例
add_executable(basic_test cstl/examples/basic_test.c)
target_link_libraries(basic_test cstl)

add_executable(thread_safe_test cstl/examples/thread_safe_test.c)
target_link_libraries(thread_safe_test cstl)

add_executable(pool_performance_test cstl/examples/pool_performance_test.c)
target_link_libraries(pool_performance_test cstl)

# 在Windows上链接pthread库
if(WIN32)
    target_link_libraries(basic_test pthread)
    target_link_libraries(thread_safe_test pthread)
    target_link_libraries(pool_performance_test pthread)
endif()
```

### 编译步骤

1. 打开命令提示符或PowerShell
2. 进入项目根目录
3. 创建构建目录：
   ```
   mkdir build
   cd build
   ```
4. 生成构建系统：
   ```
   cmake ..
   ```
5. 编译项目：
   ```
   cmake --build .
   ```
6. 运行测试案例：
   ```
   Debug\basic_test.exe
   Debug\thread_safe_test.exe
   Debug\pool_performance_test.exe
   ```

## 常见问题

### 1. 找不到pthread库

在Windows上，pthread库不是标准库的一部分。解决方法：

- 使用MinGW-w64时，确保安装了`mingw-w64-x86_64-pthreads`包
- 使用Visual Studio时，可以下载[pthreads for Windows](https://sourceware.org/pthreads-win32/)并配置项目

### 2. 编译错误

如果遇到编译错误，请检查：

- 所有源文件是否已正确添加到项目中
- 包含目录是否正确设置
- 编译器是否支持C99标准

### 3. 链接错误

如果遇到链接错误，请检查：

- 静态库是否正确生成
- 库路径是否正确设置
- 测试案例是否正确链接了静态库

## 总结

在Windows平台上编译CSTL库有多种方法：

1. **使用MinGW-w64和MSYS2**：最接近Linux/Unix环境的方法，可以使用原生的Makefile
2. **使用Visual Studio**：微软官方IDE，提供图形化界面和强大的调试功能
3. **使用CMake**：跨平台构建系统，可以在不同平台上使用相同的构建脚本

根据您的需求和偏好，选择最适合您的方法。如果您习惯使用命令行和Makefile，推荐使用MinGW-w64和MSYS2。如果您更喜欢图形化界面和强大的IDE功能，推荐使用Visual Studio。如果您需要跨平台兼容性，推荐使用CMake。