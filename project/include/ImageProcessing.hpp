#pragma once
#include <opencv4/opencv2/opencv.hpp>

bool process_image(const char *input_path, const char *output_path, int blur_lvl);
cv::Mat gaussian_blur(const cv::Mat &image, int m_dim);
