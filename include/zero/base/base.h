#pragma once
#include <stdexcept>
#include <vector>
namespace zero {

	class Object {
	public:
		Object() {};
		virtual ~Object() {};
	};
	template <typename Type>
	class Template : public Object {
	public:
		Template(const Type &data) :data_(data) {};
		
		const Type& get() const { return data_; }
		Type& get() { return data_; }
	private:
		Type data_;
	};

	template<typename Type>
	Type RemoveReference(const Type& s) {
		throw std::runtime_error("this function can't callable.");
		return Type();
	}

	template <typename Type>
	struct Range {
		Type minimum;
		Type maximum;
	};

	template <typename Type>
	class Storage: public Object {
	public:
		virtual inline bool empty() const {
			return this->datas_.empty();
		}
		virtual inline void clear() {
			datas_.clear();
		}

	protected:
		Type datas_;
	};

	template <typename Type> 
	class Iterator: virtual public Storage<Type> {
	public:
		using iterator = typename Type::iterator;
		using const_iterator = typename Type::const_iterator;

		virtual inline iterator begin() {
			return this->datas_.begin();
		}
		virtual inline const_iterator begin() const {
			return this->datas_.begin();
		}
		virtual inline iterator end() {
			return this->datas_.end();
		}

		virtual inline const_iterator end() const {
			return this->datas_.end();
		}

		virtual inline size_t size() const {
			return this->datas_.size();
		}
	};


	template <typename Type, typename Stype>
	class Array: virtual public Storage<Stype> {
	public:
		Array() {};
		~Array() {};
		virtual inline Type &operator[](const int id) {
			if (id >= 0) {
				return this->datas_[id];
			}
			else {
				int size = static_cast<int>(this->datas_.size());
				return this->datas_[id + size];
			}
		}
		virtual inline const Type &operator[](const int id) const {
			if (id >= 0) {
				return this->datas_[id];
			}
			else {
				int size = static_cast<int>(this->datas_.size());
				return this->datas_[id + size];
			}
		}
		virtual void resize(const size_t size, const Type &value) {
			this->datas_.resize(size, value);
		}
		virtual void resize(const size_t size) {
			this->datas_.resize(size);
		}
		virtual inline void push_back(const Type &data) {
			this->datas_.push_back(data);
		}
	};

	template <typename Type, typename Stype>
	class Array<Type&, Stype>: virtual public Storage<Stype> {
	public:
		Array() {};
		~Array() {
			clear();
		};
		virtual inline Type &operator[](const int id) {
			if (id >= 0) {
				return *(this->datas_[id]);
			}
			else {
				int size = static_cast<int>(this->datas_.size());
				return *(this->datas_[id + size]);
			}
		}
		virtual inline const Type &operator[](const int id) const {
			if (id >= 0) {
				return *(this->datas_[id]);
			}
			else {
				int size = static_cast<int>(this->datas_.size());
				return *(this->datas_[id + size]);
			}
		}
		template<typename... Atype>
		Type * New(Atype&&... args) {
			auto pt = new Type(args...);
			storage_.push_back(pt);
			push_back(pt);
			return pt;
		}

		Type * New() {
			auto pt = new Type;
			storage_.push_back(pt);
			push_back(pt);
			return pt;
		}
//		template <typename ...Ptype>
//		virtual inline Type *New(Ptype&&... args) {
//			auto ptr = std::make_shared<Type>(args...);
//			storage_.pop_back(ptr);
//			return ptr;
//			return nullptr;
//		}

		virtual void resize(const size_t size, Type *value) {
			this->datas_.resize(size, value);
		}
		virtual void resize(const size_t size) {
			this->datas_.resize(size);
		}
		virtual inline void push_back(Type *data) {
			this->datas_.push_back(data);
		}

		virtual inline void clear() {
			Storage<Stype>::clear();
			for (const auto &storage : storage_) {
				if(storage) delete storage;
			}
			storage_.clear();
		}
	private:
		std::vector<Type *> storage_;
	};


	template<size_t id = 0, typename FType, typename ...AType>
	inline typename std::enable_if_t<id == sizeof ...(AType)> for_each(std::tuple<AType ...>&, FType) { }

	template<size_t id = 0, typename FType, typename ...AType>
	inline typename std::enable_if_t < id < sizeof ...(AType)> for_each(std::tuple<AType ...>& argv, FType &function) {
		function(std::get<id>(argv));
		for_each<id + 1, FType, AType...>(argv, function);
	}

	template<size_t id = 0, typename FType, typename ...AType>
	inline typename std::enable_if_t<id == sizeof ...(AType)> for_each(std::tuple<AType& ...>, FType) { }
	
	template<size_t id = 0, typename FType, typename ...AType>
	inline typename std::enable_if_t < id < sizeof ...(AType)> for_each(std::tuple<AType& ...> argv, FType function) {
		function(std::get<id>(argv));
		for_each<id + 1, FType, AType...>(argv, function);
	}

}