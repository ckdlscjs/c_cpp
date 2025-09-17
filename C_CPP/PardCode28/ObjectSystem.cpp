#include "ObjectSystem.h"
#include "Object.h"

ObjectSystem::ObjectSystem()
{
}

ObjectSystem::~ObjectSystem()
{
}

Object* ObjectSystem::CreateObject(const std::wstring& szNameObj)
{
	return m_Objects[m_lObjectID++] = new Object(szNameObj);
}

size_t ObjectSystem::AddObject(Object* object)
{
	m_Objects[m_lObjectID] = object;
	return m_lObjectID++;
}

bool ObjectSystem::DeleteObject(size_t objectIdx)
{
	if (m_Objects.find(objectIdx) == m_Objects.end()) 
		return false;
	delete m_Objects[objectIdx];
	m_Objects.erase(objectIdx);
	return true;
}

void ObjectSystem::Init()
{
}

void ObjectSystem::Release()
{
	for (auto iter = m_Objects.begin(); iter != m_Objects.end();)
	{
		delete iter->second;
		iter = m_Objects.erase(iter);
	}
}
