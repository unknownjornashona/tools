#ifndef CROPPING_H
#define CROPPING_H

#include <opencv2/opencv.hpp>

class Cropping {
public:
    virtual ~Cropping() {}

    // 裁剪图像
    virtual cv::Mat crop(const cv::Mat& image, const cv::Rect& region) = 0;
};

#endif // CROPPING_H
