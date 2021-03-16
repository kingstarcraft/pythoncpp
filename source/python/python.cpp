#include "zero/python/python.h"
#include <filesystem>
#include <Python.h>
#include <numpy/arrayobject.h>
#include <iostream>
#include "zero/base/algorithm.h"



namespace zero {
	namespace python {
		class Mutex {
		public:
			Mutex() {
				gil_state_ = PyGILState_Check();
				if (gil_state_ == 0) { gil_lock_ = PyGILState_Ensure(); }
				thread_lock_ = PyEval_SaveThread();
				PyEval_RestoreThread(thread_lock_);
//				std::cout << "lock" << std::endl;
			}

			~Mutex() {
				thread_lock_ = PyEval_SaveThread();
				PyEval_RestoreThread(thread_lock_);
				if (gil_state_ == 0) { PyGILState_Release(gil_lock_);  gil_state_ = 1; }
//				std::cout << "unlock" << std::endl;
			}

		private:
			int gil_state_;
			PyGILState_STATE gil_lock_;
			PyThreadState* thread_lock_;
			const Mutex& operator=(const Mutex&src) const{
				return *this;
			}
			//std::mutex mutex_;
		};

		void* Convert(const void* src) {
			return (void *)src;
		}
		void *Convert(const std::string& src) {
			Mutex mutex;
			return Py_BuildValue("s", src.data());
		}
		void* Convert(const char* src) {
			Mutex mutex;
			return Py_BuildValue("s", src);
		}
		void * Convert(const int& src) {
			Mutex mutex;
//			std::lock_guard<Mutex> lock(mutex);
			return Py_BuildValue("i", src);;
		}

		void * Convert(const double& src) {
			Mutex mutex;
//			std::lock_guard<Mutex> lock(mutex);
			return Py_BuildValue("d", src);;
		}

		void *Convert(const float& src) {
			Mutex mutex;
//			std::lock_guard<Mutex> lock(mutex);
			return Py_BuildValue("d", src);;
		}

		void* Convert(const std::vector<Python::Object>& src) {
			Mutex mutex;
//			std::lock_guard<Mutex> lock(mutex);
			auto dst = PyTuple_New(src.size());
			for (size_t i = 0; i < src.size(); i++) {
				Py_INCREF(src.at(i).data());
				PyTuple_SetItem(dst, i, (PyObject*)src.at(i).data());
			}
			return dst;
		}

		//	template <typename Dtype>
		//	void* Convert(const std::vector<Dtype>& src) {
		//		auto dst = PyList_New(src.size());
		//		for (size_t i = 0; i < src.size(); i++) {
		//			auto object = python::Convert(src[i]);
		//			PyTuple_SetItem(dst, i, (PyObject*)src.at(i).data());
		//		}
		//	
		//	}

		bool Instance(const Python::Object& variable, const Python::Object& type) {
			Mutex mutex;
//			std::lock_guard<Mutex> lock(mutex);
			return PyObject_IsInstance((PyObject*)variable.data(), (PyObject*)type.data());
		}
		/*
template ZERO_API class classname<uint16_t>;        \
template ZERO_API class classname<uint32_t>;        \
template ZERO_API class classname<uint64_t>;        \
template ZERO_API class classname<int8_t>;          \
template ZERO_API class classname<int16_t>;         \
template ZERO_API class classname<int32_t>;         \
template ZERO_API class classname<int64_t>;         \
template ZERO_API class classname<float_t>;         \
template ZERO_API class classname<double_t>;
*/


		template<typename Dtype>
		void* Convert(const Mat<Dtype>& src) {
			std::vector<npy_intp> dims;
			for (auto dim : src.shape()) { dims.push_back(dim); }
			
			auto dtype = NPY_UINT8;
			if (typeid(Dtype) == typeid(int8_t)) { dtype = NPY_INT8; }
			else if (typeid(Dtype) == typeid(int16_t)) { dtype = NPY_INT16; }
			else if (typeid(Dtype) == typeid(int32_t)) { dtype = NPY_INT32; }
			else if (typeid(Dtype) == typeid(int64_t)) { dtype = NPY_INT64; }
			else if (typeid(Dtype) == typeid(uint8_t)) { dtype = NPY_UINT8; }
			else if (typeid(Dtype) == typeid(uint16_t)) { dtype = NPY_UINT16; }
			else if (typeid(Dtype) == typeid(uint32_t)) { dtype = NPY_UINT32; }
			else if (typeid(Dtype) == typeid(uint64_t)) { dtype = NPY_UINT64; }
			else if (typeid(Dtype) == typeid(float_t)) { dtype = NPY_FLOAT32; }
			else if (typeid(Dtype) == typeid(double_t)) { dtype = NPY_FLOAT64; }
			else { throw std::logic_error(std::string("Can't convert ")+ typeid(Dtype).name() +" to python type."); }
			Mutex mutex;
			return PyArray_SimpleNewFromData(dims.size(), dims.data(), dtype, (void*)src.data());
		}

