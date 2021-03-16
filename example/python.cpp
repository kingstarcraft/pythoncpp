#ifdef USE_OPENCV
#include <opencv2/opencv.hpp>
#endif
#include "zero/python/python.h"



int main(int argc, char* argv[]) {
	// open python engine
	// zero::Python python(python_root) if can't find python with "where/which python" in cmd.
	zero::Python python;

	
	//import cv2
	auto cv2 = python["cv2"];

	//image = cv2.imread("data/lena.jpg")
	auto python_image = cv2["imread"]("E:/zero/data/lena.jpg");
	cv2["imwrite"]("E:/zero/data/lena.png", python_image);

#ifdef USE_OPENCV
	try {
		auto cpp_image = python_image.get<zero::Mat<uchar>>().get<cv::Mat>();
		cv::imshow("cpp", cpp_image);
		cv::waitKey(0);
	}
	catch (std::exception& error) {
		std::cout << error.what();
	};

#endif

	return 0;
}