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
	float m_fFov;					//field of view, 시야각
	float m_fScreenWidth;			//화면너비, aspectratio = 너비 / 높이
	float m_fScreenHeight;			//화면높이
	float m_fNear;					//근단면
	float m_fFar;					//원단면
	//Vector4 m_vQuarternion		//사원수, 추후추가필요 -> rotate등의 오일러각으로 변화해사용
	Matrix4x4 m_matWorld;
	Matrix4x4 m_MatView;
	Matrix4x4 m_MatProj;
	Matrix4x4 m_MatOrtho;
};

struct Component_Light
{
public:
};

struct Component_Collider
{
public:

};