		void* Convert(const Python::Object& src) {
			return (void*)(src.get<void *>());
		}

		void Release(const void* src) {
			Mutex mutex;
//			std::lock_guard<Mutex> lock(mutex);
			Py_DECREF(src);
		}


		enum class Type {
			FUNCTION = 0, 
			VARIABLE = 1,
			CLASS = 2,
			MODULE = 3, 
		};

		Type type(const void* src) {
			auto name = ((PyObject*)src)->ob_type->tp_name;
			if (std::string("module") == name) {
				return Type::MODULE;
			}
			return Type::VARIABLE;

		}
	}

	namespace cpp {

		template<typename Type>
		Type Convert(const void* src) {
			python::Mutex mutex;
			return PyFloat_AsDouble((PyObject*)src);
		}

		template<>
		Python::Object Convert(const void* src) {
			return Python::Object(src);
		}

		template<>
		std::string Convert(const void* src) {
			python::Mutex mutex;
			auto buff = PyUnicode_AsUnicode((PyObject *)src);
			auto dst = zero::Convert(buff);
//			PyMem_RawFree(buff);
			return dst;
		}

		template<> void *Convert(const void* src) { return (void *)src; }


		template<typename SType, typename DType>
		static void copy(const SType *src, const npy_intp *shape, const npy_intp *stride, const int nd, const size_t dim, DType* dst) {
			if (nd <= 0) { return; }
			if (dim == nd - 1) {
				for (int i = 0; i < shape[dim]; i++) {
					dst[i] = src[i];
				}
			}
			else {
				for (int i = 0; i < shape[dim]; i++) {
					auto s_offset = src + i * stride[dim] / sizeof(SType);
					auto d_stride = 1;
					for (int j = dim+1; j < nd; j++) { d_stride *= shape[j]; }
					auto d_offset = dst + i*d_stride;
					copy(s_offset, shape, stride, nd, dim + 1, d_offset);
				}
			}
		}


		template<typename DType>
		Mat<DType> ConvertMat(const void* src) {
			PyArrayObject* numpy = (PyArrayObject*)src;
			std::vector<size_t> shape(numpy->nd);
			for (int i = 0; i < numpy->nd; ++i) { shape[i] = numpy->dimensions[i]; }
			Mat<DType> mat(shape);
			NPY_TYPES dtype = (NPY_TYPES)numpy->descr->type_num;
			if (dtype == NPY_INT8) { copy((int8_t*)numpy->data, numpy->dimensions, numpy->strides, numpy->nd, 0, (DType*)mat.data()); }
			else if (dtype == NPY_INT16) { copy((int16_t*)numpy->data, numpy->dimensions, numpy->strides, numpy->nd, 0, (DType*)mat.data()); }
			else if (dtype == NPY_INT32) { copy((int32_t*)numpy->data, numpy->dimensions, numpy->strides, numpy->nd, 0, (DType*)mat.data()); }
			else if (dtype == NPY_INT64) { copy((int64_t*)numpy->data, numpy->dimensions, numpy->strides, numpy->nd, 0, (DType*)mat.data()); }
			else if (dtype == NPY_UINT8) { copy((uint8_t*)numpy->data, numpy->dimensions, numpy->strides, numpy->nd, 0, (DType*)mat.data()); }
			else if (dtype == NPY_UINT16) { copy((uint16_t*)numpy->data, numpy->dimensions, numpy->strides, numpy->nd, 0, (DType*)mat.data()); }
			else if (dtype == NPY_UINT32) { copy((uint32_t*)numpy->data, numpy->dimensions, numpy->strides, numpy->nd, 0, (DType*)mat.data()); }
			else if (dtype == NPY_FLOAT32) { copy((float_t*)numpy->data, numpy->dimensions, numpy->strides, numpy->nd, 0, (DType*)mat.data()); }
			else if (dtype == NPY_FLOAT64) { copy((double_t*)numpy->data, numpy->dimensions, numpy->strides, numpy->nd, 0, (DType*)mat.data()); }
			else { throw std::logic_error(std::string("Can't convert dtype=(") + std::to_string(dtype) + ") numpy to C++ mat"); }
			return mat;
		}

