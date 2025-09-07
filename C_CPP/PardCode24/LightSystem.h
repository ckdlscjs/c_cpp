#pragma once
#include "BaseSystem.h"

class BaseLight;

class LightSystem : public BaseSystem<LightSystem>
{
	friend class BaseSystem<LightSystem>;	//CRTP패턴을 위한 friend선언
private:
	LightSystem();
	~LightSystem();
	LightSystem(const LightSystem&) = delete;
	LightSystem& operator=(const LightSystem&) = delete;
	LightSystem(LightSystem&&) = delete;
	LightSystem& operator=(LightSystem&&) = delete;

	bool CheckMask(int VK_KEY);
public:
	BaseLight* GetLight(size_t idx);
	void AddLight(BaseLight* light);
	void Init();
	void Frame();
	void Release();

private:
	//카메라들
	std::unordered_map<size_t, BaseLight*> m_Lights;
	size_t m_lLightID;

	//입력관련, 추후 컴포넌트로 분리필요
	std::unordered_map<E_InputEventType, std::vector<size_t>> m_IdxCallbacks;
	std::unordered_map<int, int> VK_MASK;
	size_t m_lVKMask;
};
#define _LightSystem LightSystem::GetInstance()
