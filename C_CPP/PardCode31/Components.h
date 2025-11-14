#pragma once
#include "CommonHeader.h"

struct C_Input
{
public:
	std::bitset<256> bVKMask;
};

struct C_Transform
{
public:
	Vector3 vScale;
	Quarternion qRotate;
	//Vector3 vRotate;
	Vector3 vPosition;
};
enum E_Behavior
{
	NONE,
	MOVE_FORWARD,
	MOVE_BACKWARD,
	MOVE_RIGHT,
	MOVE_LEFT,
	MOVE_UP,
	MOVE_DOWN,
	COUNT,
};

static const std::array<Vector3, E_Behavior::COUNT> MoveDirs =
{
	Vector3(0.0f, 0.0f, 0.0f),	  //NONE,
	Vector3(0.0f, 0.0f, 1.0f),	  //MOVE_FORWARD,
	Vector3(0.0f, 0.0f, -1.0f),   //MOVE_BACKWARD,
	Vector3(1.0f, 0.0f, 0.0f),	  //MOVE_RIGHT,
	Vector3(-1.0f, 0.0f, 0.0f),   //MOVE_LEFT,
	Vector3(0.0f, 1.0f, 0.0f),	  //MOVE_UP,
	Vector3(0.0f, -1.0f, 0.0f),   //MOVE_DOWN,
};

struct C_Behavior
{
public:
	std::array<unsigned char, E_Behavior::COUNT> BehaviorMap;
};

struct C_Camera
{
public:
	float fFov;					//field of view, 시야각
	float fScreenWidth;			//화면너비, aspectratio = 너비 / 높이
	float fScreenHeight;		//화면높이
	float fNear;				//근단면
	float fFar;					//원단면
	Matrix4x4 m_MatWorld = GetMat_Identity();
	Matrix4x4 m_MatView = GetMat_Identity();
	Matrix4x4 m_MatProj = GetMat_Identity();
	Matrix4x4 m_MatOrtho = GetMat_Identity();
};

struct C_Render
{
public:
	bool bRenderable = false;
	size_t hash_ra = 0;
};

struct C_Light
{
public:
};

struct C_Collider
{
public:

};
