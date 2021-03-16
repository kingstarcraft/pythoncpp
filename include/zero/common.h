#pragma once
#include <stdint.h>
//#include <corecrt_math.h>

#ifdef ZERO_EXPORTS
#define ZERO_API __declspec(dllexport)

#define INSTANTIATE_CLASS(classname)                    \
    template ZERO_API class classname<uint8_t>;         \
    template ZERO_API class classname<uint16_t>;        \
    template ZERO_API class classname<uint32_t>;        \
    template ZERO_API class classname<uint64_t>;        \
    template ZERO_API class classname<int8_t>;          \
    template ZERO_API class classname<int16_t>;         \
    template ZERO_API class classname<int32_t>;         \
    template ZERO_API class classname<int64_t>;         \
    template ZERO_API class classname<float_t>;         \
    template ZERO_API class classname<double_t>;
#else
#define ZERO_API __declspec(dllimport)
#endif

#define SET_VARIABLE_ACCESSIBLE(type, name)             \
    public:                                             \
    inline type &name() {                               \
        return name##_;                                 \
    }                                                   \
    inline type name() const {                          \
        return name##_;                                 \
    }

#define DEFINE_ACCESSIBLE_PRIVATE_VARIABLE(type, name)  \
private:                                                \
	type name##_;                                       \
	SET_VARIABLE_ACCESSIBLE(type, name)

#define DEFINE_ACCESSIBLE_PROTECT_VARIABLE(type, name)  \
protected:                                              \
	type name##_;                                       \
	SET_VARIABLE_ACCESSIBLE(type, name)

#define DEFINE_CONST_TYPE(type)                         \
   typedef const type const_##type;

DEFINE_CONST_TYPE(int)
DEFINE_CONST_TYPE(float)
DEFINE_CONST_TYPE(double)

#define STRING(name) #name





//  namespace kfbio {
//  	typedef char Dtype;
//  	typedef int Ktype;
//  	//	template<typename Dtype>
//  	class Array {
//  	public:
//  		class Iterator {
//  		public:
//  			Iterator(const Array &param)
//  				: param_(param) {};
//  			~Iterator() {};
//  
//  
//  		private:
//  			const Array &param_;
//  		};
//  
//  		virtual A &operator[](B pos) = 0;
//  		virtual A operator[](int pos) const = 0;
//  
//  		Iterator &begine() {
//  
//  		}
//  
//  
//  	private:
//  		B pos_;
//  	};
//  }

//  namespace kfbio {
//  	template<typename Dtype>
//  	class vector : public std::vector<Dtype> {
//  	public:
//  		Dtype operator[](int id) const {
//  			return std::vector<int>::operator[](CanonicalAxisIndex(id));
//  		}
//  		Dtype &operator[](int id) {
//  			return std::vector<int>::operator[](CanonicalAxisIndex(id));
//  		}
//  
//  	private:
//  		inline size_t CanonicalAxisIndex(int id) const {
//  			return id < 0 ? id + (int)(this->size()) : id;
//  		}
//  	};
//  }