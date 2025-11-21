cmake_minimum_required(VERSION 3.10)

# 项目名称
project(FreeMDict CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加构建类型选项，默认为Release
option(BUILD_TYPE "选择构建类型: Debug 或 Release" Release)

# 添加SMALL_MEMORY选项，默认为OFF
option(SMALL_MEMORY "启用小内存模式" OFF)


# 添加包含目录（支持全路径include）
include_directories(${CMAKE_SOURCE_DIR})

# 添加源文件（排除main.cpp用于库构建）
file(GLOB_RECURSE LIBRARY_SOURCES "${CMAKE_SOURCE_DIR}/src/*.cpp")
list(REMOVE_ITEM LIBRARY_SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")

# 创建静态库
add_library(free_mdict_static STATIC ${LIBRARY_SOURCES})
set_target_properties(free_mdict_static PROPERTIES OUTPUT_NAME "free_mdict")

# 设置静态库输出到lib文件夹
set_target_properties(free_mdict_static PROPERTIES ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib")

# 设置库的头文件
set_target_properties(free_mdict_static PROPERTIES PUBLIC_HEADER "${CMAKE_SOURCE_DIR}/include/extern/mdict_extern.h")

# 创建可执行文件
add_executable(free_mdict "${CMAKE_SOURCE_DIR}/src/main.cpp")


# 如果启用了SMALL_MEMORY，则定义相应的宏
if(SMALL_MEMORY)
    message(STATUS "当前SMALL_MEMORY选项值: ${SMALL_MEMORY}")
    target_compile_definitions(free_mdict PRIVATE SMALL_MEMORY)
    target_compile_definitions(free_mdict_static PRIVATE SMALL_MEMORY)
    message(STATUS "已启用小内存模式: SMALL_MEMORY")
else()
    message(STATUS "未启用小内存模式")
endif()

# 根据选项设置CMAKE_BUILD_TYPE
if(BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_BUILD_TYPE Debug)
    message(STATUS "构建模式: Debug")
    target_compile_definitions(free_mdict PRIVATE DEBUG)
    target_compile_definitions(free_mdict_static PRIVATE DEBUG)
elseif(BUILD_TYPE STREQUAL "Release")
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "构建模式: Release")
    target_compile_definitions(free_mdict PRIVATE NDEBUG)
    target_compile_definitions(free_mdict_static PRIVATE NDEBUG)
else()
    message(WARNING "无效的构建类型: ${BUILD_TYPE}，默认使用Release模式")
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "构建模式: Release (默认)")
    target_compile_definitions(free_mdict PRIVATE NDEBUG)
    target_compile_definitions(free_mdict_static PRIVATE NDEBUG)
endif()

# 在全局编译标志中添加AddressSanitizer选项
#set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -fsanitize=address -fno-omit-frame-pointer")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -g -Wno-deprecated-declarations")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3 -Wno-deprecated-declarations")

# 查找zlib库
find_package(ZLIB REQUIRED)
if(ZLIB_FOUND)
    include_directories(${ZLIB_INCLUDE_DIRS})
    # 移除PRIVATE关键字以保持与其他链接调用的一致性
    target_link_libraries(free_mdict ${ZLIB_LIBRARIES})
    target_link_libraries(free_mdict_static ${ZLIB_LIBRARIES})
    message(STATUS "已找到并链接zlib库: ${ZLIB_LIBRARIES}")
else()
    message(FATAL_ERROR "未找到zlib库，请先安装zlib")
endif()

# 在macOS上，使用Homebrew安装的库可能在非标准路径，添加查找路径
if(APPLE)
    # 设置可能的gflags安装路径（Homebrew默认安装路径）
    set(GFLAGS_ROOT_DIR /usr/local/opt/gflags CACHE PATH "gflags安装路径")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${GFLAGS_ROOT_DIR})
    # 也可以直接指定库文件路径
    find_library(GFLAGS_LIBRARY gflags HINTS ${GFLAGS_ROOT_DIR}/lib)
    find_path(GFLAGS_INCLUDE_DIR gflags/gflags.h HINTS ${GFLAGS_ROOT_DIR}/include)
    
    if(GFLAGS_LIBRARY AND GFLAGS_INCLUDE_DIR)
        set(GFLAGS_FOUND TRUE)
        set(GFLAGS_LIBRARIES ${GFLAGS_LIBRARY})
        set(GFLAGS_INCLUDE_DIRS ${GFLAGS_INCLUDE_DIR})
        message(STATUS "在macOS上找到gflags库: ${GFLAGS_LIBRARY}")
        message(STATUS "gflags包含目录: ${GFLAGS_INCLUDE_DIR}")
    endif()
else()
    # 非macOS系统，尝试使用标准查找方式
    find_package(gflags REQUIRED)
endif()

# 如果前面的尝试失败，尝试使用pkg-config
if(NOT GFLAGS_FOUND)
    find_package(PkgConfig QUIET)
    if(PKG_CONFIG_FOUND)
        pkg_check_modules(GFLAGS_PKG gflags)
        if(GFLAGS_PKG_FOUND)
            set(GFLAGS_FOUND TRUE)
            set(GFLAGS_INCLUDE_DIRS ${GFLAGS_PKG_INCLUDE_DIRS})
            set(GFLAGS_LIBRARIES ${GFLAGS_PKG_LIBRARIES})
            message(STATUS "通过pkg-config找到gflags库")
        endif()
    endif()
endif()

# 如果仍然找不到，尝试直接查找库文件和头文件
if(NOT GFLAGS_FOUND)
    find_library(GFLAGS_LIBRARY NAMES gflags)
    find_path(GFLAGS_INCLUDE_DIR gflags/gflags.h)
    
    if(GFLAGS_LIBRARY AND GFLAGS_INCLUDE_DIR)
        set(GFLAGS_FOUND TRUE)
        set(GFLAGS_LIBRARIES ${GFLAGS_LIBRARY})
        set(GFLAGS_INCLUDE_DIRS ${GFLAGS_INCLUDE_DIR})
        message(STATUS "直接找到gflags库: ${GFLAGS_LIBRARY}")
        message(STATUS "gflags包含目录: ${GFLAGS_INCLUDE_DIR}")
    endif()
endif()

# 检查gflags是否找到
if(GFLAGS_FOUND)
    include_directories(${GFLAGS_INCLUDE_DIRS})
    target_link_libraries(free_mdict ${GFLAGS_LIBRARIES})
    target_link_libraries(free_mdict_static ${GFLAGS_LIBRARIES})
    message(STATUS "已链接gflags库")
else()
    message(WARNING "未找到gflags库，尝试使用系统路径")
    # 最后尝试一次标准查找，但不报错
    find_package(gflags QUIET)
    if(gflags_FOUND)
        include_directories(${gflags_INCLUDE_DIRS})
        target_link_libraries(free_mdict ${gflags_LIBRARIES})
        target_link_libraries(free_mdict_static ${gflags_LIBRARIES})
        message(STATUS "已找到并链接gflags库")
    else()
        message(FATAL_ERROR "未找到gflags库，请先安装gflags")
    endif()
endif()

# 链接可执行文件到我们的静态库
target_link_libraries(free_mdict free_mdict_static)

# 安装配置
install(TARGETS free_mdict_static
    ARCHIVE DESTINATION free_mdict/lib
    PUBLIC_HEADER DESTINATION free_mdict/include
)

install(TARGETS free_mdict
    RUNTIME DESTINATION free_mdict/bin
)