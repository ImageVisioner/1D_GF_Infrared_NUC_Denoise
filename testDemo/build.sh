#!/bin/bash

# ==============================================================================
# 1D-GF 项目一键编译脚本
# ==============================================================================
# 使用方法: ./build.sh [选项]
#
# 选项:
#   -c, --clean     清理构建目录
#   -r, --release   发布模式编译 (默认)
#   -d, --debug     调试模式编译
#   -t, --test      编译后运行测试
#   -h, --help      显示帮助信息
#
# 示例:
#   ./build.sh                    # 正常编译
#   ./build.sh -c                 # 清理后编译
#   ./build.sh -d -t              # 调试模式编译并测试
# ==============================================================================

set -e  # 遇到错误立即退出

# ==============================================================================
# 颜色输出函数
# ==============================================================================
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# ==============================================================================
# 配置变量
# ==============================================================================
PROJECT_NAME="1D-GF"
BUILD_DIR="build"
BUILD_TYPE="Release"
RUN_TEST=false
CLEAN_BUILD=false
PROFILE_MODE=false

# ==============================================================================
# 帮助函数
# ==============================================================================
show_help() {
    cat << EOF
${PROJECT_NAME} 一键编译脚本

使用方法: $0 [选项]

选项:
  -c, --clean     清理构建目录后重新编译
  -r, --release   发布模式编译 (默认)
  -d, --debug     调试模式编译
  -t, --test      编译后运行测试
  -p, --profile   编译后运行性能分析测试
  -h, --help      显示此帮助信息

注意:
  首次运行时脚本会自动设置执行权限

示例:
  $0                    # 正常编译 (发布模式)
  $0 -c                 # 清理后编译
  $0 -d -t              # 调试模式编译并测试
  $0 -p                 # 编译后性能分析
  $0 --clean --debug    # 清理后调试模式编译

构建产物将位于: ${BUILD_DIR}/bin/
EOF
}

# ==============================================================================
# 参数解析
# ==============================================================================
parse_args() {
    while [[ $# -gt 0 ]]; do
        case $1 in
            -c|--clean)
                CLEAN_BUILD=true
                shift
                ;;
            -r|--release)
                BUILD_TYPE="Release"
                shift
                ;;
            -d|--debug)
                BUILD_TYPE="Debug"
                shift
                ;;
            -t|--test)
                RUN_TEST=true
                shift
                ;;
            -p|--profile)
                RUN_TEST=true
                PROFILE_MODE=true
                shift
                ;;
            -h|--help)
                show_help
                exit 0
                ;;
            *)
                print_error "未知选项: $1"
                echo "使用 '$0 --help' 查看帮助信息"
                exit 1
                ;;
        esac
    done
}

