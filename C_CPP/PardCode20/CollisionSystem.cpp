#include "CollisionSystem.h"
#include "Frustum.h"
#include "Plane.h"
#include "Sphere.h"
#include "Box.h"
//�غκ��� å�Ӻи� ����ʿ�
#include "CameraSystem.h"
#include "FirstPersonCamera.h"
#include "RenderSystem.h"	//�ӽ�, obj���޾ƿ������� ���� objectSystem���� �޾ƿ;���
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
	std::cout << "�������� ��ü�� : " << count << ", �ø��� ��ü�� : " << _RenderSystem.objs .size() - count << '\n';
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

		//���� box�� ������ type�� ���� �к����·� ��ȭ�ʿ�
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
	//�߽�, �������� ������ǥ��κ�ȯ�Ѵ�
	Vector3 center;
	float radius;
	sphere.GetWorldSphere(matWorld, center, radius);

	//6���� ��� ���Ͽ� ��� ��麸�� �Ѱ��̶� ����� �ڿ��ִٸ� �ø���Ų��
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

	//6���� ��鿡 8���� �������� ������Ų��, ���� ���� �Ÿ��� ��� �Ǵ��Ѵ�
	for (int i = 0; i < 6; i++)
	{
		bool allCornersOutSide = true;
		for (const auto& corner : corners)
		{
			//�� �ڳʶ� ����� �����ְų� ���ʿ� �ִٸ� (ax + by + cz + d >= 0, �����ǹ����� ����ü�Ǿ���), ��鿡 �����ְų� �����̸� 
			if (frustum.GetPlanes()[i].Distance(corner) >= 0.0f)
			{
				allCornersOutSide = false;
				break;
			}
		}
		if (allCornersOutSide) //�� ��鿡 ���� ������� �ٱ��� �����Ƿ� �ø���Ų��
			return false;
	}
	return true;
}

