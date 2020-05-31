// If you want visualize photo
// #define DEBUG

// blur_level - degree of blurring (optimally 0 - 5)
// make_photo_blur("path/to/picture", "path/to/picture_save", blur_level); - opencv GaussianBlur
// my_blur("path/to/picture", "path/to/picture_save", blur_level); - my realize blur
// my_blur("path/to/picture", "path/to/picture_save", blur_level, 1); - my multithreade realize blur

#include "blur.hpp"

int make_photo_blur(const std::string& f_path, const std::string& path_to_save, int level_blur) {
    cv::Mat img = cv::imread(f_path);

    if (img.empty()) {
        std::cout << "Could not open or find the image" << std::endl;
        return IMG_OPEN_ERROR;
    }

    int kernel_size = level_blur * 10 + 1;

    cv::Mat img_blurred;

    cv::GaussianBlur(img, img_blurred, cv::Size(kernel_size, kernel_size), 0);

    #ifdef DEBUG
    std::string window_name = "DEBUG";
    cv::namedWindow(window_name);
    imshow(window_name, img_blurred);

    cv::waitKey(0);

    cv::destroyAllWindows();
    #endif

    cv::imwrite(path_to_save, img_blurred);

    return 0;
}

cv::Vec3b avg_pixel_radius(cv::Mat img, int x, int y, int radius) {
    int avg_r = 0;
    int avg_g = 0;
    int avg_b = 0;

    int delta = (radius - 1) / 2;

    int count = 0;

    for (int i = x - delta; i <= x + delta; ++i) {
        for (int j = y - delta; j <= y + delta; ++j) {
            if (i < 0 or i >= img.cols) {
                continue;
            }
            if (j < 0 or j >= img.rows) {
                continue;
            }
            count++;
            cv::Vec3b &color = img.at<cv::Vec3b>(j, i);
            avg_b += color[0];
            avg_g += color[1];
            avg_r += color[2];
        }
    }
    return cv::Vec3b(avg_b / count, avg_g / count, avg_r / count);
}

void iteration(cv::Mat img, cv::Mat img_blur, int iter, int n, int size) {
    for (int y = iter; y < img.rows; y += n) {
        for (int x = 0; x < img.cols; x ++) {
            img_blur.at<cv::Vec3b>(cv::Point(x,y)) = avg_pixel_radius(img, x, y, size);
        }
    }
}

int my_blur(const std::string& f_path, const std::string& path_to_save, int level_blur, int many_thread = 0) {
    cv::Mat img = cv::imread(f_path);

    if (img.empty()) {
        std::cout << "Could not open or find the image" << std::endl;
        return IMG_OPEN_ERROR;
    }

    cv::Mat img_blur(img);

    int size = level_blur * 2 + 1;

    if (many_thread) {
        int n = _SC_NPROCESSORS_ONLN;
        std::vector<std::thread> workers(n);
        for (int i = 0; i < n; i++) {
            workers[i] = std::thread(iteration, img, img_blur, i, n, size);
        }
        for (int i = 0; i < n; i++) {
            workers[i].join();
        }
    } else{
        iteration(img, img_blur, 0, 1, size);
    }

    #ifdef DEBUG
    std::string window_name = "DEBUG";
    cv::namedWindow(window_name);
    imshow(window_name, img_blur);

    cv::waitKey(0);

    cv::destroyAllWindows();
    #endif

    cv::imwrite(path_to_save, img_blur);

    return 0;
}
