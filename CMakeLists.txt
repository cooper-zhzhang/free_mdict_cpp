cmake_minimum_required(VERSION 3.10)

# 项目名称
project(FreeMDict CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加构建类型选项，默认为Debug
option(BUILD_TYPE "选择构建类型: Debug 或 Release" Debug)

# 添加SMALL_MEMORY选项，默认为OFF
option(SMALL_MEMORY "启用小内存模式" OFF)

# 根据选项设置CMAKE_BUILD_TYPE
if(BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "构建模式: Debug")
elseif(BUILD_TYPE STREQUAL "Release")
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "构建模式: Release")
else()
    message(WARNING "无效的构建类型: ${BUILD_TYPE}，默认使用Debug模式")
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "构建模式: Debug (默认)")
endif()

# 添加包含目录（支持全路径include）
include_directories(${CMAKE_SOURCE_DIR})

# 添加源文件
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")

# 创建可执行文件
add_executable(free_mdict ${SOURCES})

# 如果启用了SMALL_MEMORY，则定义相应的宏
if(SMALL_MEMORY)
    message(STATUS "当前SMALL_MEMORY选项值: ${SMALL_MEMORY}")
    #target_compile_definitions(free_mdict PRIVATE SMALL_MEMORY)
    message(STATUS "已启用小内存模式: SMALL_MEMORY")
else()
    message(STATUS "未启用小内存模式")
endif()

# 在全局编译标志中添加AddressSanitizer选项
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -fsanitize=address -fno-omit-frame-pointer")
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g -fsanitize=address -fno-omit-frame-pointer")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -g")

# 设置链接器标志
#set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
#set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -fsanitize=address")


# 移除CMAKE_EXE_LINKER_FLAGS的设置，改用target_link_options
message(STATUS "已在全局编译标志中添加AddressSanitizer选项")
message(STATUS "SMALL_MEMORY模式下，AddressSanitizer配置已全局应用")

# 为free_mdict目标添加AddressSanitizer链接选项
#target_compile_options(free_mdict PRIVATE -fsanitize=address)
#target_link_options(free_mdict PRIVATE -fsanitize=address)

#target_compile_options(free_mdict PRIVATE -fsanitize=address -fno-omit-frame-pointer -g)
#target_link_options(free_mdict PRIVATE -fsanitize=address)
# 查找zlib库
find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
    # 移除PRIVATE关键字以保持与其他链接调用的一致性
    target_link_libraries(free_mdict ${ZLIB_LIBRARIES})
    message(STATUS "已找到并链接zlib库: ${ZLIB_LIBRARIES}")
else()
    message(FATAL_ERROR "未找到zlib库，请先安装zlib")
endif()






# 可选：设置编译选项
if(MSVC)
    target_compile_options(free_mdict PRIVATE /W4)
else()
    target_compile_options(free_mdict PRIVATE -Wall -Wextra -pedantic)
endif()