		template<typename Type>
		std::vector<Type> ConvertVector(const void* src) {
			python::Mutex mutex;
//			std::lock_guard<python::Mutex> lock(mutex);
			std::vector<Type> reval;
			auto get_size = PyTuple_Size;
			auto get_item = PyTuple_GetItem;
			if (PyList_Check(src)) {
				get_size = PyList_Size;
				get_item = PyList_GetItem;
			}
			else if (!PyTuple_Check(src)) { return reval; }
			for (int i = 0; i < get_size((PyObject*)src); i++) {
				auto value = get_item((PyObject*)src, i);
				Py_INCREF(value);
				reval.push_back(Convert<Type>(value));
			}
			return reval;
		}

		template<> Mat<int8_t> Convert(const void* src) { return ConvertMat<int8_t>(src); }
		template<> Mat<int16_t> Convert(const void* src) { return ConvertMat<int16_t>(src); }
		template<> Mat<int32_t> Convert(const void* src) { return ConvertMat<int32_t>(src); }
		template<> Mat<int64_t> Convert(const void* src) { return ConvertMat<int64_t>(src); }
		template<> Mat<uint8_t> Convert(const void* src) { return ConvertMat<uint8_t>(src); }
		template<> Mat<uint16_t> Convert(const void* src) { return ConvertMat<uint16_t>(src); }
		template<> Mat<uint32_t> Convert(const void* src) { return ConvertMat<uint32_t>(src); }
		template<> Mat<uint64_t> Convert(const void* src) { return ConvertMat<uint64_t>(src); }
		template<> Mat<float_t> Convert(const void* src) { return ConvertMat<float_t>(src); }
		template<> Mat<double_t> Convert(const void* src) { return ConvertMat<double_t>(src); }

		template<> std::vector<int8_t> Convert(const void* src) { return ConvertVector<int8_t>(src); }
		template<> std::vector<int16_t> Convert(const void* src) { return ConvertVector<int16_t>(src); }
		template<> std::vector<int32_t> Convert(const void* src) { return ConvertVector<int32_t>(src); }
		template<> std::vector<int64_t> Convert(const void* src) { return ConvertVector<int64_t>(src); }
		template<> std::vector<uint8_t> Convert(const void* src) { return ConvertVector<uint8_t>(src); }
		template<> std::vector<uint16_t> Convert(const void* src) { return ConvertVector<uint16_t>(src); }
		template<> std::vector<uint32_t> Convert(const void* src) { return ConvertVector<uint32_t>(src); }
		template<> std::vector<uint64_t> Convert(const void* src) { return ConvertVector<uint64_t>(src); }
		template<> std::vector<float_t> Convert(const void* src) { return ConvertVector<float_t>(src); }
		template<> std::vector<double_t> Convert(const void* src) { return ConvertVector<double_t>(src); }
		template<> std::vector<std::string> Convert(const void* src) { return ConvertVector<std::string>(src); }
		template<> std::vector<Python::Object> Convert(const void* src) { return ConvertVector<Python::Object>(src); }
	}



	Python::Python() {
#ifdef _WIN32
		auto path = zero::Execute("where python3.dll");
#else
		auto path = zero::Execute("which python3");
#endif
		if (path.empty()) { return; }
		for (auto& p : path) { if (p == '\\') { p = '/'; } }

		std::filesystem::path root(path);
		this->Initialize(root.parent_path().string().data());
		this->Add(root.parent_path().string() + "/Lib");
	}

	Python::Python(const std::string& root) {
		Initialize(root);
	}


	void Python::Initialize(const std::string& root) {
		status_ = false;
		auto wroot = Py_DecodeLocale(root.c_str(), nullptr);
		Py_SetPythonHome(wroot);
		try { 
			Py_Initialize();
			PyEval_InitThreads();
			int init = PyEval_ThreadsInitialized();
			if (init) { PyEval_SaveThread(); };
			python::Mutex lock;
			if (PyArray_API == nullptr) { status_ = _import_array() >= 0; }
		}
		catch (const std::exception& error) {
			python::Mutex lock;
			PyMem_RawFree(wroot);
			throw error;
		}
		python::Mutex lock;
		status_ = status_ and (PyRun_SimpleString("import sys") == 0);
		PyMem_RawFree(wroot);
	}

	bool Python::Add(const std::string& directory) {
		std::string command = std::string("sys.path.append('") + directory + "')";
		python::Mutex lock;
		return PyRun_SimpleString(command.data()) == 0;
	}

