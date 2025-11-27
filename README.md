# 1D-GF: 1D导向滤波图像去噪

一个高效的C++实现，用于图像去噪的1D导向滤波算法。该项目将MATLAB的1D导向滤波代码转换为使用OpenCV的C++版本，提供快速、准确的图像去噪功能。


 **作者联系方式**:ImageVisioner@outlook.com
 **参考文献**
* Y. Cao, M. Y. Yang, and C. Tisse, “Effective Strip Noise Removal for Low-textured Infrared Images Based on 1D Guided Filtering,” IEEE Trans. Circuits Syst. Video Technol., vol. 26, no. 12, pp. 2176–2188, 2016.

* He K, Sun J, Tang X. Guided image filtering[J]. IEEE transactions on pattern analysis and machine intelligence, 2012, 35(6): 1397-1409.

## 📋 功能特性

- 🚀 **高效性能**: 基于累积和的O(1)时间复杂度盒式滤波
- 🎯 **精确去噪**: 1D导向滤波算法，分别在行和列方向进行滤波
- 📸 **图像支持**: 支持灰度图像处理
- ⚡ **快速处理**: C++实现，比MATLAB版本更快
- 🔧 **易于集成**: 简洁的API，易于集成到其他项目
- 📦 **跨平台**: 支持Linux/macOS/Windows

## 🧮 算法原理

### 1D导向滤波流程

1. **行方向滤波**: 对图像的每一行应用导向滤波
2. **高频提取**: 计算高频分量（细节）
3. **列方向滤波**: 对高频分量进行列方向导向滤波
4. **纹理提取**: 获取最终的纹理细节
5. **去噪输出**: 生成去噪后的图像

### 核心参数
- **行滤波半径**: `r = 4` (对应384×288分辨率)
- **行正则化参数**: `ε = 0.4²`
- **列滤波半径**: `r_col = round(0.5 × (rows × 0.25 - 1))`
- **列正则化参数**: `ε = 0.2²`

## 🏗️ 构建要求

### 系统要求
- **操作系统**: Linux/macOS/Windows
- **编译器**: GCC 7+ / Clang 6+ / MSVC 2017+
- **CMake**: 3.10+

### 依赖库
- **OpenCV**: 4.0+ (core, imgproc, highgui, imgcodecs)

#### Ubuntu/Debian
```bash
sudo apt update
sudo apt install build-essential cmake libopencv-dev
```

#### CentOS/RHEL
```bash
sudo yum install gcc gcc-c++ make cmake opencv-devel
```

#### macOS (Homebrew)
```bash
brew install cmake opencv
```


## 🚀 快速开始

### 方法一：一键编译脚本 (推荐)

```bash
# 直接运行一键编译脚本
./build.sh

# 或使用不同选项
./build.sh -c          # 清理后重新编译
./build.sh -d          # 调试模式编译
./build.sh -t          # 编译后自动测试
./build.sh -c -d -t    # 清理+调试模式+测试
```

### 方法二：手动编译

```bash
# 创建构建目录
mkdir build && cd build

# 配置项目
cmake ..

# 编译项目
make
```

### 2. 运行程序

```bash
# 进入构建目录
cd build/bin

# 使用默认图像
./1d_gf

# 使用自定义图像
./1d_gf path/to/your/image.png
```

### 3. 查看结果

程序会显示处理时间，并在当前目录生成 `output.png` 结果文件。

## 📖 使用方法

### 命令行参数

```bash
./bin/1d_gf [image_path]
```

- `image_path`: (可选) 输入图像路径，默认使用 `1D-GF/images/input_7.png`

### 输出文件

- `output.png`: 去噪后的图像结果
- 控制台输出: 处理耗时信息

## 📁 项目结构

```
1D-GF/
├── build.sh               # 一键编译脚本 ⭐
├── main.cpp               # 主函数文件
├── guided_filter.hpp      # 导向滤波头文件
├── guided_filter.cpp      # 导向滤波实现文件
├── CMakeLists.txt         # CMake构建配置
├── 1D-GF/                # MATLAB原版代码和测试图像
│   ├── GF.m              # MATLAB主程序
│   ├── images/           # 测试图像
│   └── operations/       # MATLAB函数
├── build/                # 构建目录 (生成)
│   └── bin/             # 可执行文件输出目录
└── README.md             # 项目文档
```

## 🔨 构建工具

### 一键编译脚本 (build.sh)

项目提供了一个功能强大的 `build.sh` 脚本，让编译过程变得极其简单：

#### 脚本特性
- ✅ **自动依赖检查**: 自动检测并提示缺失的依赖
- ✅ **智能构建**: 支持清理重建和增量编译
- ✅ **多构建模式**: 支持Release和Debug模式
- ✅ **自动测试**: 可选的功能测试和性能分析测试
- ✅ **性能分析**: 专门的性能剖析模式 (-p选项)
- ✅ **详细输出**: 彩色输出和进度显示
- ✅ **错误处理**: 遇到错误立即停止并提示

#### 使用方法

```bash
# 首次使用前设置执行权限
chmod +x build.sh

# 基本用法 - 一键编译
./build.sh

# 高级用法
./build.sh -c                 # 清理后重新编译
./build.sh -d                 # 调试模式编译
./build.sh -t                 # 编译后自动功能测试
./build.sh -p                 # 编译后性能分析测试 ⭐
./build.sh -c -d -p           # 清理+调试+性能分析
./build.sh --help             # 查看详细帮助
```

#### 脚本选项详解

