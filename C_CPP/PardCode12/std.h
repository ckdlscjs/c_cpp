#pragma once

//Macro
#define _ASEERTION_NULCHK(result, message) assert(result && message)
#define _ASEERTION_CREATE(result, message) assert(result == S_OK && message)
#define _DEGTORAD(DEG) DEG / 180.0f * DirectX::XM_PI
#define _RADTODEG(RAD) RAD / DirectX::XM_PI * 180.0f

//App Header
#include <cstdlib>
#include <crtdbg.h>
#include <Windows.h>
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <functional>
//#include "bits/stdc++.h"

//d3d Header
#include <d3d11.h>
#include <DirectXMath.h>					//XMMATRIX
#include <d3dcompiler.h>					//d3dcomplie, 쉐이더사용을위한추가
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")		//d3dcomplie, 쉐이더사용을위한추가

using namespace DirectX;

//Resources data struct
enum class InputEventType
{
	KEY_DOWN,
	KEY_UP,
	KEY_PRESSED,
	MOUSE_BUTTON_DOWN,
	MOUSE_BUTTON_UP,
	MOUSE_MOVE,
	MOUSE_WHEEL,
};

struct InputEvent
{
	InputEventType type;
	int keyCode = 0;
	int mouseX = 0;
	int mouseY = 0;
	int mouseDeltaX = 0;
	int mouseDeltaY = 0;
	int wheelDelta = 0;
};
using EventCallBack = std::function<void(const InputEvent&)>;

struct Vertex_PC
{
	XMFLOAT3 pos0;
	XMFLOAT4 color0;
};

struct Vertex_PPCC
{
	XMFLOAT3 pos0;
	XMFLOAT3 pos1;
	XMFLOAT4 color0;
	XMFLOAT4 color1;
};

//16바이트 단위로 gpu메모리에서 패딩되므로 단위를 맞춘다
__declspec(align(16))
struct Constant_time
{
	float fTime;
};

__declspec(align(16))
struct Constant_wvp
{
	XMMATRIX matWorld;
	XMMATRIX matView;
	XMMATRIX matProj;
};

//공용함수들

//선형보간(vector3)
inline XMFLOAT3 lerp(const XMFLOAT3& st, const XMFLOAT3& en, float t)
{
	return XMFLOAT3(st.x * (1.0f - t) + en.x * t, st.y * (1.0f - t) + en.y*t, st.z * (1.0f - t) + en.z * t);
}

inline XMMATRIX GetMat_Identity()
{
	XMMATRIX mat;
	mat.r[0] = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	mat.r[1] = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	mat.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	mat.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	return mat;
}

inline XMMATRIX GetMat_Scale(const XMFLOAT3 scale)
{
	XMMATRIX mat;
	mat.r[0] = XMVectorSet(scale.x, 0.0f, 0.0f, 0.0f);
	mat.r[1] = XMVectorSet(0.0f, scale.y, 0.0f, 0.0f);
	mat.r[2] = XMVectorSet(0.0f, 0.0f, scale.z, 0.0f);
	mat.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	return mat;
}

inline XMMATRIX GetMat_Translation(const XMFLOAT3 translation)
{
	XMMATRIX mat;
	mat.r[0] = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	mat.r[1] = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	mat.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	mat.r[3] = XMVectorSet(translation.x, translation.y, translation.z, 1.0f);
	return mat;
}
inline XMMATRIX GetMat_RotPitch(const float degree)
{
	XMMATRIX mat;
	float radAngle = degree / 180.0f * DirectX::XM_PI;
	float cost = cosf(radAngle);
	float sint = sinf(radAngle);
	mat.r[0] = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	mat.r[1] = XMVectorSet(0.0f, cost, sint, 0.0f);
	mat.r[2] = XMVectorSet(0.0f, -sint, cost, 0.0f);
	mat.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	return mat;
}

inline XMMATRIX GetMat_RotYaw(const float degree)
{
	XMMATRIX mat;
	float radAngle = _DEGTORAD(degree);
	float cost = cosf(radAngle);
	float sint = sinf(radAngle);
	mat.r[0] = XMVectorSet(cost, 0.0f, -sint, 0.0f);
	mat.r[1] = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	mat.r[2] = XMVectorSet(sint, 0.0f, cost, 0.0f);
	mat.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	return mat;
}

