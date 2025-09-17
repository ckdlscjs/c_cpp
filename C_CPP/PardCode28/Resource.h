#pragma once
#include "CommonHeader.h"

class Resource
{
public:
	Resource() {}
	virtual ~Resource() {}
	Resource(const Resource&) = delete;
	Resource& operator=(const Resource&) = delete;
	Resource(Resource&&) = delete;
	Resource& operator=(Resource&&) = delete;
	virtual void SetHash(size_t hash) = 0;
	virtual void SetFilePath(const std::wstring& szFilepath) = 0;
	virtual const size_t GetHash() const = 0;
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
protected:
	void SetHash(size_t hash) override;
	void SetFilePath(const std::wstring& szFilepath) override;
public:
	const size_t GetHash() const override;
	const std::wstring& GetPath() const override;
private:
	std::wstring m_szFilePath;
	size_t m_lResourceID;
};

template<typename T>
inline BaseResource<T>::BaseResource()
{
	std::cout << "Initialize : " << "BaseResource <" << typeid(T).name() << "> Class"  << '\n';
}

template<typename T>
inline BaseResource<T>::~BaseResource()
{
	std::cout << "Release : " << "BaseResource <" << typeid(T).name() << "> Class"  << '\n';
}

template<typename T>
inline void BaseResource<T>::SetHash(size_t hash)
{
	m_lResourceID = hash;
}

template<typename T>
inline void BaseResource<T>::SetFilePath(const std::wstring& szFilepath)
{
	m_szFilePath = szFilepath;
}

template<typename T>
inline const size_t BaseResource<T>::GetHash() const
{
	return m_lResourceID;
}

template<typename T>
inline const std::wstring& BaseResource<T>::GetPath() const
{
	return m_szFilePath;
}
