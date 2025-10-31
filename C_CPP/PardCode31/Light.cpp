#include "Light.h"
#include "FirstPersonCamera.h"
#include "CameraSystem.h"
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

void* DirectionalLight::GetConstant()
{
	/*m_CBData.mAmbient = m_mAmbient;
	m_CBData.mDiffuse = m_mDiffuse;
	m_CBData.mSpecular = m_mSpecular;
	m_CBData.vDirection = Vector4(m_vDirection, m_fShiness);*/
	return &m_CBData;
}

void* PointLight::GetConstant()
{
	/*m_CBData.mAmbient = m_mAmbient;
	m_CBData.mDiffuse = m_mDiffuse;
	m_CBData.mSpecular = m_mSpecular;
	m_CBData.vPosition = Vector4(m_vPosition,m_fShiness);
	m_CBData.fAttenuations = Vector4(m_fAtt_a0, m_fAtt_a1, m_fAtt_a2, m_fRange);*/
	return &m_CBData;
}

void* SpotLight::GetConstant()
{
	/*m_CBData.mAmbient = m_mAmbient;
	m_CBData.mDiffuse = m_mDiffuse;
	m_CBData.mSpecular = m_mSpecular;
	m_CBData.vDirection = _CameraSystem.GetCamera(0)->GetTarget().Normalize();
	m_CBData.vPosition = Vector4(_CameraSystem.GetCamera(0)->GetPosition(), m_fShiness);
	m_CBData.fAttenuations = Vector4(m_fAtt_a0, m_fAtt_a1, m_fAtt_a2, m_fRange);
	m_CBData.fSpots = Vector3(m_fSpot, m_fCos_OuterCone, m_fCos_InnerCone);*/
	return &m_CBData;
}