	Python::~Python() {
		auto gil_state = PyGILState_Check();
		if (gil_state == 0) { auto gil_lock = PyGILState_Ensure(); }
		//thread_lock_ = PyEval_SaveThread();
		//PyEval_RestoreThread(thread_lock_);
		Py_Finalize();
	}



	//	std::vector<std::string> Python::Object::attribute() {
	//		return std::vector<std::string>();
	//	}

	Python::Object Python::operator[](const std::string& name) const{
		python::Mutex mutex;
//		std::lock_guard<python::Mutex> lock(mutex);
		auto module = PyImport_ImportModule(name.data());
		if (module == nullptr) {
			PyErr_Print();
			PyObject* ptype, * pvalue, * ptraceback;
			PyErr_Fetch(&ptype, &pvalue, &ptraceback);
		}
		return Python::Object(module);
	}

//	Python::Object Python::Object::operator+(const Python::Object& src) {
//		return this->operator[]("__add__")(src);
//	}


	std::vector<std::string> Python::Object::attribute() const{
		python::Mutex mutex;
//		std::lock_guard<python::Mutex> lock(mutex);
		auto objects = PyObject_Dir((PyObject*)(core_.get()));
		if (objects == nullptr) { return std::vector<std::string>(); }
		auto names = cpp::Convert<std::vector<std::string>>(objects);
		Py_DECREF(objects);
		return names;
	}

	Python::Object::Object(const void* core) {
		if (core == nullptr) { return; }
		core_ = std::shared_ptr<const void>(core, python::Release);
	}

	/*

		Python::Object Convert(const int& src) {
			return Python::Object(Py_BuildValue("i", src));
		}

		Python::Object Convert(const double& src) {
			return Python::Object(Py_BuildValue("d", src));
		}

		Python::Object Convert(const float& src) {
			return Python::Object(Py_BuildValue("d", src));
		}
	*/

	Python::Object::Object(const std::string& src) {
		core_ = std::shared_ptr<const void>(python::Convert(src), python::Release);
	}
	Python::Object::Object(const char* src) {
		core_ = std::shared_ptr<const void>(python::Convert(src), python::Release);
	}

	Python::Object::Object(const int& src) {
		core_ = std::shared_ptr<const void>(python::Convert(src), python::Release);
	}
	Python::Object::Object(const float_t& src) {
		core_ = std::shared_ptr<const void>(python::Convert(src), python::Release);
	}

	Python::Object::Object(const double_t& src) {
		core_ = std::shared_ptr<const void>(python::Convert(src), python::Release);
	}
	Python::Object::Object(const Python::Object &src) {
		*this = src;
	};

	Python::Object::Object(const std::vector<Python::Object>& src) {
		core_ = std::shared_ptr<const void>(python::Convert(src), python::Release);
	};

	std::string Python::Object::type() const {
		auto core = (PyObject*)(core_.get());
		if (core == nullptr || core->ob_type == nullptr) { return "null"; }
		if (std::string(core->ob_type->tp_name) == "NoneType") return "None";
		return core->ob_type->tp_name;
	}

	Python::Object Python::Object::call(const std::vector<Python::Object>& arguments) const{
		PyObject* src = nullptr;
		if (arguments.size() > 0) {
			src = (PyObject *)python::Convert(arguments);
		}
		if (core_.get() == nullptr) {
			throw std::runtime_error("None variable can't be called in python.");
		}

		try { 
			//Py_BEGIN_ALLOW_THREADS
			std::cout << arguments.size();
			python::Mutex mutex;
//			std::lock_guard<python::Mutex> lock(mutex);
			auto dst = PyObject_CallObject((PyObject*)core_.get(), src);
			if (src != nullptr) Py_DECREF(src);
			if (dst == nullptr) return Python::Object(dst);
			return Python::Object(dst);
		}
		catch (const std::exception& error) { 
			if (src != nullptr) { 
				python::Mutex mutex;
//				std::lock_guard<python::Mutex> lock(mutex);
				Py_DECREF(src);
			}
			throw error; 
		}

	}

	Python::Object &Python::Object::operator=(const Python::Object &src) {
		core_ = src.core_;
		return *this;
	}

	Python::Object Python::Object::operator[](const std::string& src) const{
		python::Mutex mutex;
//		std::lock_guard<python::Mutex> lock(mutex);
		return Python::Object(PyObject_GetAttrString((PyObject*)core_.get(), src.data()));;
	}

