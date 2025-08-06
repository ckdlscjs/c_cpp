#pragma once
#include "std.h"

class BaseLight
{
protected:
	BaseLight();
	BaseLight(const BaseLight&) = delete;				//��������ڻ���
	BaseLight& operator=(const BaseLight&) = delete;	//������Կ����ڻ���
	BaseLight(BaseLight&&) = delete;					//�̵������ڻ���
	BaseLight& operator=(BaseLight&&) = delete;			//�̵����Կ����ڻ���
	
public:
	Vector4 m_mAmbient;
	Vector4 m_mDiffuse;
	Vector4 m_mSpecular;
public:
	virtual void Frame();
	virtual ~BaseLight();
	virtual void* GetConstant() = 0;
};

class DirectionalLight : public BaseLight
{
public:
	~DirectionalLight() override;
	void* GetConstant() override;
public:
	Vector3 m_vDirection;
	float m_fShiness;
	CB_DirectionalLight m_CBData;
};

class PointLight : public BaseLight
{
public:
	~PointLight() override;
	void* GetConstant() override;
public:
	Position m_vPosition;
	float m_fShiness;
	float m_fAtt_a0;
	float m_fAtt_a1;
	float m_fAtt_a2;
	float m_fRange;
	CB_PointLight m_CBData;
};

class SpotLight : public BaseLight
{
public:
	~SpotLight() override;
	void* GetConstant() override;
public:
	Vector3 m_vDirection;
	Position m_vPosition;
	float m_fShiness;
	float m_fAtt_a0;
	float m_fAtt_a1;
	float m_fAtt_a2;
	float m_fRange;
	float m_fSpot;
	float m_fCos_OuterCone;
	float m_fCos_InnerCone;
	CB_SpotLight m_CBData;
};