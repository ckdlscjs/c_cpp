#pragma once
#include "CommonHeader.h"

class Component
{
	
};

class Component_Transform : public Component
{
public:
	Vector3 m_vScale;
	Vector3 m_vRotate;
	Vector3 m_vPosition;
	float m_fSpeedScale;
	float m_fSpeedRotate;
	float m_fSpeedMove;
};

class Component_Render : public Component
{
public:
	bool bRenderable = false;
	size_t m_hashCollider;
	std::vector<size_t> m_IdxCBs;
	std::vector<size_t> m_hashMeshes;
	std::vector<size_t> m_hashTextures;
};
