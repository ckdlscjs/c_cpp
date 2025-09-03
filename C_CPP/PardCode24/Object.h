#pragma once
#include "CommonHeader.h"
#include "Components.h"

class Object
{
public:
	Object(const std::wstring& szNameObj);
	~Object();
	Object(const Object&) = delete;
	Object& operator=(const Object&) = delete;
	Object(Object&&) = delete;
	Object& operator=(Object&&) = delete;

	void Init();
	void Release();
	bool AddComponent(std::type_index typeID, size_t idx);
	template<typename T>
	size_t GetComponent() const;
private:
	std::wstring m_szName;
	std::unordered_map<std::type_index, size_t> m_Components;
};

template<typename T>
inline size_t Object::GetComponent() const
{
	if (m_Components.find(typeid(T)) == m_Components.end()) return 0;
	return m_Components[typeid(T)];
}
