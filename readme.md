#  PythonCPP
A C++ library for call python module

## Dependence

- C++17
- python>=3
- Opencv(optional)
- Release(DEBUG may not work with 3.8.5, there is a bug in python_d.lib)

## Usage
### Windows
Download [prebuilt binaries](https://github.com/kingstarcraft/pythoncpp/releases/download/v1.0.0/zero.zip) for your project.

Or compile the source code with VS2017:
- Open windows/python/python.props and set PythonDir=$(your python root) at line 10.
- Open windows/zero.sln and build libpython project to generate python.dll and python.lib



## Example
- Call opencv module
```cpp
#ifdef USE_OPENCV
#include <opencv2/opencv.hpp>
#endif
#include "zero/python/python.h"



int main(int argc, char* argv[]) {
	// open python engine
	// use "zero::Python python(python_root)" if python_root was not in path.
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
```
