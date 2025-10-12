#pragma once
#include "CommonHeader.h"

struct Component_Transform
{
public:
	Vector3 m_vScale;
	Vector3 m_vRotate;
	Vector3 m_vPosition;
};

struct Component_Render
{
public:
	bool bRenderable = false;
	std::vector<Mesh_Material> m_MeshMaterials;
};

struct Component_Input
{
public:
	std::unordered_map<E_InputEvent, std::vector<size_t>> m_IdxCallbacks;
};

struct Component_Camera
{
public:
	float m_fFov;
	float m_fScreenWidth;
	float m_fScreenHeight;
	float m_fNear;
	float m_fFar;
};

struct Component_Light
{
public:

};

struct Component_Collider
{
public:

};
