#pragma once
#include "std.h"
class Resource
{
public:
	Resource() {}
	virtual ~Resource() {}
	Resource(const Resource&) = delete;
	Resource& operator=(const Resource&) = delete;
	Resource(Resource&&) = delete;
	Resource& operator=(Resource&&) = delete;
	virtual const std::wstring& GetPath() const = 0;
};
template<typename T>
class BaseResource : public Resource
{
protected:
	BaseResource();
	virtual ~BaseResource();
	BaseResource(const BaseResource&) = delete;
	BaseResource& operator=(const BaseResource&) = delete;
	BaseResource(BaseResource&&) = delete;
	BaseResource& operator=(BaseResource&&) = delete;
public:
	const std::wstring& GetPath() const override;
protected:
	std::wstring m_szFilePath;
	size_t m_lResourceID;
};

template<typename T>
inline BaseResource<T>::BaseResource()
{
	std::cout << "BaseResource <" << typeid(T).name() << "> Class" << " 생성자 호출" << '\n';
}

template<typename T>
inline BaseResource<T>::~BaseResource()
{
	std::cout << "BaseResource <" << typeid(T).name() << "> Class" << " 소멸자 호출" << '\n';
}

template<typename T>
inline const std::wstring& BaseResource<T>::GetPath() const
{
	return m_szFilePath;
}
