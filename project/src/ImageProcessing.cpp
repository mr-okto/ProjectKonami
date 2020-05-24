#include <ImageProcessing.hpp>
#include <vector>
#include <tuple>
#include <boost/filesystem.hpp>
#include "Randomizer.hpp"
using namespace cv;

bool process_image(const std::string &input_path, const std::string &output_path, unsigned int blur_lvl) {
  Mat image = imread(input_path, IMREAD_UNCHANGED);
  //check whether the image is loaded or not
  if (image.data == nullptr) {
    return false;
  }
  if (!blur_lvl) {
    imwrite(output_path, image);
    return true;
  }
  // Blur matrix size
  unsigned int m_x = image.cols * blur_lvl / 20;
  unsigned int m_y = image.rows * blur_lvl / 20;
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


// Non thread safe
// File format: {dest_dir}/{user_id % folders_count}/img_{random_fname(32)}_{access_lvl}.jpg
// Example (dest_dir == ".", user_id = 228): ./28/img_kgeuFGTlGubSgtdSoRiGGufMEdDugTiD_5.jpg
std::vector<std::string> create_blurred_copies(const std::string &image_path,
                                               const std::string &dest_dir,
                                               long long user_id,
                                               unsigned int blur_levels) {
  std::vector<std::string> filenames;
  int folders_count = 100;
  boost::filesystem::path p_base(dest_dir);
  // Images are stored in folders
  p_base /= std::to_string(user_id % folders_count);
  create_directories(p_base);
  p_base /= "img_";
  boost::filesystem::path p_test;
  std::string f_template;
//  Randomizer &randomizer = Randomizer::get_instance();
  do {
    p_test = p_base;
//    f_template = randomizer.get_string(32);
      f_template = "template";
    p_test += f_template;
    p_test += "_0.jpg";
  } while (boost::filesystem::exists(p_test));

  std::stringstream final_path;
  for (unsigned int i = 0; i < blur_levels; i++) {
    final_path.str(std::string());
    final_path.clear();
    final_path << p_base.string() << f_template << "_" << i << ".jpg";
    if (process_image(image_path, final_path.str(), i)) {
      filenames.emplace_back(final_path.str());
    }
  }
  return filenames;
}
