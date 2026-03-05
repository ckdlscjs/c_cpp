#include "CollisionSystem.h"
#include "CameraSystem.h"
#include "ResourceSystem.h"
#include "ECSSystem.h"
#include "Inputsystem.h"	//Picking

#include "Assets.h"
#include "Mesh.h"

#include "Frustum.h"
#include "Plane.h"
#include "Sphere.h"
#include "Box.h"



//밑부분은 책임분리 고려필요


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

	//현재 프레임에서 절두체 컬링으로 렌더할 객체들
	std::vector<std::pair<size_t, size_t>> RenderAbles;

	//시야절두체
	Frustum frustum(c_cam_main.fFar, cam_matView, cam_matProj);

	//피킹
	/*
	* 화면좌표계->NDC->PROJ->VIEW->WORLD->LOCAL
	* x(ndc) -> (x(screen) * 2.0f / width) - 1.0f
	* y(ndc) -> -(y(screen) * 2.0f / height) + 1.0f
	* x(v) -> x(p)로의 변환에서 r(종횡비)를 역수로 나눴으므로 이를 같이 계산한다, 이는 투영행렬의 요소에담겨있다(matP00)
	* x' -> x / d -> x / cot(a/2) 
	* y' -> y / d -> y / cot(a/2)
	*/
	Vector2 pickingPos = _InputSystem.GetPickingPos();
	float vx = (+2.0f * pickingPos.GetX() / g_iWidth - 1.0f) / cam_matProj[0].GetX();
	float vy = (-2.0f * pickingPos.GetY() / g_iHeight + 1.0f) / cam_matProj[1].GetY();
	
	Vector4 rayOrigin(0.0f, 0.0f, 0.0f, 1.0f);
	Vector4 rayDir(vx, vy, 1.0f, 0.0f);
	Matrix4x4 matInvView = GetMat_Inverse(cam_matView);

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
			for (size_t col = st_col; col < archetype->GetCount_Chunk(row); col++)
			{
				renders[col].bRenderable = false;
				const Vector3& scale = transforms[col].vScale;
				const Quarternion& rotate = transforms[col].qRotate;
				const Vector3& position = transforms[col].vPosition;
				Matrix4x4 matWorld = GetMat_World(scale, rotate, position);
				
				//MousePicking Variable
				Matrix4x4 matInvWorld = GetMat_Inverse(matWorld);
				Matrix4x4 matToLocal = matInvView * matInvWorld;
				Vector4	localRayOrigin = rayOrigin * matToLocal;
				Vector4 localRayDir = (rayDir * matToLocal).Normalize();

				const auto& MeshMats = _ResourceSystem.GetResource<RenderAsset>(renders[col].hash_ra)->m_hMeshMats;
				for (UINT j = 0; j < MeshMats.size(); j++)
				{
					auto& iter = MeshMats[j];
					BaseMesh* pMesh = _ResourceSystem.GetResource<BaseMesh>(iter.hash_mesh);

					//Frustum Culling
					{
						for (const auto& iter : pMesh->GetCLs())
						{
							if (IsCollision(frustum, iter, matWorld))
							{
								renders[col].bRenderable = true;
								//RenderAbles.push_back({ row, col });
								break;
							}
						}
						if (!renders[col].bRenderable) break;
					}

					//Picking
					{
						if (!_InputSystem.IsPressed_LBTN()) continue;
						for (const auto& iter : pMesh->GetCLs())
						{
							if (IsCollision(localRayOrigin, localRayDir, iter))
							{
								std::wcout << "----------------" << pMesh->GetPath() << "----------------" << '\n';
							}
						}
					}
				}
			}
		}
		st_col = 0;
	}
}


size_t CollisionSystem::CreateCollider(const std::wstring& szName, const std::vector<Vector3>* vertices, E_Collider collider)
{
	switch (collider)
	{
		case E_Collider::SPHERE:
		{
			return AddCollider<Sphere>(szName + L"Sphere", vertices);
		}	break;

		case E_Collider::OBB:
		case E_Collider::AABB:
		{
			return AddCollider<Box>(szName + L"Box", vertices);
		}break;

		case E_Collider::RAY:
			break;
		default:
			return size_t();
	}
}


bool CollisionSystem::IsCollision(const Frustum& frustum, size_t hash, const Matrix4x4& matWorld)
{
	switch (m_Colliders[hash]->GetType())
	{
		case E_Collider::SPHERE:
		{
			return (IsCollision(frustum, *static_cast<Sphere*>(m_Colliders[hash]), matWorld));
		}break;

		//추후 box로 통일후 type에 따른 분별형태로 변화필요
		case E_Collider::OBB:
		case E_Collider::AABB:
		{
			return IsCollision(frustum, *static_cast<Box*>(m_Colliders[hash]), matWorld);
		}break;

		case E_Collider::RAY:
			break;
		default:
			return false;
	}
	return false;
}

bool CollisionSystem::IsCollision(const Frustum& frustum, const Sphere& sphere, const Matrix4x4& matWorld)
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

bool CollisionSystem::IsCollision(const Frustum& frustum, const Box& box, const Matrix4x4& matWorld)
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

bool CollisionSystem::IsCollision(const Vector4& rayOrigin, const Vector4& rayDir, size_t hash)
{
	switch (m_Colliders[hash]->GetType())
	{
		case E_Collider::SPHERE:
		{
			return IsCollision(rayOrigin, rayDir, *static_cast<Sphere*>(m_Colliders[hash]));
		}break;

		//로컬공간상으로 돌려서 판별하므로 obb,aabb를 동일하게처리한다
		case E_Collider::OBB:
		case E_Collider::AABB:
		{
			return IsCollision(rayOrigin, rayDir, *static_cast<Box*>(m_Colliders[hash]));
		}break;

		case E_Collider::RAY:
			break;
		default:
			return false;
	}
	return false;
}

