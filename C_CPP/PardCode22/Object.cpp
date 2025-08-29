#include "Object.h"

Object::Object(const std::wstring& szNameObj)
{
	std::cout << "Initialize : " << "Object" << " Class" << '\n';
	Init();
}

Object::~Object()
{
	std::cout << "Release : " << "Object" << " Class" << '\n';
	Release();
}

void Object::Init()
{
	
}

void Object::Release()
{
}

bool Object::AddComponent(std::type_index typeID, size_t idx)
{
	if (m_Components.find(typeID) != m_Components.end()) return false;
	m_Components[typeID] = idx;
	return true;
}
