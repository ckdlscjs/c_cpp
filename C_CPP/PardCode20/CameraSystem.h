#pragma once
#include "BaseSystem.h"

class BaseCamera;

class CameraSystem : public BaseSystem<CameraSystem>
{
	friend class BaseSystem<CameraSystem>;
private:
	CameraSystem();
	~CameraSystem();
	CameraSystem(CameraSystem& CameraSystem) = delete;
	CameraSystem& operator=(const CameraSystem& CameraSystem) = delete;
	CameraSystem(CameraSystem&&) = delete;
	CameraSystem& operator=(CameraSystem&&) = delete;
	bool CheckMask(UINT VK_KEY);
public:
	BaseCamera* GetCamera(size_t idx);
	void AddCamera(BaseCamera* camera);
	void Init();
	void Frame(float deltaTime);
	void Release();
	
private:
	//카메라들
	std::unordered_map<size_t, BaseCamera*> m_Cameras;
	size_t m_lCameraID;

	//입력관련, 추후 컴포넌트로 분리필요
	std::unordered_map<InputEventType, std::vector<size_t>> m_IdxCallbacks;
	std::unordered_map<UINT, UINT> VK_MASK;
	size_t m_lVKMask;
};
#define _CameraSystem CameraSystem::GetInstance()