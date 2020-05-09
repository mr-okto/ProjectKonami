#include <ImageProcessing.hpp>
#include <vector>

using namespace cv;

// Blur levels 0-5
bool process_image(const char *input_path, const char *output_path, int blur_lvl) {
  Mat image = imread(input_path, IMREAD_UNCHANGED);
  if (blur_lvl > 5 || blur_lvl < 0) {
    return false;
  }
  //check whether the image is loaded or not
  if (image.data == nullptr) {
    return false;
  }
  if (!blur_lvl) {
    imwrite(output_path, image);
    return true;
  }
  // Blur matrix size
  int m_x = image.cols * blur_lvl / 20;
  int m_y = image.rows * blur_lvl / 20;
  if (!(m_x % 2)) {
      m_x += 1; // must be even number
  }
  if (!(m_y % 2)) {
      m_y += 1; // must be even number
  }
  Mat result;
  GaussianBlur(image, result, Size(m_x, m_y), 0, 0);
  imwrite(output_path, result);
  return true;
}

Mat gaussian_blur(const Mat &image, int m_dim) {
    Mat result = image.clone();
    int w = image.cols;
    int h = image.rows;
    int cn = image.channels();
    // Weight and value of pixel channels
    std::vector<double> c_value(cn, 0);
    std::vector<double> c_weight(cn, 0);

    uchar *img_ptr = image.data;
    uchar *result_ptr = result.data;

    for (int line = 0; line < h; line++) {
        for (int col = 0; col < w; col++) {
          // Creation of samples matrix
          for (int iy = line - m_dim; iy <= line + m_dim; iy++) {
            for (int ix = col - m_dim; ix <= col + m_dim; ix++) {
              // Check index
              int x = std::min(w - 1, std::max(0, ix));
              int y = std::min(h - 1,std::max(0, iy));
              double dsq = (ix - col) * (ix - col) + (iy - line) * (iy - line);
              double weight = exp(-dsq / (2 * m_dim * m_dim)) / (M_PI * 2 * m_dim * m_dim);
              // Samples from RGB channels
              for (int c = 0; c < cn; c++) {
                c_value[c] += img_ptr[y * w * cn + x * cn + c] * weight;
                c_weight[c] += weight;
              }
            }
          }
          for (int c = 0; c < cn; c++) {
            result_ptr[line * w * cn + col * cn + c] = round(c_value[c] / c_weight[c]);
            c_value[c] = 0.0;
            c_weight[c] = 0.0;
          }
        }
    }
    return result;
}

