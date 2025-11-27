#include "guided_filter.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <iomanip>

using namespace cv;
using namespace std;

namespace gf {

// 性能统计实现
void PerformanceStats::printReport() const {
    cout << "\n" << string(60, '=') << endl;
    cout << "🎯 性能分析报告" << endl;
    cout << string(60, '=') << endl;

    cout << fixed << setprecision(4);
    cout << "📊 基本信息:" << endl;
    cout << "   图像尺寸: " << image_width << "×" << image_height << endl;
    cout << "   总处理时间: " << total_time << " 秒" << endl;
    cout << "   行滤波调用: " << row_filter_calls << " 次" << endl;
    cout << "   列滤波调用: " << col_filter_calls << " 次" << endl;

    cout << "\n⏱️  时间分布:" << endl;
    cout << "   预处理时间: " << preprocessing_time << " 秒 (" << (preprocessing_time/total_time*100) << "%)" << endl;
    cout << "   行方向滤波: " << row_filter_time << " 秒 (" << (row_filter_time/total_time*100) << "%)" << endl;
    cout << "   列方向滤波: " << col_filter_time << " 秒 (" << (col_filter_time/total_time*100) << "%)" << endl;
    cout << "   后处理时间: " << postprocessing_time << " 秒 (" << (postprocessing_time/total_time*100) << "%)" << endl;

    if (row_filter_calls > 0) {
        cout << "\n📈 平均性能:" << endl;
        cout << "   单次行滤波: " << (row_filter_time/row_filter_calls*1000) << " ms" << endl;
        cout << "   单次列滤波: " << (col_filter_time/col_filter_calls*1000) << " ms" << endl;
    }

    cout << string(60, '=') << endl;
}

void PerformanceStats::printOptimizationSuggestions() const {
    cout << "\n💡 优化建议:" << endl;

    // 分析时间分布
    double row_percentage = row_filter_time / total_time * 100;
    double col_percentage = col_filter_time / total_time * 100;

    if (row_percentage > 60 || col_percentage > 60) {
        cout << "⚡ 主要瓶颈: ";
        if (row_percentage > col_percentage) {
            cout << "行方向滤波 (占用 " << row_percentage << "% 的时间)" << endl;
            cout << "   建议: 考虑使用SIMD指令优化盒式滤波" << endl;
            cout << "   建议: 探索并行处理多行数据" << endl;
        } else {
            cout << "列方向滤波 (占用 " << col_percentage << "% 的时间)" << endl;
            cout << "   建议: 优化内存访问模式" << endl;
            cout << "   建议: 考虑OpenMP并行化" << endl;
        }
    }

    // 基于图像尺寸的建议
    if (image_width * image_height > 1000000) { // > 1M像素
        cout << "📏 大图像优化:" << endl;
        cout << "   建议: 分块处理以减少内存占用" << endl;
        cout << "   建议: 使用更高效的数据类型 (float vs double)" << endl;
    }

    // 函数调用频率分析
    if (row_filter_calls > image_height * 0.8) {
        cout << "🔄 循环优化:" << endl;
        cout << "   建议: 减少不必要的矩阵克隆操作" << endl;
        cout << "   建议: 优化内存分配策略" << endl;
    }

    cout << string(60, '=') << endl;
}

/**
 * 行方向盒式滤波器 - 使用累积和实现O(1)时间复杂度
 * @param imSrc 输入图像
 * @param w 滤波半径
 * @return 滤波后的图像
 */
Mat rowBoxFilter(const Mat& imSrc, int w) {
    int hei = imSrc.rows;  // 图像高度
    int wid = imSrc.cols;  // 图像宽度
    Mat imDst = Mat::zeros(hei, wid, CV_64F);

    // 沿X轴（列方向）计算累积和
    Mat imCum = Mat::zeros(hei, wid, CV_64F);
    imSrc.col(0).copyTo(imCum.col(0));
    for (int j = 1; j < wid; j++) {
        imCum.col(j) = imCum.col(j - 1) + imSrc.col(j);
    }

    // 左边部分: 0 到 w
    for (int j = 0; j <= w; j++) {
        imCum.col(w + j).copyTo(imDst.col(j));
    }

    // 中间部分: w+1 到 wid-w-1
    for (int j = w + 1; j < wid - w; j++) {
        imDst.col(j) = imCum.col(j + w) - imCum.col(j - w - 1);
    }

    // 右边部分: wid-w 到 wid-1
    for (int j = wid - w; j < wid; j++) {
        imDst.col(j) = imCum.col(wid - 1) - imCum.col(j - w - 1);
    }

    return imDst;
}

/**
 * 列方向盒式滤波器 - 使用累积和实现O(1)时间复杂度
 * @param imSrc 输入图像
 * @param h 滤波半径
 * @return 滤波后的图像
 */
Mat columnBoxFilter(const Mat& imSrc, int h) {
    int hei = imSrc.rows;  // 图像高度
    int wid = imSrc.cols;  // 图像宽度
    Mat imDst = Mat::zeros(hei, wid, CV_64F);

    // 沿Y轴（行方向）计算累积和
    Mat imCum = Mat::zeros(hei, wid, CV_64F);
    imSrc.row(0).copyTo(imCum.row(0));
    for (int i = 1; i < hei; i++) {
        imCum.row(i) = imCum.row(i - 1) + imSrc.row(i);
    }

    // 顶部部分: 0 到 h
    for (int i = 0; i <= h; i++) {
        imCum.row(h + i).copyTo(imDst.row(i));
    }

    // 中间部分: h+1 到 hei-h-1
    for (int i = h + 1; i < hei - h; i++) {
        imDst.row(i) = imCum.row(i + h) - imCum.row(i - h - 1);
    }

    // 底部部分: hei-h 到 hei-1
    for (int i = hei - h; i < hei; i++) {
        imDst.row(i) = imCum.row(hei - 1) - imCum.row(i - h - 1);
    }

    return imDst;
}

/**
 * 行方向导向滤波器
 * @param I 引导图像
 * @param p 输入图像
 * @param r 滤波半径
 * @param eps 正则化参数，防止除零
 * @return 滤波后的图像
 */
Mat rowGuidedFilter(const Mat& I, const Mat& p, int r, double eps) {
    int hei = I.rows;
    int wid = I.cols;

    // 计算每个局部窗口的像素数量
    Mat ones_mat = Mat::ones(hei, wid, CV_64F);
    Mat N = rowBoxFilter(ones_mat, r);

    // 计算均值
    Mat mean_I, mean_p, mean_Ip, cov_Ip;
    divide(rowBoxFilter(I, r), N, mean_I);      // I的均值
    divide(rowBoxFilter(p, r), N, mean_p);      // p的均值
    divide(rowBoxFilter(I.mul(p), r), N, mean_Ip);  // I*p的均值
    cov_Ip = mean_Ip - mean_I.mul(mean_p);      // I和p的协方差

    // 计算I的方差
    Mat mean_II, var_I;
    divide(rowBoxFilter(I.mul(I), r), N, mean_II);  // I*I的均值
    var_I = mean_II - mean_I.mul(mean_I);           // I的方差

    // 计算线性系数a和b（论文公式5和6）
    Mat a, b;
    divide(cov_Ip, var_I + eps, a);  // a = cov_Ip / (var_I + eps)
    b = mean_p - a.mul(mean_I);      // b = mean_p - a * mean_I

    // 计算a和b的均值
    Mat mean_a, mean_b;
    divide(rowBoxFilter(a, r), N, mean_a);
    divide(rowBoxFilter(b, r), N, mean_b);

    // 输出图像（论文公式8）
    Mat q = mean_a.mul(I) + mean_b;
    return q;
}

/**
 * 列方向导向滤波器
 * @param I 引导图像
 * @param p 输入图像
 * @param r 滤波半径
 * @param eps 正则化参数，防止除零
 * @return 滤波后的图像
 */
Mat columnGuidedFilter(const Mat& I, const Mat& p, int r, double eps) {
    int hei = I.rows;
    int wid = I.cols;

    // 计算每个局部窗口的像素数量
    Mat ones_mat = Mat::ones(hei, wid, CV_64F);
    Mat N = columnBoxFilter(ones_mat, r);

    // 计算均值
    Mat mean_I, mean_p, mean_Ip, cov_Ip;
    divide(columnBoxFilter(I, r), N, mean_I);      // I的均值
    divide(columnBoxFilter(p, r), N, mean_p);      // p的均值
    divide(columnBoxFilter(I.mul(p), r), N, mean_Ip);  // I*p的均值
    cov_Ip = mean_Ip - mean_I.mul(mean_p);         // I和p的协方差

    // 计算I的方差
    Mat mean_II, var_I;
    divide(columnBoxFilter(I.mul(I), r), N, mean_II);  // I*I的均值
    var_I = mean_II - mean_I.mul(mean_I);              // I的方差

    // 计算线性系数a和b（论文公式5和6）
    Mat a, b;
    divide(cov_Ip, var_I + eps, a);  // a = cov_Ip / (var_I + eps)
    b = mean_p - a.mul(mean_I);      // b = mean_p - a * mean_I

    // 计算a和b的均值
    Mat mean_a, mean_b;
    divide(columnBoxFilter(a, r), N, mean_a);
    divide(columnBoxFilter(b, r), N, mean_b);

    // 输出图像（论文公式8）
    Mat q = mean_a.mul(I) + mean_b;
    return q;
}

/**
 * 1D导向滤波图像去噪主函数 (带性能统计)
 * @param input 输入灰度图像 (CV_64F, 0-1范围)
 * @param stats 性能统计结构体 (输出)
 * @param row_radius 行方向滤波半径 (默认4)
 * @param row_eps 行方向正则化参数 (默认0.16)
 * @param col_eps 列方向正则化参数 (默认0.04)
 * @return 去噪后的图像 (CV_64F, 0-1范围)
 */
Mat denoise1DGuidedFilter(const Mat& input, PerformanceStats& stats,
                         int row_radius, double row_eps, double col_eps) {
    auto total_start = chrono::high_resolution_clock::now();

    int rows = input.rows;
    int cols = input.cols;

    // 初始化统计信息
    stats.image_width = cols;
    stats.image_height = rows;
    stats.row_filter_calls = rows;
    stats.col_filter_calls = cols;

    // 预处理计时开始
    auto preprocess_start = chrono::high_resolution_clock::now();

    // 第一步：1D行方向导向滤波
    // 对于384*288分辨率的图像，r=4，即窗口大小w=9=2*r+1
    Mat smooth = Mat::zeros(rows, cols, CV_64F);

    auto preprocess_end = chrono::high_resolution_clock::now();
    stats.preprocessing_time = chrono::duration<double>(preprocess_end - preprocess_start).count();

    // 行滤波计时开始
    auto row_filter_start = chrono::high_resolution_clock::now();

    for (int i = 0; i < rows; i++) {
        Mat row_i = input.row(i).clone();
        Mat result = rowGuidedFilter(row_i, row_i, row_radius, row_eps);
        result.copyTo(smooth.row(i));
    }

    auto row_filter_end = chrono::high_resolution_clock::now();
    stats.row_filter_time = chrono::duration<double>(row_filter_end - row_filter_start).count();

    // 计算高频部分
    Mat highpart = input - smooth;

    // 第二步：1D列方向导向滤波
    Mat strip = Mat::zeros(rows, cols, CV_64F);
    int r_col = cvRound(0.5 * (rows * 0.25 - 1));  // 计算列方向滤波半径

    auto col_filter_start = chrono::high_resolution_clock::now();

    for (int j = 0; j < cols; j++) {
        Mat smooth_col = smooth.col(j).clone();
        Mat highpart_col = highpart.col(j).clone();
        Mat result = columnGuidedFilter(smooth_col, highpart_col, r_col, col_eps);
        result.copyTo(strip.col(j));
    }

    auto col_filter_end = chrono::high_resolution_clock::now();
    stats.col_filter_time = chrono::duration<double>(col_filter_end - col_filter_start).count();

    // 后处理计时开始
    auto postprocess_start = chrono::high_resolution_clock::now();

    // 计算去噪后的图像
    Mat im_output = input - strip;   // 去噪后的图像

    auto postprocess_end = chrono::high_resolution_clock::now();
    stats.postprocessing_time = chrono::duration<double>(postprocess_end - postprocess_start).count();

    auto total_end = chrono::high_resolution_clock::now();
    stats.total_time = chrono::duration<double>(total_end - total_start).count();

    return im_output;
}

/**
 * 1D导向滤波图像去噪主函数 (简化版本)
 * @param input 输入灰度图像 (CV_64F, 0-1范围)
 * @param row_radius 行方向滤波半径 (默认4)
 * @param row_eps 行方向正则化参数 (默认0.16)
 * @param col_eps 列方向正则化参数 (默认0.04)
 * @return 去噪后的图像 (CV_64F, 0-1范围)
 */
Mat denoise1DGuidedFilter(const Mat& input,
                         int row_radius,
                         double row_eps,
                         double col_eps) {
    PerformanceStats dummy_stats; // 不使用的统计信息
    return denoise1DGuidedFilter(input, dummy_stats, row_radius, row_eps, col_eps);
}

} // namespace gf
