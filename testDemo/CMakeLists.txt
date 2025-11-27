cmake_minimum_required(VERSION 3.10)

# ==============================================================================
# 项目基本信息
# ==============================================================================
project(1D-GF VERSION 1.0.0 LANGUAGES CXX)

# 设置C++标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ==============================================================================
# 构建配置
# ==============================================================================

# 默认构建类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# 编译选项
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

# 输出目录设置
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ==============================================================================
# 依赖查找
# ==============================================================================

# 查找OpenCV
find_package(OpenCV REQUIRED COMPONENTS core imgproc highgui imgcodecs)

# ==============================================================================
# 打印配置信息
# ==============================================================================

message(STATUS "=== Build Configuration ===")
message(STATUS "Project: ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "")
message(STATUS "=== Dependencies ===")
message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV libs: ${OpenCV_LIBS}")

# ==============================================================================
# 可执行文件定义
# ==============================================================================

# 1D导向滤波程序
add_executable(1d_gf main.cpp guided_filter.cpp)

target_include_directories(1d_gf PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(1d_gf PRIVATE ${OpenCV_LIBS})

# ==============================================================================
# 资源文件复制
# ==============================================================================

# 复制1D-GF图像资源
if(EXISTS "${CMAKE_SOURCE_DIR}/1D-GF-MatLab")
    file(COPY ${CMAKE_SOURCE_DIR}/1D-GF-MatLab DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
endif()

# ==============================================================================
# 安装配置
# ==============================================================================

# 安装可执行文件
install(TARGETS 1d_gf
    RUNTIME DESTINATION bin
)

# 安装资源文件
if(EXISTS "${CMAKE_SOURCE_DIR}/1D-GF")
    install(DIRECTORY ${CMAKE_SOURCE_DIR}/1D-GF
        DESTINATION share/${PROJECT_NAME}
    )
endif()

# ==============================================================================
# 构建信息总结
# ==============================================================================

message(STATUS "")
message(STATUS "=== Build Summary ===")
message(STATUS "Target: 1d_gf - 1D Guided Filter denoising program")
message(STATUS "Output directory: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message(STATUS "Install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "")
message(STATUS "Usage:")
message(STATUS "  Build: make")
message(STATUS "  Run: ./bin/1d_gf [image_path]")
message(STATUS "")

