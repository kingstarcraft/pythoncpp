#pragma once
#include <zero/common.h>
#include <zero/base/vector.h>
namespace zero {
	template <typename Dtype>
	class ZERO_API Mat : public vector<Dtype> {
	public:
		explicit Mat() {};
		explicit Mat(const std::vector<size_t>& shape) {
			Create(shape);
		}

		template <typename Type> 
		Mat(const Type &mat){
			Copy<Type>(mat);
		}

		void Create(const std::vector<size_t>& shape) {
			shape_ = shape;
			size_t size = shape_.size() > 0;
			for (auto& dim : shape_) { size *= dim; }
			this->resize(size);
		}

		const std::vector<size_t> &shape() const {
			return shape_;
		}

		const size_t& shape(const int64_t &dim) const {
			if (dim < 0) { return shape_[dim + int64_t(shape_.size())]; }
			return shape_[dim];
		}

		const Dtype* data() const {
			return this->datas_.data();
		}

		const Dtype &data(size_t i) const {
			return this->datas_[i];
		}

		Dtype& data(size_t i) {
			return this->datas_[i];
		}

		template <typename Type>
		typename std::enable_if_t<std::is_same_v<Type, Mat>> Copy(const Mat &mat) {
			*this = mat;
		}



#ifdef HAVE_OPENCV_CORE
		template <typename Mat>
		typename std::enable_if_t<std::is_same_v<Mat, cv::Mat>> Copy(const cv::Mat& mat) {
			size_t channels = mat.channels();
			if (channels == 1) { this->Create({ (size_t)mat.rows, (size_t)mat.cols }); }
			else { this->Create({ (size_t)mat.rows, (size_t)mat.cols, channels }); }
			if (sizeof(Dtype) == 1) { 
				cv::Mat dst(mat.size(), CV_8UC(channels), this->datas_.data());
				mat.convertTo(dst, CV_8UC(channels));
			}
			else if (sizeof(Dtype) == 2) { 
				cv::Mat dst(mat.size(), CV_16UC(channels), this->datas_.data());
				mat.convertTo(dst, CV_16UC(channels));
			}
			else if (typeid(int) == typeid(Dtype)) {
				cv::Mat dst(mat.size(), CV_32SC(channels), this->datas_.data());
				mat.convertTo(dst, CV_32SC(channels));
			}
			else if (typeid(float) == typeid(Dtype)) {
				cv::Mat dst(mat.size(), CV_32FC(channels), this->datas_.data());
				mat.convertTo(dst, CV_32FC(channels));
			}
			else if (sizeof(Dtype) == 8) {
				cv::Mat dst(mat.size(), CV_64FC(channels), this->datas_.data());
				mat.convertTo(dst, CV_64FC(channels)); 
			}
			else { throw std::logic_error(std::string("Can't convert cv::Mat to ") + typeid(Dtype).name() + " Mat."); }
		}

		template <typename Mat>
		typename std::enable_if_t<std::is_same_v<Mat, cv::Mat>, cv::Mat> get() const {
			int channels = 1;
			int height = 1;
			int width = 1;
			if (shape_.size() == 0) { return cv::Mat(); }
			if (shape_.size() >= 1) { height = shape_[0]; }
			if (shape_.size() >= 2) { width = shape_[1]; }
			if (shape_.size() >= 3) { channels = shape_[2]; }
			if (shape_.size() >= 4) { throw std::logic_error(std::string("Can't convert ") + std::to_string(shape_.size()) + "D Mat to cv::Mat."); }

			int type = CV_8UC(channels);
			if (typeid(Dtype) == typeid(int8_t)) { type = CV_8SC(channels); }
			else if (typeid(Dtype) == typeid(uint8_t)) { type = CV_8UC(channels); }
			else if (typeid(Dtype) == typeid(int16_t)) { type = CV_16SC(channels); }
			else if (typeid(Dtype) == typeid(uint16_t)) { type = CV_16UC(channels); }
			else if (typeid(Dtype) == typeid(int32_t)) { type = CV_32SC(channels); }
			else if (typeid(Dtype) == typeid(float_t)) { type = CV_32FC(channels); }
			else if (typeid(Dtype) == typeid(double_t)) { type = CV_64FC(channels); }
			else { throw std::logic_error(std::string("Can't convert ") + typeid(Dtype).name() + " Mat to cv::Mat."); }
			return cv::Mat(height, width, type, (void*)this->datas_.data());
		}

		template <typename Mat>
		typename std::enable_if_t <std::is_same_v<Mat, cv::Mat>> get(cv::Mat& mat) const {
			get<cv::Mat>().copyTo(mat);
		}
#endif
	private:
		std::vector<size_t> shape_;

	};
}
