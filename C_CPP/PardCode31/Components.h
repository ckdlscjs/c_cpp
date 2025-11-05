#pragma once
#include "CommonHeader.h"

struct C_Transform
{
public:
	Vector3 vScale;
	Quarternion qRotate;
	//Vector3 vRotate;
	Vector3 vPosition;
};

struct C_Movement
{
public:
	float fSpeed_Scale;
	float fSpeed_Rotate;
	float fSpeed_Translation;
};

struct C_Input
{
public:
	std::bitset<256> vk_mask;
};

struct C_Camera
{
public:
	float fFov;					//field of view, 시야각
	float fScreenWidth;			//화면너비, aspectratio = 너비 / 높이
	float fScreenHeight;			//화면높이
	float fNear;					//근단면
	float fFar;					//원단면
	/*Matrix4x4 m_matWorld;
	Matrix4x4 m_MatView;
	Matrix4x4 m_MatProj;
	Matrix4x4 m_MatOrtho;*/
};



struct C_Render
{
public:
	bool bRenderable = false;
	std::vector<Mesh_Material> MeshMaterials;
};

struct C_Light
{
public:
};

struct C_Collider
{
public:

};
