#ifndef __GENERIC_SINGLETON_HPP__
#define __GENERIC_SINGLETON_HPP__
#include <memory>

template<class T>
class GenericSingleton {
public:
	static void Init() {
		s_instance = std::make_unique<T>();
	}

	static void Init(std::unique_ptr<T> instance) {
		s_instance = std::move(instance);
	}

	static T* GetRef() noexcept {
		return s_instance.get();
	}

protected:
	static std::unique_ptr<T> s_instance;
};
#endif
