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
	//ī�޶��
	std::unordered_map<size_t, BaseCamera*> m_Cameras;
	size_t m_lCameraID;

	//�Է°���, ���� ������Ʈ�� �и��ʿ�
	std::unordered_map<E_InputEvent, std::vector<size_t>> m_IdxCallbacks;
};
#define _CameraSystem CameraSystem::GetInstance()