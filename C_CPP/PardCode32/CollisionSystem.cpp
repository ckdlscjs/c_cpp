#include "CollisionSystem.h"
#include "Frustum.h"
#include "Plane.h"
#include "Sphere.h"
#include "Box.h"
#include "ECSSystem.h"
#include "Assets.h"
//밑부분은 책임분리 고려필요
#include "CameraSystem.h"
#include "ResourceSystem.h"
#include "Inputsystem.h"	//Picking
//#include "ResourceSystem.h"

CollisionSystem::CollisionSystem()
{

}
CollisionSystem::~CollisionSystem()
{
	for (auto iter = m_Colliders.begin(); iter != m_Colliders.end();)
	{
		delete iter->second;
		iter = m_Colliders.erase(iter);
	}
}
void CollisionSystem::Init()
{

}

void CollisionSystem::Frame(float deltatime)
{
	size_t lookup_maincam = _CameraSystem.lookup_maincam;
	const auto& c_cam_main = _ECSSystem.GetComponent<C_Camera>(lookup_maincam);
	const auto& c_cam_proj = _ECSSystem.GetComponent<C_Projection>(lookup_maincam);
	const Matrix4x4& cam_matView = c_cam_main.matView;
	const Matrix4x4& cam_matProj = c_cam_proj.matProj;

	Frustum frustum(c_cam_main.fFar, cam_matView, cam_matProj);

	ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, C_Collider>();
	std::vector<Archetype*> queries = _ECSSystem.QueryArchetypes(key);

	for (auto& archetype : queries)
	{
		size_t st_row = 0;
		size_t st_col = 0;
		for (size_t row = st_row; row < archetype->GetCount_Chunks(); row++)
		{
			auto& transforms = archetype->GetComponents<C_Transform>(row);
			auto& renders = archetype->GetComponents<C_Render>(row);
			auto& colliders = archetype->GetComponents<C_Collider>(row);
			for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
			{
				renders[col].bRenderable = false;
				const Vector3& scale = transforms[col].vScale;
				const Quarternion& rotate = transforms[col].qRotate;
				const Vector3& position = transforms[col].vPosition;
				Matrix4x4 matWorld = GetMat_World(scale, rotate, position);
				const ColliderAsset* pColliderAsset = _ResourceSystem.GetResource<ColliderAsset>(colliders[col].hash_ca);
				for (const auto& iter : pColliderAsset->m_hColliders)
				{
					if (CheckBound(frustum, iter, matWorld))
					{
						renders[col].bRenderable = true;
						break;
					}
				}
			}
		}
		st_col = 0;
	}

#ifdef _ECS
	UINT count_all = 1;
	UINT count = 1;
	auto pCamera = _CameraSystem.GetCamera(0);
	Frustum frustum(pCamera->GetFarZ(), pCamera->GetViewMatrix(), pCamera->GetProjMatrix());

	for (const auto& obj : _RenderSystem.objs)
	{
		obj->bRenderable = false;
		Matrix4x4 matWorld = GetMat_World(obj->m_vScale, obj->m_vRotate, obj->m_vPosition);
		count_all += _ResourceSystem.GetResource<Mesh<Vertex_PTN>>(obj->m_Mesh_Material[0].hash_mesh)->GetCL().size();
		for (const auto& MeshMat : obj->m_Mesh_Material)
		{
			Mesh<Vertex_PTN>* pMesh = _ResourceSystem.GetResource<Mesh<Vertex_PTN>>(MeshMat.hash_mesh);
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
	std::cout << "렌더링될 객체수 : " << count << ", 컬링된 객체수 : " << count_all - count << '\n';
#endif // _ECS

	
}



size_t CollisionSystem::CreateCollider(const std::wstring& szName, const std::vector<Vector3>* vertices, E_Collider collider)
{
	switch (collider)
	{
		case E_Collider::SPHERE:
		{
			return AddCollider<Sphere>(szName + L"Sphere", vertices);
		}	break;
			
		case E_Collider::AABB:
		{
			return AddCollider<Box>(szName + L"Box", vertices);
		}break;
			
		case E_Collider::OBB:
			break;
		case E_Collider::RAY:
			break;
		default:
			return size_t();
	}
}


bool CollisionSystem::CheckBound(const Frustum& frustum, size_t hash, const Matrix4x4& matWorld)
{
	switch (m_Colliders[hash]->GetType())
	{
		case E_Collider::SPHERE:
		{
			return (CheckBound(frustum, *static_cast<Sphere*>(m_Colliders[hash]), matWorld));
		}break;

		//추후 box로 통일후 type에 따른 분별형태로 변화필요
		case E_Collider::AABB:
		{
			return CheckBound(frustum, *static_cast<Box*>(m_Colliders[hash]), matWorld);
		}break;

		case E_Collider::OBB:
			break;
		case E_Collider::RAY:
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
	box.GetWorldCorners(matWorld, corners);

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

