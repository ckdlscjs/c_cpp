#include "CollisionSystem.h"
#include "Frustum.h"
#include "Plane.h"
#include "Sphere.h"
#include "Box.h"
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
	UINT count = 1;
	auto pCamera = _CameraSystem.GetCamera(0);
	Frustum frustum(pCamera->GetFarZ(), pCamera->GetViewMatrix(), pCamera->GetProjMatrix());
	_RenderSystem.objs[0]->bRenderable = true;
	for (int i = 1; i < _RenderSystem.objs.size(); i++)	//i(0) skysphere
	{
		auto iter = _RenderSystem.objs[i];
		iter->bRenderable = false;
		Matrix4x4 matWorld = GetMat_WorldMatrix(iter->m_vScale, iter->m_vRotate, iter->m_vPosition);
		for (const auto hash : iter->m_hashMeshes)
		{
			size_t idxCollider = _ResourceSystem.GetResource<Mesh>(hash)->GetIdx_Collider();
			if (CheckBound(frustum, idxCollider, matWorld))
			{
				iter->bRenderable = true;
				count++;
				break;
			}
		}
	}
	/*
	for (const auto& iter : _RenderSystem.objs)
	{
		
	}
	*/
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

size_t CollisionSystem::CreateCollider(const std::wstring& szName, const std::vector<Vector3>* vertices, Colliders collider)
{
	switch (collider)
	{
		case Colliders::SPHERE:
		{
			return AddCollider<Sphere>(szName, vertices);
		}	break;
			
		case Colliders::AABB:
		{
			return AddCollider<Box>(szName, vertices);
		}break;
			
		case Colliders::OBB:
			break;
		case Colliders::RAY:
			break;
		default:
			return size_t();
	}
}


bool CollisionSystem::CheckBound(const Frustum& frustum, size_t idxCollider, const Matrix4x4& matWorld)
{
	switch (m_Colliders[idxCollider]->GetType())
	{
		case Colliders::SPHERE:
		{
			return (CheckBound(frustum, *static_cast<Sphere*>(m_Colliders[idxCollider]), matWorld));
		}break;

		//추후 box로 통일후 type에 따른 분별형태로 변화필요
		case Colliders::AABB:
		{
			return CheckBound(frustum, *static_cast<Box*>(m_Colliders[idxCollider]), matWorld);
		}break;

		case Colliders::OBB:
			break;
		case Colliders::RAY:
			break;
		default:
			return false;
	}
	return false;
}

bool CollisionSystem::CheckBound(const Frustum& frustum, const Sphere& sphere, const Matrix4x4& matWorld)
{
	//중심, 반지름을 월드좌표계로변환한다
	Vector3 center;
	float radius;
	sphere.GetWorldSphere(matWorld, center, radius);

	//6개의 면과 비교하여 어떠한 평면보다 한곳이라도 평면의 뒤에있다면 컬링시킨다
	for (int i = 0; i < 6; i++)
	{
		if (frustum.GetPlanes()[i].Distance(center) < -radius)
			return false;
	}
	return true;
}

bool CollisionSystem::CheckBound(const Frustum& frustum, const Box& box, const Matrix4x4& matWorld)
{
	std::array<Vector3, 8> corners;
	box.GetBoxCorners(matWorld, corners);

	//6개의 평면에 8개의 꼭지점을 대응시킨다, 평면과 점의 거리를 재어 판단한다
	for (int i = 0; i < 6; i++)
	{
		bool allCornersOutSide = true;
		for (const auto& corner : corners)
		{
			//한 코너라도 평면의 걸쳐있거나 안쪽에 있다면 (ax + by + cz + d >= 0, 법선의방향이 절두체의안쪽), 평면에 걸쳐있거나 안쪽이면 
			if (frustum.GetPlanes()[i].Distance(corner) >= 0.0f)
			{
				allCornersOutSide = false;
				break;
			}
		}
		if (allCornersOutSide) //한 평면에 대해 모든점이 바깥에 있으므로 컬링시킨다
			return false;
	}
	return true;
}

