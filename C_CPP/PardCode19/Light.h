#pragma once
#include "std.h"

class BaseLight
{
protected:
	BaseLight();
	BaseLight(const BaseLight&) = delete;				//복사생성자삭제
	BaseLight& operator=(const BaseLight&) = delete;	//복사대입연산자삭제
	BaseLight(BaseLight&&) = delete;					//이동생성자삭제
	BaseLight& operator=(BaseLight&&) = delete;			//이동대입연산자삭제
	
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