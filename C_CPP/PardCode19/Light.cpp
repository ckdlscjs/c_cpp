#include "Light.h"

BaseLight::BaseLight()
{
	m_mAmbient = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	m_mDiffuse = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	m_mSpecular = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
}

void BaseLight::Frame()
{
}

BaseLight::~BaseLight()
{
}

DirectionalLight::~DirectionalLight()
{
}

void* DirectionalLight::GetConstant()
{
	m_CBData.m_Ambient = m_mAmbient;
	m_CBData.m_Diffuse = m_mDiffuse;
	m_CBData.m_Specular = m_mSpecular;
	m_CBData.v_direction = m_vDirection;
	return &m_CBData;
}

PointLight::~PointLight()
{
}

void* PointLight::GetConstant()
{
	m_CBData.m_Ambient = m_mAmbient;
	m_CBData.m_Diffuse = m_mDiffuse;
	m_CBData.m_Specular = m_mSpecular;
	m_CBData.v_Position = Vector4(m_vPosition.ToVector3(), m_fRange);
	m_CBData.f_Attenuation = Vector3(m_fAtt_a0, m_fAtt_a1, m_fAtt_a2);
	return &m_CBData;
}

SpotLight::~SpotLight()
{
}

void* SpotLight::GetConstant()
{
	m_CBData.m_Ambient = m_mAmbient;
	m_CBData.m_Diffuse = m_mDiffuse;
	m_CBData.m_Specular = m_mSpecular;
	m_CBData.v_Position = Vector4(m_vPosition.ToVector3(), m_fRange);
	m_CBData.f_Attenuation = Vector3(m_fAtt_a0, m_fAtt_a1, m_fAtt_a2);
	m_CBData.f_Spots = Vector3(m_fSpot, m_fCos_OuterCone, m_fCos_InnerCone);
	return &m_CBData;
}