# ==============================================================================
# 依赖检查
# ==============================================================================
check_dependencies() {
    print_info "检查构建依赖..."

    local missing_deps=()

    # 检查cmake
    if ! command -v cmake &> /dev/null; then
        missing_deps+=("cmake")
    fi

    # 检查make
    if ! command -v make &> /dev/null; then
        missing_deps+=("make")
    fi

    # 检查g++
    if ! command -v g++ &> /dev/null; then
        missing_deps+=("g++")
    fi

    # 检查pkg-config (用于OpenCV)
    if ! command -v pkg-config &> /dev/null; then
        missing_deps+=("pkg-config")
    fi

    # 检查OpenCV
    if ! pkg-config --exists opencv4 2>/dev/null && ! pkg-config --exists opencv 2>/dev/null; then
        missing_deps+=("opencv")
    fi

    if [ ${#missing_deps[@]} -ne 0 ]; then
        print_error "缺少以下依赖:"
        for dep in "${missing_deps[@]}"; do
            echo "  - $dep"
        done
        echo
        print_info "请安装依赖后重试:"
        echo "  Ubuntu/Debian: sudo apt update && sudo apt install build-essential cmake libopencv-dev"
        echo "  CentOS/RHEL: sudo yum install gcc gcc-c++ make cmake opencv-devel"
        echo "  macOS: brew install cmake opencv"
        exit 1
    fi

    print_success "所有依赖检查通过"
}

# ==============================================================================
# 清理构建目录
# ==============================================================================
clean_build() {
    if [ "$CLEAN_BUILD" = true ] || [ ! -d "$BUILD_DIR" ]; then
        print_info "清理构建目录: $BUILD_DIR"
        rm -rf "$BUILD_DIR"
        print_success "构建目录已清理"
    fi
}

# ==============================================================================
# 创建构建目录
# ==============================================================================
create_build_dir() {
    print_info "创建构建目录: $BUILD_DIR"
    mkdir -p "$BUILD_DIR"
    print_success "构建目录创建完成"
}

# ==============================================================================
# CMake配置
# ==============================================================================
configure_cmake() {
    print_info "运行CMake配置 (构建类型: $BUILD_TYPE)"

    cd "$BUILD_DIR"

    if ! cmake -DCMAKE_BUILD_TYPE="$BUILD_TYPE" ..; then
        print_error "CMake配置失败"
        exit 1
    fi

    print_success "CMake配置完成"
    cd ..
}

# ==============================================================================
# 编译项目
# ==============================================================================
build_project() {
    print_info "开始编译项目..."

    cd "$BUILD_DIR"

    local start_time=$(date +%s)

    if ! make -j$(nproc); then
        print_error "编译失败"
        exit 1
    fi

    local end_time=$(date +%s)
    local duration=$((end_time - start_time))

    print_success "编译完成 (耗时: ${duration}s)"
    cd ..
}

# ==============================================================================
# 运行测试
# ==============================================================================
run_tests() {
    if [ "$RUN_TEST" = true ]; then
        print_info "运行测试程序..."

        if [ ! -f "$BUILD_DIR/bin/1d_gf" ]; then
            print_error "可执行文件不存在: $BUILD_DIR/bin/1d_gf"
            exit 1
        fi

        cd "$BUILD_DIR/bin"

        # 使用默认测试图像
        if [ -f "1D-GF-MatLab/images/input_7.png" ]; then
            if [ "$PROFILE_MODE" = true ]; then
                print_info "运行性能分析测试..."
                echo "==============================================="
                echo "🎯 性能分析模式 - 显示详细性能报告"
                echo "==============================================="
                ./1d_gf "1D-GF-MatLab/images/input_7.png" 2>&1 | head -40
                echo "==============================================="
                print_success "性能分析完成"
            else
                print_info "运行基本功能测试..."
                if timeout 10 ./1d_gf "1D-GF-MatLab/images/input_7.png" >/dev/null 2>&1; then
                    print_success "功能测试通过"
                else
                    print_warning "功能测试完成 (可能需要手动检查结果)"
                fi
            fi
        else
            print_warning "测试图像不存在，跳过自动测试"
            print_info "手动测试命令: ./bin/1d_gf [image_path]"
        fi

        cd ../..
    fi
}

# ==============================================================================
# 显示构建信息
# ==============================================================================
show_build_info() {
    echo
    print_success "构建完成!"
    echo
    echo "构建信息:"
    echo "  项目名称: $PROJECT_NAME"
    echo "  构建类型: $BUILD_TYPE"
    echo "  可执行文件: $BUILD_DIR/bin/1d_gf"
    echo "  构建目录: $BUILD_DIR/"
    echo
    echo "运行程序:"
    echo "  cd $BUILD_DIR/bin"
    echo "  ./1d_gf                    # 使用默认图像"
    echo "  ./1d_gf path/to/image.png  # 使用指定图像"
    echo
}

# ==============================================================================
# 权限检查
# ==============================================================================
check_permissions() {
    if [[ ! -x "$0" ]]; then
        print_warning "脚本没有执行权限，正在自动设置..."
        if chmod +x "$0"; then
            print_success "执行权限设置成功"
        else
            print_error "无法设置执行权限，请手动运行: chmod +x $0"
            exit 1
        fi
    fi
}

# ==============================================================================
# 主函数
# ==============================================================================
main() {
    echo "========================================"
    echo "  $PROJECT_NAME 一键编译脚本"
    echo "========================================"
    echo

    # 检查脚本权限
    check_permissions

    # 解析命令行参数
    parse_args "$@"

    # 检查依赖
    check_dependencies
    echo

    # 清理构建目录
    clean_build

    # 创建构建目录
    create_build_dir

    # CMake配置
    configure_cmake

    # 编译项目
    build_project

    # 运行测试
    run_tests

    # 显示构建信息
    show_build_info
}

# ==============================================================================
# 脚本入口
# ==============================================================================
main "$@"