| 选项 | 长选项 | 说明 |
|------|--------|------|
| `-c` | `--clean` | 清理构建目录后重新编译 |
| `-r` | `--release` | 发布模式编译 (默认) |
| `-d` | `--debug` | 调试模式编译 |
| `-t` | `--test` | 编译后运行自动测试 |
| `-h` | `--help` | 显示帮助信息 |

## 📁 代码结构

### 模块化设计

项目采用模块化设计，将功能分为以下几个文件：

- **`guided_filter.hpp`**: 公共接口头文件
  - 函数声明和类型定义
  - 命名空间 `gf` 封装

- **`guided_filter.cpp`**: 核心算法实现
  - 盒式滤波算法
  - 导向滤波算法
  - 去噪主流程

- **`main.cpp`**: 应用程序入口
  - 命令行参数处理
  - 图像加载和保存
  - 性能计时和结果显示

### 核心函数说明

#### 基础滤波函数
- `gf::rowBoxFilter()`: 行方向盒式滤波
- `gf::columnBoxFilter()`: 列方向盒式滤波

#### 导向滤波函数
- `gf::rowGuidedFilter()`: 行导向滤波
- `gf::columnGuidedFilter()`: 列导向滤波

#### 高层接口
- `gf::denoise1DGuidedFilter()`: 完整的1D去噪流程
- `gf::PerformanceStats`: 性能统计结构体

#### 性能分析接口
- `stats.printReport()`: 打印详细性能报告
- `stats.printOptimizationSuggestions()`: 显示优化建议

## ⚡ 性能表现

### 测试环境
- **CPU**: Intel i5-8250U
- **内存**: 8GB
- **图像尺寸**: 384×288 (灰度)

### 性能数据
- **处理时间**: ~1秒 (384×288图像)
- **内存占用**: < 50MB
- **CPU使用率**: 单线程处理

### 如何使用性能分析

程序运行时会自动显示详细的性能分析报告，包括：

1. **时间分布**: 各处理阶段的耗时占比
2. **瓶颈识别**: 自动识别最耗时的操作
3. **优化建议**: 基于分析结果的改进建议

#### 示例输出
运行程序后会看到类似以下的性能报告，帮助你了解：
- 哪个处理阶段耗时最多
- 如何优化算法性能
- 内存使用情况分析

### 性能分析功能

程序内置详细的性能分析功能，能够：

#### 📊 **时间统计**
- **总处理时间**: 完整的算法执行时间
- **各阶段耗时**: 预处理、行滤波、列滤波、后处理
- **函数调用统计**: 行/列滤波器的调用次数和平均耗时

#### 🎯 **瓶颈识别**
- **自动识别**: 哪个处理阶段耗时最多
- **百分比分析**: 各阶段时间占比
- **优化建议**: 基于分析结果的针对性建议

#### 📈 **详细报告**
```
============================================================
🎯 性能分析报告
============================================================
📊 基本信息:
   图像尺寸: 384×288
   总处理时间: 0.9562 秒
   行滤波调用: 288 次
   列滤波调用: 384 次

⏱️  时间分布:
   预处理时间: 0.0006 秒 (0.06%)
   行方向滤波: 0.4878 秒 (51.02%)
   列方向滤波: 0.4667 秒 (48.81%)
   后处理时间: 0.0003 秒 (0.04%)

📈 平均性能:
   单次行滤波: 1.694 ms
   单次列滤波: 1.215 ms
============================================================

💡 优化建议:
🔄 循环优化:
   建议: 减少不必要的矩阵克隆操作
   建议: 优化内存分配策略
============================================================
```

## 📚 参考文献

1. **导向滤波原论文**:
   Kaiming He, Jian Sun, Xiaoou Tang. "Guided Image Filtering." ECCV 2010.

2. **1D导向滤波扩展**:
   原论文的1D变体实现，用于高效的图像去噪。

## 🔄 从MATLAB到C++的转换

本项目将以下MATLAB函数转换为C++：

| MATLAB函数 | C++函数 | 说明 |
|-----------|---------|------|
| `rowboxfilter.m` | `rowBoxFilter()` | 行方向盒式滤波 |
| `columnboxfilter.m` | `columnBoxFilter()` | 列方向盒式滤波 |
| `rowguidedfilter.m` | `rowGuidedFilter()` | 行导向滤波 |
| `columnguidedfilter.m` | `columnGuidedFilter()` | 列导向滤波 |



### 开发环境设置

```bash
# 克隆项目
git clone <repository-url>
cd 1D-GF

# 创建构建目录
mkdir build && cd build

# 配置和构建
cmake ..
make

# 运行测试并查看性能分析
./bin/1d_gf
```

### 性能调优指南

项目内置性能分析功能，帮助开发者优化算法：

1. **运行程序**：程序会自动显示详细的性能报告
2. **分析瓶颈**：查看哪个处理阶段耗时最多
3. **应用优化**：根据建议改进代码性能
4. **对比测试**：修改后重新运行，比较性能提升

### 代码架构说明

项目采用现代C++模块化设计：

- **头文件分离**: `guided_filter.hpp` 提供清晰的API接口
- **命名空间封装**: 使用 `gf` 命名空间避免命名冲突
- **功能分离**: 算法实现与应用逻辑分离，便于测试和重用
- **易于扩展**: 可以轻松添加新的滤波算法或参数配置

## 📄 许可证

本项目采用MIT许可证 - 详见LICENSE文件

---

**最后更新**: 2025年11月
