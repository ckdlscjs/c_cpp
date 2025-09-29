#pragma once
#include "BaseSystem.h"
class Object;

class ObjectSystem : public BaseSystem<ObjectSystem>
{
	friend class BaseSystem<ObjectSystem>;	//CRTP패턴을 위한 friend선언
private:
	ObjectSystem();
	ObjectSystem(const ObjectSystem&) = delete;
	ObjectSystem& operator=(const ObjectSystem&) = delete;
	ObjectSystem(ObjectSystem&&) = delete;
	ObjectSystem& operator=(ObjectSystem&&) = delete;
public:
	Object* CreateObject(const std::wstring& szNameObj);
	size_t AddObject(Object* object);
	bool DeleteObject(size_t objectIdx);
	void Init();
	void Release();

private:
	std::unordered_map<size_t, Object*> m_Objects;
	size_t m_lObjectID;
};
#define _ObjectSystem ObjectSystem::GetInstance()

