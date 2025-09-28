#pragma once
#include "CommonHeader.h"

template<typename T>
class BaseState
{
public:
	BaseState();
	virtual ~BaseState();
	BaseState(const BaseState&) = delete;
	BaseState& operator=(const BaseState&) = delete;
	BaseState(BaseState&&) = delete;
	BaseState& operator=(BaseState&&) = delete;
protected:
	std::vector<T*> m_pStates;
};

template<typename T>
inline BaseState<T>::BaseState()
{
	std::cout << "Initialize : " << "BaseState <" << typeid(T).name() << "> Class" << '\n';
}

template<typename T>
inline BaseState<T>::~BaseState()
{
	std::cout << "Release : " << "BaseState <" << typeid(T).name() << "> Class" << '\n';
	for (auto iter = m_pStates.begin(); iter != m_pStates.end();)
	{
		(*iter)->Release();
		iter = m_pStates.erase(iter);
	}
}
