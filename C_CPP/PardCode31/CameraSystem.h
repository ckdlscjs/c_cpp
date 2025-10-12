#pragma once
#include "BaseSystem.h"

class BaseCamera;

class CameraSystem : public BaseSystem<CameraSystem>
{
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