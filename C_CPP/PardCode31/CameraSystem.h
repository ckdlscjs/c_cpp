#pragma once
#include "BaseSystem.h"

class BaseCamera;
class CameraSystem : public BaseSystem<CameraSystem>
{
//	friend class BaseSystem<CameraSystem>;
//private:
//	CameraSystem();
//	CameraSystem(CameraSystem& CameraSystem) = delete;
//	CameraSystem& operator=(const CameraSystem& CameraSystem) = delete;
//	CameraSystem(CameraSystem&&) = delete;
//	CameraSystem& operator=(CameraSystem&&) = delete;
//public:
//	~CameraSystem();
//	void Init();
//	void Frame(float deltaTime);
//
//	//속성
//	void SetPosition(int chunkIdx);
//	void SetFOV(int chunkIdx, float FOV);
//	void SetScreenWidth(int chunkIdx, float width);
//	void SetScreenHeight(int chunkIdx, float height);
//	void SetClipPlanes(int chunkIdx, float nearZ, float farZ);
//	const Vector3& GetPosition(int chunkIdx);
//
//
//	//행렬
//	const Matrix4x4& GetViewMatrix(int chunkIdx);
//	const Matrix4x4& GetProjMatrix(int chunkIdx);
//	const Matrix4x4& GetOrthoMatrix(int chunkIdx);
//
//	
//private:
//	ArchetypeKey m_Key;
//	
	//old, oop기반
	
	friend class BaseSystem<CameraSystem>;
private:
	CameraSystem();
	CameraSystem(CameraSystem& CameraSystem) = delete;
	CameraSystem& operator=(const CameraSystem& CameraSystem) = delete;
	CameraSystem(CameraSystem&&) = delete;
	CameraSystem& operator=(CameraSystem&&) = delete;
	bool CheckMask(UINT VK_KEY);
public:
	~CameraSystem();
	void Init();
	void Frame(float deltaTime);
	void AddCamera(BaseCamera* camera);
	BaseCamera* GetCamera(size_t idx);
	
private:
	//카메라들
	std::unordered_map<size_t, BaseCamera*> m_Cameras;
	size_t m_lCameraID;

	//입력관련, 추후 컴포넌트로 분리필요
	std::unordered_map<E_InputEvent, std::vector<size_t>> m_IdxCallbacks;
	
};
#define _CameraSystem CameraSystem::GetInstance()