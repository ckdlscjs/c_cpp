#pragma once
#include "CommonHeader.h"

template<typename T>
class BaseSystem
{
protected:
	BaseSystem();
	~BaseSystem();								
	BaseSystem(const BaseSystem&) = delete;				//��������ڻ���
	BaseSystem& operator=(const BaseSystem&) = delete;	//������Կ����ڻ���
	BaseSystem(BaseSystem&&) = delete;					//�̵������ڻ���
	BaseSystem& operator=(BaseSystem&&) = delete;		//�̵����Կ����ڻ���

public:
	static T& GetInstance();
};

template<typename T>
inline BaseSystem<T>::BaseSystem()
{
	std::cout << "Initialize : " << "BaseSystem <" << typeid(T).name() << "> Class" << " ������ ȣ��" << '\n';
}

template<typename T>
inline BaseSystem<T>::~BaseSystem()
{
	std::cout << "Release : " << "BaseSystem <" << typeid(T).name() << "> Class" << " �Ҹ��� ȣ��" << '\n';
}

template<typename T>
inline T& BaseSystem<T>::GetInstance()
{
	static T instance;
	return instance;
}