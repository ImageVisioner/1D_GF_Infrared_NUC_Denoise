#include "guided_filter.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono> //用来处理时间的标准库

using namespace cv;
using namespace std;

/**
 * Email:imagevisioner@outlook.com
 */


/**
 * 主函数 - 1D导向滤波图像去噪程序 (带性能分析)
 */
int main(int argc, char** argv) {
    // 默认图像路径
    string imagePath = "1D-GF-MatLab/images/input_2.png";
    if (argc > 1) {
        imagePath = argv[1];
    }

    cout << "🚀 1D导向滤波图像去噪程序" << endl;
    cout << "📁 输入图像: " << imagePath << endl;
    cout << string(60, '-') << endl;

    // 读取灰度图像
    Mat im_input = imread(imagePath, IMREAD_GRAYSCALE);
    if (im_input.empty()) {
        cerr << "❌ 错误: 无法加载图像 " << imagePath << endl;
        return -1;
    }

    cout << "✅ 图像加载成功: " << im_input.cols << "×" << im_input.rows << " (灰度)" << endl;

    // 转换为double类型并归一化到[0,1]
    Mat im_double;
    im_input.convertTo(im_double, CV_64F, 1.0 / 255.0);

    // 创建性能统计对象
    gf::PerformanceStats stats;

    // 执行去噪处理
    cout << "🔄 开始去噪处理..." << endl;
    Mat im_output = gf::denoise1DGuidedFilter(im_double, stats);

    // 显示详细性能报告
    stats.printReport();
    stats.printOptimizationSuggestions();

    // 转换回8位图像用于显示
    Mat output_display;
    im_output.convertTo(output_display, CV_8U, 255.0);

    // 显示结果
    cout << "🖼️  显示结果窗口 (按任意键关闭)..." << endl;
    imshow("去噪结果", output_display);
    waitKey(0);

    // 保存输出图像
    imwrite("output.png", output_display);
    cout << "💾 结果已保存到 output.png" << endl;

    cout << "\n🎉 处理完成!" << endl;
    return 0;
}
