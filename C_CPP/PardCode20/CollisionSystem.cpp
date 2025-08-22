#include "CollisionSystem.h"
#include "Frustum.h"
#include "Plane.h"
#include "Sphere.h"
//밑부분은 책임분리 고려필요
#include "CameraSystem.h"
#include "FirstPersonCamera.h"
#include "RenderSystem.h"	//임시, obj를받아오기위함 추후 objectSystem에서 받아와야함
#include "ResourceSystem.h"
#include "Mesh.h"
#include "TempObj.h"

CollisionSystem::CollisionSystem()
{
}

CollisionSystem::~CollisionSystem()
{
}

void CollisionSystem::Init()
{

}

void CollisionSystem::Frame(float deltatime)
{
	UINT count = 0;
	auto pCamera = _CameraSystem.GetCamera(0);
	Frustum frustum(pCamera->GetFarZ(), pCamera->GetViewMatrix(), pCamera->GetProjMatrix());
	for (const auto& iter : _RenderSystem.objs)
	{
		iter->bRenderable = false;
		Matrix4x4 matWorld = GetMat_WorldMatrix(iter->m_vScale, iter->m_vRotate, iter->m_vPosition);
		for (const auto hash : iter->m_hashMeshes)
		{
			size_t idxCollider = _ResourceSystem.GetResource<Mesh>(hash)->GetIdx_Collider();
			if (CheckBound(frustum, *static_cast<Sphere*>(m_Colliders[idxCollider]), matWorld))
			{
				iter->bRenderable = true;
				count++;
				break;
			}
		}
	}
	std::cout << "렌더링될 객체수 : " << count << ", 컬링된 객체수 : " << _RenderSystem.objs .size() - count << '\n';
}


void CollisionSystem::Release()
{
	for (auto iter = m_Colliders.begin(); iter != m_Colliders.end();)
	{
		delete iter->second;
		iter = m_Colliders.erase(iter);
	}
}

size_t CollisionSystem::CreateCollider_Sphere(const std::wstring& szName, const std::vector<Vector3>* poss)
{
	return AddCollider<Sphere>(szName, poss);
}

bool CollisionSystem::CheckBound(const Frustum& frustum, const Sphere& sphere, const Matrix4x4& matWorld)
{
	Vector3 center = sphere.GetCenter() * matWorld;
	float maxScale = max(max(matWorld[0].GetX(), matWorld[1].GetY()), matWorld[2].GetZ());
	float radius = sphere.GetRadius() * maxScale;
	for (int i = 0; i < 6; i++)
	{
		if (frustum.GetPlanes()[i].Distance(center) < -radius)
			return false;
	}
	return true;
}

