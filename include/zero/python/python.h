#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>

#include <zero/common.h>
#include <zero/base/base.h>
#include <zero/math/mat.h>

namespace zero {
	class ZERO_API Python {
	public:
		class ZERO_API Object {
		public:

			//	template<typename... Atype>
			//	Object operator()(Atype&&... arguments) const{
			//		std::vector<void*> list;
			//		std::vector<uint8_t> mask;
			//		for_each(std::forward_as_tuple(arguments...), [&](const auto& argument) {
			//			list.push_back(python::Convert(argument));
			//			mask.push_back(
			//				typeid(void*) != typeid(argument) &&
			//				typeid(const void*) != typeid(argument) &&
			//				typeid(Object) != typeid(argument) &&
			//				typeid(const Object) != typeid(argument)
			//			);
			//		});
			//		auto reval = this->call(list);
			//		for (int i = 0; i < mask.size(); i++) { if (mask[i]) { python::Release(list[i]); } }
			//		return reval;
			//	}


//			virtual Object create() const = 0;
			//const Object &operator[](const std::string& path) const;
			std::string type() const;
			std::vector<std::string> attribute() const;
			template <typename Type>
			Type get() const;
			Object() {};
			Object(const void* core);
			~Object() {};
			Object(const char *src);
			Object(const std::string& src);
			Object(const int& src);
			Object(const float_t& src);
			Object(const double_t& src);
			Object(const Object& src);
			Object(const std::vector<Python::Object>& src);

			template <typename Dtype>
			Object(const Mat<Dtype>& src) {
				core_ = std::shared_ptr<const void>(python::Convert(src), python::Release);
			}

			template<typename... Atype>
			Object operator()(Atype&&... arguments) const {
				std::vector<Object> list;
				for_each(std::forward_as_tuple(arguments...), [&](const auto& argument) { list.push_back(Object(argument));});
				return this->call(list);
			}

			const void* data() const { return core_.get(); }
			Object& operator=(const Object& src);
			Object operator[](const std::string& uid) const;
			Object operator[](const int& uid) const;
			void set(const std::string& uid, const Object& src);
			void set(const int& uid, const Object& src);
//			Object operator+(const Object& src);
		protected:
			Object call(const std::vector<Object>& arguments) const;
//			std::shared_ptr<Object> &operator=(const std::shared_ptr<Object>& src);
		private:
			std::shared_ptr<const void> core_;
//			std::map<std::string, Object> attribute_;
		};

		Python();
		Python(const std::string &root);
		~Python();
		Object operator[](const std::string& name) const;
		bool Add(const std::string& directory);
		inline bool status() const { return status_; }


	protected:
		std::mutex lock_;
		void Initialize(const std::string& root);
	private:
		bool status_;
	};

//	bool ZERO_API operator[](std::shared_ptr<Python::Object> camera, std::string name);
	namespace python {
		ZERO_API bool Instance(const Python::Object& variable, const Python::Object& type);
		ZERO_API void* Convert(const void* src);
		ZERO_API void* Convert(const std::string& src);
		ZERO_API void* Convert(const char *src);
		ZERO_API void* Convert(const int& src);
		ZERO_API void* Convert(const float_t& src);
		ZERO_API void* Convert(const double_t& src);
		template <typename Dtype>
		ZERO_API void* Convert(const Mat<Dtype>& src);
		ZERO_API void* Convert(const Python::Object& src);
		ZERO_API void Release(const void* src);
	}
}