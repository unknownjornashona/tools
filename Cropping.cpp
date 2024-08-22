#include "Cropping.h"

class SimpleCropping : public Cropping {
public:
    cv::Mat crop(const cv::Mat& image, const cv::Rect& region) override {
        if (region.x < 0 || region.y < 0 || 
            region.x + region.width > image.cols || 
            region.y + region.height > image.rows) {
            throw std::out_of_range("裁剪区域超出图像边界.");
        }
        return image(region); // 使用矩形区域裁剪图像
    }
};

// 如果需要在其他地方使用，可以根据需要提供实例化函数或其他功能
