#pragma once
#include <vector>
#include <map>
//#include <algorithm>
#include "base.h"

namespace zero {
	template<typename Type>
	class vector : public Iterator<std::vector<Type>>, public Array<Type, std::vector<Type>> {
	public:
		vector() {};
		~vector() {};

		vector(size_t size) {
			this->datas_.resize(size);
		}
		vector(size_t size, Type value) {
			this->datas_.resize(size, value);
		}
		vector(const std::initializer_list<Type> &datas) {
			this->datas_ = datas;
		}

		virtual vector &operator=(std::initializer_list<Type> datas) {
			this->datas_ = datas;
			return *this;
		}

//		virtual vector operator-(const vector &datas) {
//			vector reval;
//			auto size = std::min(datas.size(), this->size());
//			reval.resize(size);
//			for (auto i = 0; i < size; i++) {
//				reval[i] = this->datas_[i] - datas[i];
//			}
//			return reval;
//		}
	};

	template<typename Type>
	class vector<Type &>: public Iterator<std::vector<Type *>>, public Array<Type &, std::vector<Type *>> {
	public:
		vector() {};
		~vector() {};

		vector(size_t size) {
			this->datas_.resize(size);
		}
		vector(size_t size, Type value) {
			this->datas_.resize(size, value);
		}
		vector(const std::initializer_list<Type *> &datas) {
			this->datas_ = datas;
		}

		virtual vector &operator=(const std::initializer_list<Type *> &datas) {
			this->datas_ = datas;
			return *this;
		}
	private:

	};
}