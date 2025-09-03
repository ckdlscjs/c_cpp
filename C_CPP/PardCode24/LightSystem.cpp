#include "LightSystem.h"
#include "Light.h"
#include "InputSystem.h"

LightSystem::LightSystem()
{
}

LightSystem::~LightSystem()
{
}

bool LightSystem::CheckMask(int VK_KEY)
{
	return false;
}

BaseLight* LightSystem::GetLight(size_t idx)
{
	return m_Lights[idx];
}

void LightSystem::AddLight(BaseLight* light)
{
	_ASEERTION_NULCHK(light, "Light is NULL");
	m_Lights[m_lLightID++] = light;
}

void LightSystem::Init()
{
	m_lLightID = 0;
	m_lVKMask = 0;
	int maskNum = 0;

}

void LightSystem::Frame()
{

}

void LightSystem::Release()
{
	for (auto iter = m_Lights.begin(); iter != m_Lights.end();)
	{
		delete iter->second;
		iter = m_Lights.erase(iter);
	}

	for (const auto& category : m_IdxCallbacks)
		for (const auto& iter : category.second)
			_InputSystem.RemoveListner(category.first, iter);
}
