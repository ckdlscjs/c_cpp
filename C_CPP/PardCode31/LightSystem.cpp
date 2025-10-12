#include "LightSystem.h"
#include "Light.h"
#include "InputSystem.h"

LightSystem::LightSystem()
{
}
LightSystem::~LightSystem()
{
	for (auto iter = m_Lights.begin(); iter != m_Lights.end();)
	{
		delete iter->second;
		iter = m_Lights.erase(iter);
	}

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
}

void LightSystem::Frame()
{

}
