#pragma once
#include "BaseSystem.h"
class Collider;
class Frustum;
class Sphere;
class CollisionSystem : public BaseSystem<CollisionSystem>
{
	friend class BaseSystem<CollisionSystem>;	//CRTP패턴을 위한 friend선언
private:
	CollisionSystem();
	~CollisionSystem();
	CollisionSystem(const CollisionSystem&) = delete;
	CollisionSystem& operator=(const CollisionSystem&) = delete;
	CollisionSystem(CollisionSystem&&) = delete;
	CollisionSystem& operator=(CollisionSystem&&) = delete;
public:
	void Init();
	void Frame(float deltatime);
	void Release();
	size_t CreateCollider_Sphere(const std::wstring& szName, const std::vector<Vector3>* poss);
private:
	template<typename T, typename... Types>
	size_t AddCollider(const std::wstring& szName, Types... args);

	bool CheckBound(const Frustum& frustum, const Sphere& sphere, const Matrix4x4& matWorld);

	std::unordered_map<size_t, Collider*> m_Colliders;
	size_t m_lColliderID;
};
#define _CollisionSystem CollisionSystem::GetInstance()

template<typename T, typename ...Types>
inline size_t CollisionSystem::AddCollider(const std::wstring& szName, Types ...args)
{
	size_t hashID = HashFilePath(szName);
	if (m_Colliders.find(hashID) != m_Colliders.end()) return hashID;
	//객체생성후 컨테이너에 등록
	T* newResource = new T(std::forward<Types>(args)...);
	_ASEERTION_NULCHK(newResource, typeid(T).name());
	m_Colliders[hashID] = newResource;
	return hashID;
}
