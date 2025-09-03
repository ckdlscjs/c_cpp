#pragma once
#include "BaseSystem.h"

class Collider;
class Frustum;
class Sphere;
class Box;

class CollisionSystem : public BaseSystem<CollisionSystem>
{
	friend class BaseSystem<CollisionSystem>;	//CRTP������ ���� friend����
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
	Collider* CreateCollider(size_t hash, const std::vector<Vector3>* vertices, Colliders collider);	//�ö��̴� ���ø� Ȥ�� ���ڸ� �޾Ƽ� �����°Բ� �������ٰ��ΰ� �б��Լ�
	
private:
	template<typename T, typename... Types>
	Collider* AddCollider(size_t hash, Types... args);
	bool CheckBound(const Frustum& frustum, size_t hash, const Matrix4x4& matWorld);
	bool CheckBound(const Frustum& frustum, const Sphere& sphere, const Matrix4x4& matWorld);
	bool CheckBound(const Frustum& frustum, const Box& box, const Matrix4x4& matWorld);

	std::unordered_map<size_t, Collider*> m_Colliders;
	size_t m_lColliderID;
};
#define _CollisionSystem CollisionSystem::GetInstance()

template<typename T, typename ...Types>
inline Collider* CollisionSystem::AddCollider(size_t hash, Types ...args)
{
	if (m_Colliders.find(hash) != m_Colliders.end()) return m_Colliders[hash];
	//��ü������ �����̳ʿ� ���
	T* newCollider = new T(std::forward<Types>(args)...);
	_ASEERTION_NULCHK(newCollider, typeid(T).name());
	m_Colliders[hash] = newCollider;
	return newCollider;
}
