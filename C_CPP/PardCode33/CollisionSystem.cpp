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

/*
	XMMATRIX P = mCam.Proj();

	// Compute picking ray in view space.
	// 2장과 같은 이유로 P(0,0) -> P.r[0].m128_f32[0]
	// 2장과 같은 이유로 P(1,1) -> P.r[1].m128_f32[1]
	float vx = (+2.0f * sx / mClientWidth - 1.0f) / P.r[0].m128_f32[0];
	float vy = (-2.0f * sy / mClientHeight + 1.0f) / P.r[1].m128_f32[1];

	// Ray definition in view space.
	XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

	// Tranform ray to local space of Mesh.
	XMMATRIX V = mCam.View();
	XMMATRIX invView = XMMatrixInverse(&XMMatrixDeterminant(V), V);

	XMMATRIX W = XMLoadFloat4x4(&mMeshWorld);
	XMMATRIX invWorld = XMMatrixInverse(&XMMatrixDeterminant(W), W);

	XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

	rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
	rayDir = XMVector3TransformNormal(rayDir, toLocal);

	// Make the ray direction unit length for the intersection tests.
	rayDir = XMVector3Normalize(rayDir);

	// If we hit the bounding box of the Mesh, then we might have picked a Mesh triangle,
	// so do the ray/triangle tests.
	//
	// If we did not hit the bounding box, then it is impossible that we hit
	// the Mesh, so do not waste effort doing ray/triangle tests.

	// Assume we have not picked anything yet, so init to -1.
	mPickedTriangle = -1;
	float tmin = 0.0f;
	if (XNA::IntersectRayAxisAlignedBox(rayOrigin, rayDir, &mMeshBox, &tmin))
	{
		// Find the nearest ray/triangle intersection.
		tmin = MathHelper::Infinity;
		for (UINT i = 0; i < mMeshIndices.size() / 3; ++i)
		{
			// Indices for this triangle.
			UINT i0 = mMeshIndices[i * 3 + 0];
			UINT i1 = mMeshIndices[i * 3 + 1];
			UINT i2 = mMeshIndices[i * 3 + 2];

			// Vertices for this triangle.
			XMVECTOR v0 = XMLoadFloat3(&mMeshVertices[i0].Pos);
			XMVECTOR v1 = XMLoadFloat3(&mMeshVertices[i1].Pos);
			XMVECTOR v2 = XMLoadFloat3(&mMeshVertices[i2].Pos);

			// We have to iterate over all the triangles in order to find the nearest intersection.
			float t = 0.0f;
			if (XNA::IntersectRayTriangle(rayOrigin, rayDir, v0, v1, v2, &t))
			{
				if (t < tmin)
				{
					// This is the new nearest picked triangle.
					tmin = t;
					mPickedTriangle = i;
				}
			}
		}
	}
	*/
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

	ArchetypeKey key = _ECSSystem.GetArchetypeKey<C_Transform, C_Render, T_Collider>();
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
					for (const auto& iter : pMesh->GetCLs())
					{
						if (IsCollision(frustum, iter, matWorld))
						{
							renders[col].bRenderable = true;
							RenderAbles.push_back({ row, col });



							break;
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

void CollisionSystem::SetColliderDebugData(const size_t hash, CB_Debug_Box& cb)
{
	_ASEERTION_NULCHK(m_Colliders.find(hash) != m_Colliders.end(), "Collider Not exist");
	cb.vMin = static_cast<Box*>(m_Colliders[hash])->GetMin();
	cb.vMax = static_cast<Box*>(m_Colliders[hash])->GetMax();
}

