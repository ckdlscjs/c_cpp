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
	float m_fFov;					//field of view, �þ߰�
	float m_fScreenWidth;			//ȭ��ʺ�, aspectratio = �ʺ� / ����
	float m_fScreenHeight;			//ȭ�����
	float m_fNear;					//�ٴܸ�
	float m_fFar;					//���ܸ�
	//Vector4 m_vQuarternion		//�����, �����߰��ʿ� -> rotate���� ���Ϸ������� ��ȭ�ػ��
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
