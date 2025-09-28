#pragma once
#include "BaseSystem.h"

class BaseLight;

class LightSystem : public BaseSystem<LightSystem>
{
	friend class BaseSystem<LightSystem>;	//CRTP������ ���� friend����
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
	//ī�޶��
	std::unordered_map<size_t, BaseLight*> m_Lights;
	size_t m_lLightID;

	//�Է°���, ���� ������Ʈ�� �и��ʿ�
	std::unordered_map<E_InputEventType, std::vector<size_t>> m_IdxCallbacks;
	std::unordered_map<int, int> VK_MASK;
	size_t m_lVKMask;
};
#define _LightSystem LightSystem::GetInstance()
