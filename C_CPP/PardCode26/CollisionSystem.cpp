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
	UINT count_all = 1;
	UINT count = 1;
	auto pCamera = _CameraSystem.GetCamera(0);
	Frustum frustum(pCamera->GetFarZ(), pCamera->GetViewMatrix(), pCamera->GetProjMatrix());
	
	for (const auto& obj : _RenderSystem.objs)
	{
		obj->bRenderable = false;
		Matrix4x4 matWorld = GetMat_WorldMatrix(obj->m_vScale, obj->m_vRotate, obj->m_vPosition);
		count_all += _ResourceSystem.GetResource<Mesh>(obj->m_Mesh_Material[0].first)->GetCL().size();
		for (const auto& MeshMat : obj->m_Mesh_Material)
		{
			Mesh* pMesh = _ResourceSystem.GetResource<Mesh>(MeshMat.first);
			for (const auto& iter : pMesh->GetCL())
			{
				if (CheckBound(frustum, iter, matWorld))
				{
					obj->bRenderable = true;
					count++;
					break;
				}
			}
		}
	}
	std::cout << "�������� ��ü�� : " << count << ", �ø��� ��ü�� : " << count_all - count << '\n';
}

void CollisionSystem::Release()
{
	for (auto iter = m_Colliders.begin(); iter != m_Colliders.end();)
	{
		delete iter->second;
		iter = m_Colliders.erase(iter);
	}
}

size_t CollisionSystem::CreateCollider(const std::wstring& szName, const std::vector<Vector3>* vertices, E_Colliders collider)
{
	switch (collider)
	{
		case E_Colliders::SPHERE:
		{
			return AddCollider<Sphere>(szName + L"Sphere", vertices);
		}	break;
			
		case E_Colliders::AABB:
		{
			return AddCollider<Box>(szName + L"Box", vertices);
		}break;
			
		case E_Colliders::OBB:
			break;
		case E_Colliders::RAY:
			break;
		default:
			return size_t();
	}
}


bool CollisionSystem::CheckBound(const Frustum& frustum, size_t hash, const Matrix4x4& matWorld)
{
	switch (m_Colliders[hash]->GetType())
	{
		case E_Colliders::SPHERE:
		{
			return (CheckBound(frustum, *static_cast<Sphere*>(m_Colliders[hash]), matWorld));
		}break;

		//���� box�� ������ type�� ���� �к����·� ��ȭ�ʿ�
		case E_Colliders::AABB:
		{
			return CheckBound(frustum, *static_cast<Box*>(m_Colliders[hash]), matWorld);
		}break;

		case E_Colliders::OBB:
			break;
		case E_Colliders::RAY:
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
	box.GetWorldCorners(matWorld, corners);

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

