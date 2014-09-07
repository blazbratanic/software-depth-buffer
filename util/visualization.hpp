#ifndef VISUALIZATION_HPP_
#define VISUALIZATION_HPP_

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include <util/array2dview.h>
#include <util/array2dview_op.h>
#include <util/constants.h>
#include <util/contour.h>
#include <util/position.hpp>

#include <vector>
#include <utility>

template <typename T>
void save_image(ConstArray2dView<T> image, std::string filename) {
  Array2d<float> tmp;
  tmp.Allocate(image.rows(), image.cols());
  sil::copy(image, tmp);
  const T max_el = *max_element(image);
  const T min_el = *min_element(image);
  if ((max_el - min_el) != 0) {
    sil::transform(tmp, tmp, [min_el, max_el](T a) {
      return (a - min_el) / (max_el - min_el) * 255;
    });
  }

  cv::Mat im(tmp.rows(), tmp.cols(), CV_8UC1, cv::Scalar(0));

  FOREACH2D(i0, i1, tmp) {
    im.at<unsigned char>(i0, i1) = static_cast<unsigned char>(tmp(i0, i1));
  }

  cv::imwrite(filename, im);
}


void save_label(ConstArray2dView<int> image, std::string filename) {

  cv::Mat im(image.rows(), image.cols(), CV_8UC3, cv::Scalar(0));

  FOREACH2D(i0, i1, image) {
    int b = image(i0, i1) % 256;
    int g = ((image(i0, i1) - b) / 256) % 256;
    int r = ((image(i0, i1) - b - 256 * g) / (256 * 256)) % 256;
    im.at<cv::Vec3b>(i0, i1)[0] = r;
    im.at<cv::Vec3b>(i0, i1)[1] = g;
    im.at<cv::Vec3b>(i0, i1)[2] = b;
  }

  cv::imwrite(filename, im);
}
#endif
