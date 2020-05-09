#pragma once
#include <opencv4/opencv2/opencv.hpp>

void process_image(const std::string &input_path, const std::string &output_path, unsigned int blur_lvl);
cv::Mat gaussian_blur(const cv::Mat &image, int m_dim);
std::vector<std::string> create_blurred_copies(const std::string &image_path, const std::string &dest_dir,
                                               long long user_id);