	Python::Object Python::Object::operator[](const int& uid) const {
//		auto function_size = PyTuple_Size;
		python::Mutex mutex;
//		std::lock_guard<python::Mutex> lock(mutex);
		auto get_item = PyTuple_GetItem;
		if (PyList_Check(core_.get())) {  get_item = PyList_GetItem; }
		return Python::Object(get_item((PyObject*)core_.get(), uid));
	}

	void Python::Object::set(const std::string& uid, const Object& src) {
		//Py_INCREF(src.data());
		python::Mutex mutex;
//		std::lock_guard<python::Mutex> lock(mutex);
		PyObject_SetAttrString((PyObject*)core_.get(), uid.data(), (PyObject*)src.data());
	}

	void Python::Object::set(const int& uid, const Python::Object& src) {
		//Py_INCREF(src.data());
		python::Mutex mutex;
//		std::lock_guard<python::Mutex> lock(mutex);
		auto set_item = PyTuple_SetItem;
		if (PyList_Check(core_.get())) { set_item = PyTuple_SetItem; }
		set_item((PyObject*)core_.get(), uid, (PyObject*)src.data());
	}

//	const Python::Object &Python::Object::operator[](const std::string& src) const {
//		auto find = attribute_.find(src);
//		if (find == attribute_.end()) {
//			Python::Object dst(nullptr);
//			return dst;
//		}
//		else { return find->second; }
//	}

	template <typename Type>
	Type Python::Object::get() const{
		if (this->type() == "None") { 
			throw std::runtime_error(std::string("Can't convert None type to ") + typeid(Type).name());
		}
		return cpp::Convert<Type>(core_.get());
	}

	template ZERO_API int8_t Python::Object::get() const;
	template ZERO_API int16_t Python::Object::get() const;
	template ZERO_API int32_t Python::Object::get() const;
	template ZERO_API int64_t Python::Object::get() const;
	template ZERO_API uint8_t Python::Object::get() const;
	template ZERO_API uint16_t Python::Object::get() const;
	template ZERO_API uint32_t Python::Object::get() const;
	template ZERO_API uint64_t Python::Object::get() const;
	template ZERO_API float_t Python::Object::get() const;
	template ZERO_API double_t Python::Object::get() const;
	template ZERO_API std::string Python::Object::get() const;

	template ZERO_API Mat<int8_t> Python::Object::get() const;
	template ZERO_API Mat<int16_t> Python::Object::get() const;
	template ZERO_API Mat<int32_t> Python::Object::get() const;
	template ZERO_API Mat<int64_t> Python::Object::get() const;
	template ZERO_API Mat<uint8_t> Python::Object::get() const;
	template ZERO_API Mat<uint16_t> Python::Object::get() const;
	template ZERO_API Mat<uint32_t> Python::Object::get() const;
	template ZERO_API Mat<uint64_t> Python::Object::get() const;
	template ZERO_API Mat<float_t> Python::Object::get() const;
	template ZERO_API Mat<double_t> Python::Object::get() const;

	template ZERO_API std::vector<int8_t> Python::Object::get() const;
	template ZERO_API std::vector<int16_t> Python::Object::get() const;
	template ZERO_API std::vector<int32_t> Python::Object::get() const;
	template ZERO_API std::vector<int64_t> Python::Object::get() const;
	template ZERO_API std::vector<uint8_t> Python::Object::get() const;
	template ZERO_API std::vector<uint16_t> Python::Object::get() const;
	template ZERO_API std::vector<uint32_t> Python::Object::get() const;
	template ZERO_API std::vector<uint64_t> Python::Object::get() const;
	template ZERO_API std::vector<float_t> Python::Object::get() const;
	template ZERO_API std::vector<double_t> Python::Object::get() const;
	template ZERO_API std::vector<std::string> Python::Object::get() const;
	template ZERO_API std::vector<Python::Object> Python::Object::get() const;


	template ZERO_API void* python::Convert(const Mat<int8_t>& src);
	template ZERO_API void* python::Convert(const Mat<int16_t>& src);
	template ZERO_API void* python::Convert(const Mat<int32_t>& src);
	template ZERO_API void* python::Convert(const Mat<int64_t>& src);
	template ZERO_API void* python::Convert(const Mat<uint8_t>& src);
	template ZERO_API void* python::Convert(const Mat<uint16_t>& src);
	template ZERO_API void* python::Convert(const Mat<uint32_t>& src);
	template ZERO_API void* python::Convert(const Mat<uint64_t>& src);
	template ZERO_API void* python::Convert(const Mat<float_t>& src);
	template ZERO_API void* python::Convert(const Mat<double_t>& src);
}