/*
* Ray to BOX(OBB로컬에서판별하므로 둘다가능), using slab method
* ray = o + td, 박스에 교차하는 지점 두곳까지의 거리비스칼라값이 근단점s, 원단점t라 할때 각각의 지점을 vMin, vMax로 가정한다
* vMin = o + sd, vMax = o + td, 고로 s/t로 치환해 스칼라값을 구한다, 이때 구한 평면별 vMin, vMax의 값들중 min들의최대 <= max들의최소 가 존재한다면
* 레이는 교차함을 의미하므로 이중 가장 작은 s값까지의 거리를 dist로본다
*/
bool CollisionSystem::IsCollision(const Vector4& rayOrigin, const Vector4& rayDir, const Box& box)
{
	if (rayDir.Length() <= _EPSILON) return false;
	Vector3 vExtent = (box.GetMax() - box.GetMin()) * 0.5f;								//extent
	Vector3 vOrigin = rayOrigin.ToVector3() - ((box.GetMax() + box.GetMin()) * 0.5f);	//레이의 중심에서 박스의 중심(상대좌표)를 감산
	float dirs[3] = { rayDir.GetX(), rayDir.GetY(), rayDir.GetZ() };
	float origins[3] = { vOrigin.GetX(), vOrigin.GetY(), vOrigin.GetZ() };
	float extents[3] = { vExtent.GetX(), vExtent.GetY(), vExtent.GetZ() };

	float tMin = -FLT_MAX;
	float tMax = +FLT_MAX;
	//x, y, z axis
	for (int i = 0; i < 3; i++)
	{
		if (std::abs(dirs[i]) <= _EPSILON && (origins[i] < -extents[i] || extents[i] < origins[i])) return false;	//평행이면서 범위밖이면 만날수없다
		float invD = 1.0f / dirs[i];
		float s = (-extents[i] - origins[i]) * invD;
		float t = (+extents[i] - origins[i]) * invD;
		if (s > t) std::swap(s, t);
		tMin = std::max(tMin, s);
		tMax = std::min(tMax, t);
	}

	if (tMin > tMax) return false;
	if (tMax < 0.0f) return false;
	
	return true;
}


/*
* ray, sphere의 교차판정
* 중심점 center로부터 ray의 시작점을 뺀 벡터 l을 기준으로하여 이와 dir를 내적해 중심으로부터의 거리 스칼라값 s(투영거리)를 구한다
* 이를 기반으로 교점이 없는 조건을 우선 판별한다
* -거리 s가 0보다 작은경우(방향이반대)
* -거리l2가 반지름보다 큰경우
* -수선의발m이 반지름r보다큰경우
* 이들이 부합한다면 방향도 반대이면서 레이가 원밖이므로 교차하지않는다
* 이후 t1,t2를 구한다
*/
bool CollisionSystem::IsCollision(const Vector4& rayOrigin, const Vector4& rayDir, const Sphere& sphere)
{
	Vector3 normalizeDir = rayDir.ToVector3().Normalize();

	Vector3 Center = sphere.GetCenter();
	float Radius = sphere.GetRadius();

	//l벡터 -> 중심과 시작점사이의 벡터
	Vector3 l = Center - rayOrigin.ToVector3();

	//방향벡터에 투영
	float s = l.DotProduct(normalizeDir);

	float l2 = l.DotProduct(l);

	float r2 = Radius * Radius;

	//l^2 = m^2 + s^2 이므로 m -> 중심에서 s까지의 수직벡터
	float m2 = l2 - s * s;

	bool NoIntersection;

	//s벡터가 영벡터보다 작으면 dir내적결과이므로 구의 중심이 광선과 반대방향
	//l^2은 시작점부터 구중심까지 거리의제곱, r^2은 반지름, 즉 l^2>r^2이면 광선의 시작점이 구의 바깥에존재
	//시작점이 구의바깥에 존재하면서 방향또한 구의반대방향, 즉 만날수없음b
	bool oppositeCenter = s < 0;
	bool rayOutSphere = l2 > r2;
	NoIntersection = oppositeCenter && rayOutSphere;

	//수선의발m이 r보다 크면 교점은 만날수없음
	NoIntersection = NoIntersection || m2 > r2;
	if (NoIntersection)
		return false;

	//t1, t2를 구성한다
	float q = std::sqrt(r2 - m2);
	float t1 = s - q;
	float t2 = s + q;
	float t = l2 > r2 ? s - q : s + q;
	return true;
}

void CollisionSystem::SetColliderDebugData(const size_t hash, CB_Debug_Box& cb)
{
	_ASEERTION_NULCHK(m_Colliders.find(hash) != m_Colliders.end(), "Collider Not exist");
	cb.vMin = static_cast<Box*>(m_Colliders[hash])->GetMin();
	cb.vMax = static_cast<Box*>(m_Colliders[hash])->GetMax();
}

void CollisionSystem::SetColliderDebugData(const size_t hash, CB_Debug_Sphere& cb)
{
	_ASEERTION_NULCHK(m_Colliders.find(hash) != m_Colliders.end(), "Collider Not exist");
	Sphere* pSphere = static_cast<Sphere*>(m_Colliders[hash]);
	cb.vInfo = Vector4(pSphere->GetCenter(), pSphere->GetRadius());
	cb.fTessFactor = 4.0f;
}
