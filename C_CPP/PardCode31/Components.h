#pragma once
#include "CommonHeader.h"

struct Component_Transform
{
public:
	Vector3 m_vScale;
	Quarternion m_qRotate;
	//Vector3 m_vRotate;
	Vector3 m_vPosition;
};

struct Component_Move
{
public:
	float m_fSpeed_Scale;
	float m_fSpeed_Rotate;
	float m_fSpeed_Translation;
};

struct Component_Input
{
public:
	
};

struct Component_Camera
{
public:
	float m_fFov;					//field of view, �þ߰�
	float m_fScreenWidth;			//ȭ��ʺ�, aspectratio = �ʺ� / ����
	float m_fScreenHeight;			//ȭ�����
	float m_fNear;					//�ٴܸ�
	float m_fFar;					//���ܸ�
	/*Matrix4x4 m_matWorld;
	Matrix4x4 m_MatView;
	Matrix4x4 m_MatProj;
	Matrix4x4 m_MatOrtho;*/
};



struct Component_Render
{
public:
	bool bRenderable = false;
	std::vector<Mesh_Material> m_MeshMaterials;
};

struct Component_Light
{
public:
};

struct Component_Collider
{
public:

};
