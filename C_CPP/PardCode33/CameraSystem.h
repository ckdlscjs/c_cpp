#pragma once
#include "BaseSystem.h"

class CameraSystem : public BaseSystem<CameraSystem>
{
	friend class BaseSystem<CameraSystem>;
private:
	CameraSystem();
	CameraSystem(CameraSystem& CameraSystem) = delete;
	CameraSystem& operator=(const CameraSystem& CameraSystem) = delete;
	CameraSystem(CameraSystem&&) = delete;
	CameraSystem& operator=(CameraSystem&&) = delete;
public:
	~CameraSystem();
	void Init();
	void Frame(float deltaTime);

	/*
	//속성
	void SetPosition(int chunkIdx, const Vector3& position);
	void SetFOV(int chunkIdx, float FOV);
	void SetScreenWidthHeight(int chunkIdx, float width, float height);
	void SetClipPlanes(int chunkIdx, float nearZ, float farZ);
	const Vector3& GetPosition(int chunkIdx);

	//행렬
	const Matrix4x4& GetViewMatrix(int chunkIdx);
	const Matrix4x4& GetProjMatrix(int chunkIdx);
	const Matrix4x4& GetOrthoMatrix_CT(int chunkIdx);
	const Matrix4x4& GetOrthoMatrix_LT(int chunkIdx);
	*/
	//임시
	size_t lookup_maincam;
	size_t lookup_cubemapcam[6];
private:
	ArchetypeKey m_Key;
	
};
#define _CameraSystem CameraSystem::GetInstance()