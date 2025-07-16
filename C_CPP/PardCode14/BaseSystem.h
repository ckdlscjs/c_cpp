#pragma once
#include "std.h"
template<typename T>
class BaseSystem
{
protected:
	BaseSystem();
	BaseSystem(const BaseSystem&) = delete;				//��������ڻ���
	BaseSystem& operator=(const BaseSystem&) = delete;	//������Կ����ڻ���
	BaseSystem(BaseSystem&&) = delete;					//�̵������ڻ���
	BaseSystem& operator=(BaseSystem&&) = delete;		//�̵����Կ����ڻ���
	virtual ~BaseSystem();								

public:
	static T& GetInstance();
};

template<typename T>
inline BaseSystem<T>::BaseSystem()
{
	std::cout << "BaseSystem <" << typeid(T).name() << "> Class" << " ������ ȣ��" << '\n';
}

template<typename T>
inline BaseSystem<T>::~BaseSystem()
{
	std::cout << "BaseSystem <" << typeid(T).name() << "> Class" << " �Ҹ��� ȣ��" << '\n';
}

template<typename T>
inline T& BaseSystem<T>::GetInstance()
{
	static T instance;
	return instance;
}