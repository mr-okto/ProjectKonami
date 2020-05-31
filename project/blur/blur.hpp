#pragma once

#include <opencv2/opencv.hpp>
#include <iostream>
#include <unistd.h>

#include <vector>
#include <thread>

#define IMG_OPEN_ERROR 1

int make_photo_blur(const std::string& f_path, const std::string& path_to_save, int level_blur);
int my_blur(const std::string& f_path, const std::string& path_to_save, int level_blur, int many_thread = 0);
