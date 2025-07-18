#pragma once
#include "std.h"
template<typename T>
class BaseSystem
{
protected:
	BaseSystem();
	BaseSystem(const BaseSystem&) = delete;				//복사생성자삭제
	BaseSystem& operator=(const BaseSystem&) = delete;	//복사대입연산자삭제
	BaseSystem(BaseSystem&&) = delete;					//이동생성자삭제
	BaseSystem& operator=(BaseSystem&&) = delete;		//이동대입연산자삭제
	virtual ~BaseSystem();								

public:
	static T& GetInstance();
};

template<typename T>
inline BaseSystem<T>::BaseSystem()
{
	std::cout << "BaseSystem <" << typeid(T).name() << "> Class" << " 생성자 호출" << '\n';
}

template<typename T>
inline BaseSystem<T>::~BaseSystem()
{
	std::cout << "BaseSystem <" << typeid(T).name() << "> Class" << " 소멸자 호출" << '\n';
}

template<typename T>
inline T& BaseSystem<T>::GetInstance()
{
	static T instance;
	return instance;
}