inline XMMATRIX GetMat_RotRoll(const float degree)
{
	XMMATRIX mat;
	float radAngle = _DEGTORAD(degree);
	float cost = cosf(radAngle);
	float sint = sinf(radAngle);
	mat.r[0] = XMVectorSet(cost, sint, 0.0f, 0.0f);
	mat.r[1] = XMVectorSet(-sint, cost, 0.0f, 0.0f);
	mat.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	mat.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	return mat;
}

inline XMMATRIX GetMat_RotRollPitchYaw(const XMFLOAT3 eulerDegrees)
{
	XMMATRIX matRoll = GetMat_RotRoll(eulerDegrees.z);
	XMMATRIX matPitch = GetMat_RotPitch(eulerDegrees.x);
	XMMATRIX matYaw = GetMat_RotYaw(eulerDegrees.y);
	return matRoll * matPitch * matYaw;
}

inline XMMATRIX GetMat_WorldMatrix(const XMFLOAT3 scale, const XMFLOAT3 eulerDegrees, const XMFLOAT3 translation)
{
	return GetMat_Scale(scale) * GetMat_RotRollPitchYaw(eulerDegrees) * GetMat_Translation(translation);
}

inline XMMATRIX GetMat_ViewMatrix(const XMFLOAT3 posCamera, const XMFLOAT3 posTarget, const XMFLOAT3 upVector = XMFLOAT3(0.0f, 1.0f, 0.0f))
{
	XMMATRIX mat;
	XMVECTOR campos = XMLoadFloat3(&posCamera);
	//forward, z
	XMVECTOR w = XMLoadFloat3(&posTarget) - campos;
	w = XMVector3Normalize(w);
	//worldup
	XMVECTOR j = XMLoadFloat3(&upVector);
	//right, x
	XMVECTOR u = XMVector3Normalize(XMVector3Cross(j, w));
	//up, y
	XMVECTOR v = XMVector3Cross(w, u);
	mat.r[0] = XMVectorSet(XMVectorGetX(u), XMVectorGetX(v), XMVectorGetX(w), 0.0f);
	mat.r[1] = XMVectorSet(XMVectorGetY(u), XMVectorGetY(v), XMVectorGetY(w), 0.0f);
	mat.r[2] = XMVectorSet(XMVectorGetZ(u), XMVectorGetZ(v), XMVectorGetZ(w), 0.0f);
	//-(Q dot u), -(Q dot v), -(Q dot w), 1.0f
	//dot내적의 결과가 x요소에있음
	mat.r[3] = XMVectorSet(-XMVectorGetX(XMVector3Dot(u, campos)),-XMVectorGetX(XMVector3Dot(v, campos)),-XMVectorGetX(XMVector3Dot(w, campos)),1.0f);
	return mat;
}

inline XMMATRIX GetMat_Perspective(float width, float height, float fov, float dist_near, float dist_far)
{
	XMMATRIX mat;
	float aspectRatio = width / height;
	float d = tanf(_DEGTORAD(fov) * 0.5f);
	mat.r[0] = XMVectorSet(1.0f/(aspectRatio * d), 0.0f, 0.0f, 0.0f);
	mat.r[1] = XMVectorSet(0.0f, 1.0f / d, 0.0f, 0.0f);
	mat.r[2] = XMVectorSet(0.0f, 0.0f, dist_far / (dist_far-dist_near), 1.0f);
	mat.r[3] = XMVectorSet(0.0f, 0.0f, -(dist_near*dist_far) / (dist_far - dist_near), 0.0f);
	return mat;
}

inline XMMATRIX GetMat_Ortho(float width, float height, float dist_near, float dist_far)
{
	XMMATRIX mat;
	mat.r[0] = XMVectorSet(2.0f/width, 0.0f, 0.0f, 0.0f);
	mat.r[1] = XMVectorSet(0.0f, 2.0f/height, 0.0f, 0.0f);
	mat.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f / (dist_far - dist_near), 0.0f);
	mat.r[3] = XMVectorSet(0.0f, 0.0f, -dist_near  / (dist_far - dist_near), 1.0f);
	return mat;
}