#pragma once

#include <opencv2/opencv.hpp>
#include <chrono>

/**
 * @brief 1D导向滤波图像去噪库
 *
 * 该头文件定义了1D导向滤波算法的接口函数，
 * 包括盒式滤波和导向滤波的实现。
 */

namespace gf {

// 性能统计结构体
struct PerformanceStats {
    double total_time = 0.0;           // 总处理时间
    double row_filter_time = 0.0;      // 行方向滤波时间
    double col_filter_time = 0.0;      // 列方向滤波时间
    double preprocessing_time = 0.0;   // 预处理时间
    double postprocessing_time = 0.0;  // 后处理时间
    int row_filter_calls = 0;          // 行滤波调用次数
    int col_filter_calls = 0;          // 列滤波调用次数
    int image_width = 0;               // 图像宽度
    int image_height = 0;              // 图像高度

    // 打印性能报告
    void printReport() const;
    // 获取性能建议
    void printOptimizationSuggestions() const;
};

/**
 * 行方向盒式滤波器 - 使用累积和实现O(1)时间复杂度
 * @param imSrc 输入图像
 * @param w 滤波半径
 * @return 滤波后的图像
 */
cv::Mat rowBoxFilter(const cv::Mat& imSrc, int w);

/**
 * 列方向盒式滤波器 - 使用累积和实现O(1)时间复杂度
 * @param imSrc 输入图像
 * @param h 滤波半径
 * @return 滤波后的图像
 */
cv::Mat columnBoxFilter(const cv::Mat& imSrc, int h);

/**
 * 行方向导向滤波器
 * @param I 引导图像
 * @param p 输入图像
 * @param r 滤波半径
 * @param eps 正则化参数，防止除零
 * @return 滤波后的图像
 */
cv::Mat rowGuidedFilter(const cv::Mat& I, const cv::Mat& p, int r, double eps);

/**
 * 列方向导向滤波器
 * @param I 引导图像
 * @param p 输入图像
 * @param r 滤波半径
 * @param eps 正则化参数，防止除零
 * @return 滤波后的图像
 */
cv::Mat columnGuidedFilter(const cv::Mat& I, const cv::Mat& p, int r, double eps);

/**
 * 1D导向滤波图像去噪主函数 (带性能统计)
 * @param input 输入灰度图像 (CV_64F, 0-1范围)
 * @param stats 性能统计结构体 (输出)
 * @param row_radius 行方向滤波半径 (默认4)
 * @param row_eps 行方向正则化参数 (默认0.16)
 * @param col_eps 列方向正则化参数 (默认0.04)
 * @return 去噪后的图像 (CV_64F, 0-1范围)
 */
cv::Mat denoise1DGuidedFilter(const cv::Mat& input, PerformanceStats& stats,
                             int row_radius = 4, double row_eps = 0.16, double col_eps = 0.04);

/**
 * 1D导向滤波图像去噪主函数 (简化版本)
 * @param input 输入灰度图像 (CV_64F, 0-1范围)
 * @param row_radius 行方向滤波半径 (默认4)
 * @param row_eps 行方向正则化参数 (默认0.16)
 * @param col_eps 列方向正则化参数 (默认0.04)
 * @return 去噪后的图像 (CV_64F, 0-1范围)
 */
cv::Mat denoise1DGuidedFilter(const cv::Mat& input,
                             int row_radius = 4,
                             double row_eps = 0.16,
                             double col_eps = 0.04);

} // namespace gf
