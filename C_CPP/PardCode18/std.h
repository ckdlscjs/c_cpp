#pragma once

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
#include <DirectXTex.h>						//Include/DXTEX, 텍스쳐사용을위한추가
#pragma comment(lib, "DirectXTex.lib")

//Macro
#define WIN32_LEAN_AND_MEAN             // 거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#define _ASEERTION_NULCHK(result, message) assert(result && message)
#define _ASEERTION_CREATE(result, message) assert(result == S_OK && message)
#define _DEGTORAD(DEG) DEG / 180.0f * XM_PI
#define _RADTODEG(RAD) RAD / XM_PI * 180.0f
#define _EPSILON 1e-6f
#define _CLAMP(val, low, high) Clamp(val, low, high)

using namespace DirectX;

//Resources data struct
enum class InputEventType
{
	KEY_DOWN,
	KEY_UP,
	KEY_PRESSED,
	MOUSE_MOVE,
	MOUSE_L_DOWN,
	MOUSE_L_UP,
	MOUSE_R_DOWN,
	MOUSE_R_UP,
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

struct PointXY
{
	int x = 0;
	int y = 0;
	PointXY(int _x = 0, int _y = 0) : x(_x), y(_y) {}
};

struct Vertex_PC
{
	XMFLOAT3 pos0;
	XMFLOAT4 color0;
};

struct Vertex_PCT
{
	XMFLOAT3 pos0;
	XMFLOAT4 color0;
	XMFLOAT2 tex0;
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
template<typename T>
inline const T& Clamp(const T& val, const T& low, const T& high)
{
	_ASEERTION_NULCHK(low <= high, "low larger then high!");
	if (val < low) return low;
	else if (val > high) return high;
	else return val;
}

/*
* 단위행렬
* 1.0f	0.0f	0.0f	0.0f
* 0.0f	1.0f	0.0f	0.0f
* 0.0f	0.0f	1.0f	0.0f
* 0.0f	0.0f	0.0f	1.0f
*/
inline XMMATRIX GetMat_Identity()
{
	XMMATRIX mat;
	mat.r[0] = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	mat.r[1] = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	mat.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	mat.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	return mat;
}

/*
* 스케일행렬
* Sx	0.0f	0.0f	0.0f
* 0.0f	Sy		0.0f	0.0f
* 0.0f	0.0f	Sz		0.0f
* 0.0f	0.0f	0.0f	1.0f
*/
inline XMMATRIX GetMat_Scale(const XMFLOAT3 scale)
{
	XMMATRIX mat;
	mat.r[0] = XMVectorSet(scale.x, 0.0f, 0.0f, 0.0f);
	mat.r[1] = XMVectorSet(0.0f, scale.y, 0.0f, 0.0f);
	mat.r[2] = XMVectorSet(0.0f, 0.0f, scale.z, 0.0f);
	mat.r[3] = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	return mat;
}

/*
* 회전행렬(Pitch)
* 1.0f	0.0f	0.0f	0.0f
* 0.0f	cos		sin		0.0f
* 0.0f	-sin	cos		0.0f
* 0.0f	0.0f	0.0f	1.0f
*/
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

/*
* 회전행렬(Yaw)
* cos	0.0f	-sin	0.0f
* 0.0f	1.0f	0.0f	0.0f
* sin	0.0f	cos		0.0f
* 0.0f	0.0f	0.0f	1.0f
*/
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

/*
* 회전행렬(Roll)
* cos	sin		0.0f	0.0f
* -sin	cos		0.0f	0.0f
* 0.0f	0.0f	1.0f	0.0f
* 0.0f	0.0f	0.0f	1.0f
*/
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

/*
* 회전행렬(합성, UVW)
* Ux		Uy		Uz		0.0f
* Vx		Vx		Vz		0.0f
* Wx		Wy		Wz		0.0f
* 0.0f		0.0f	0.0f	1.0f
*/
inline XMMATRIX GetMat_RotRollPitchYaw(const XMFLOAT3 eulerDegrees)
{
	XMMATRIX matRoll = GetMat_RotRoll(eulerDegrees.z);
	XMMATRIX matPitch = GetMat_RotPitch(eulerDegrees.x);
	XMMATRIX matYaw = GetMat_RotYaw(eulerDegrees.y);
	return matRoll * matPitch * matYaw;
}

/*
* 이동행렬
* 1.0f	0.0f	0.0f	0.0f
* 0.0f	1.0f	0.0f	0.0f
* 0.0f	0.0f	1.0f	0.0f
* Tx	Ty		Tz		1.0f
*/
inline XMMATRIX GetMat_Translation(const XMFLOAT3 translation)
{
	XMMATRIX mat;
	mat.r[0] = XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
	mat.r[1] = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	mat.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	mat.r[3] = XMVectorSet(translation.x, translation.y, translation.z, 1.0f);
	return mat;
}

/*
* 월드행렬(모델링행렬)
* Ux*Sx		Uy*Sx		Uz*Sx		0.0f
* Vx*Sy		Vx*Sy		Vz*Sy		0.0f
* Wx*Sz		Wy*Sz		Wz*Sz		0.0f
* Tx		Ty			Tz			1.0f
*/
inline XMMATRIX GetMat_WorldMatrix(const XMFLOAT3 scale, const XMFLOAT3 eulerDegrees, const XMFLOAT3 translation)
{
	return GetMat_Scale(scale) * GetMat_RotRollPitchYaw(eulerDegrees) * GetMat_Translation(translation);
}

/*
* 뷰행렬
* Ux		Vx			Wx			0.0f
* Uy		Vy			Wy			0.0f
* Uz		Vz			Wz			0.0f
* -QdotU	-QdotV		-QdotW		1.0f
*/
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

inline XMMATRIX GetMat_ViewMatrix(const XMFLOAT3 posCamera, const float fPitch, const float fYaw, const float fRoll)
{
	XMMATRIX matRotate = XMMatrixTranspose(GetMat_RotRollPitchYaw({fPitch, fYaw, fRoll}));		//회전행렬의 전치행렬(역행렬)
	XMMATRIX matTrans = GetMat_Translation({-posCamera.x, -posCamera.y, -posCamera.z});	//이동행렬의 역행렬
	XMMATRIX mat = matTrans * matRotate; //(R*T)^-1 = T^-1 * R^-1
	return mat;
}

/*
* 원근투영행렬
* 1.0f/(r*d)	0.0f		0.0f			0.0f
* 0.0f			1.0f/d		0.0f			0.0f
* 0.0f			0.0f		f/(f-n)			1.0f
* 0.0f			0.0f		-nf/(f-n)		0.0f
*/
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
inline XMMATRIX GetMat_Perspective(float aspectRatio, float fov, float dist_near, float dist_far)
{
	XMMATRIX mat;
	float d = tanf(_DEGTORAD(fov) * 0.5f);
	mat.r[0] = XMVectorSet(1.0f / (aspectRatio * d), 0.0f, 0.0f, 0.0f);
	mat.r[1] = XMVectorSet(0.0f, 1.0f / d, 0.0f, 0.0f);
	mat.r[2] = XMVectorSet(0.0f, 0.0f, dist_far / (dist_far - dist_near), 1.0f);
	mat.r[3] = XMVectorSet(0.0f, 0.0f, -(dist_near * dist_far) / (dist_far - dist_near), 0.0f);
	return mat;
}

/*
* 직교투영행렬
* 2.0f/w		0.0f		0.0f			0.0f
* 0.0f			2.0f/h		0.0f			0.0f
* 0.0f			0.0f		1.0f/(f-n)		0.0f
* 0.0f			0.0f		-n/(f-n)		1.0f
*/
inline XMMATRIX GetMat_Ortho(float width, float height, float dist_near, float dist_far)
{
	XMMATRIX mat;
	mat.r[0] = XMVectorSet(2.0f/width, 0.0f, 0.0f, 0.0f);
	mat.r[1] = XMVectorSet(0.0f, 2.0f/height, 0.0f, 0.0f);
	mat.r[2] = XMVectorSet(0.0f, 0.0f, 1.0f / (dist_far - dist_near), 0.0f);
	mat.r[3] = XMVectorSet(0.0f, 0.0f, -dist_near  / (dist_far - dist_near), 1.0f);
	return mat;
}

/*
* 역행렬을 계산해 반환하는 함수
* 행렬식 2x2 ~ 4x4
* detA = sigma(j to n) A(ij) * (-1)^(i+j) * cofMatA(ij)
*/
//2*2행렬식
inline float GetDeterminant2x2
(
	float m11, float m12,
	float m21, float m22
)
{
	float ret11 = m11 * (float)std::pow(-1, 1 + 1) * m22;
	float ret12 = m12 * (float)std::pow(-1, 1 + 2) * m21;
	return ret11 + ret12;
}
//3*3행렬식
inline float GetDeterminant3x3
(
	float m11, float m12, float m13, 
	float m21, float m22, float m23,
	float m31, float m32, float m33
)
{
	float ret11 = m11 * (float)std::pow(-1, 1 + 1) * GetDeterminant2x2(m22, m23, m32, m33);
	float ret12 = m12 * (float)std::pow(-1, 1 + 2) * GetDeterminant2x2(m21, m23, m31, m33);
	float ret13 = m13 * (float)std::pow(-1, 1 + 3) * GetDeterminant2x2(m21, m22, m31, m32);
	return ret11 + ret12 + ret13;
}
//4*4행렬식
inline float GetDeterminant4x4(const XMMATRIX& mat4x4)
{
	XMFLOAT4 r1(XMVectorGetX(mat4x4.r[0]), XMVectorGetY(mat4x4.r[0]), XMVectorGetZ(mat4x4.r[0]), XMVectorGetW(mat4x4.r[0]));
	XMFLOAT4 r2(XMVectorGetX(mat4x4.r[1]), XMVectorGetY(mat4x4.r[1]), XMVectorGetZ(mat4x4.r[1]), XMVectorGetW(mat4x4.r[1]));
	XMFLOAT4 r3(XMVectorGetX(mat4x4.r[2]), XMVectorGetY(mat4x4.r[2]), XMVectorGetZ(mat4x4.r[2]), XMVectorGetW(mat4x4.r[2]));
	XMFLOAT4 r4(XMVectorGetX(mat4x4.r[3]), XMVectorGetY(mat4x4.r[3]), XMVectorGetZ(mat4x4.r[3]), XMVectorGetW(mat4x4.r[3]));
	float ret11 = r1.x * (float)std::pow(-1, 1 + 1) * GetDeterminant3x3(r2.y, r2.z, r2.w, r3.y, r3.z, r3.w, r4.y, r4.z, r4.w);
	float ret12 = r1.y * (float)std::pow(-1, 1 + 2) * GetDeterminant3x3(r2.x, r2.z, r2.w, r3.x, r3.z, r3.w, r4.x, r4.z, r4.w);
	float ret13 = r1.z * (float)std::pow(-1, 1 + 3) * GetDeterminant3x3(r2.x, r2.y, r2.w, r3.x, r3.y, r3.w, r4.x, r4.y, r4.w);
	float ret14 = r1.w * (float)std::pow(-1, 1 + 4) * GetDeterminant3x3(r2.x, r2.y, r2.z, r3.x, r3.y, r3.z, r4.x, r4.y, r4.z);
	return ret11 + ret12 + ret13 + ret14;
}

/*
* 여인수행렬 C(A)
* (-1)^(i+j) * cofMatA(ij) 로 행렬의 각 요소를 구성시킨다
*/
inline XMMATRIX GetMat_Cofactor(const XMMATRIX& mat4x4)
{
	XMFLOAT4 r1(XMVectorGetX(mat4x4.r[0]), XMVectorGetY(mat4x4.r[0]), XMVectorGetZ(mat4x4.r[0]), XMVectorGetW(mat4x4.r[0]));
	XMFLOAT4 r2(XMVectorGetX(mat4x4.r[1]), XMVectorGetY(mat4x4.r[1]), XMVectorGetZ(mat4x4.r[1]), XMVectorGetW(mat4x4.r[1]));
	XMFLOAT4 r3(XMVectorGetX(mat4x4.r[2]), XMVectorGetY(mat4x4.r[2]), XMVectorGetZ(mat4x4.r[2]), XMVectorGetW(mat4x4.r[2]));
	XMFLOAT4 r4(XMVectorGetX(mat4x4.r[3]), XMVectorGetY(mat4x4.r[3]), XMVectorGetZ(mat4x4.r[3]), XMVectorGetW(mat4x4.r[3]));
	float m11 = (float)std::pow(-1, 1 + 1) * GetDeterminant3x3(r2.y, r2.z, r2.w, r3.y, r3.z, r3.w, r4.y, r4.z, r4.w);
	float m12 = (float)std::pow(-1, 1 + 2) * GetDeterminant3x3(r2.x, r2.z, r2.w, r3.x, r3.z, r3.w, r4.x, r4.z, r4.w);
	float m13 = (float)std::pow(-1, 1 + 3) * GetDeterminant3x3(r2.x, r2.y, r2.w, r3.x, r3.y, r3.w, r4.x, r4.y, r4.w);
	float m14 = (float)std::pow(-1, 1 + 4) * GetDeterminant3x3(r2.x, r2.y, r2.z, r3.x, r3.y, r3.z, r4.x, r4.y, r4.z);

	float m21 = (float)std::pow(-1, 2 + 1) * GetDeterminant3x3(r1.y, r1.z, r1.w, r3.y, r3.z, r3.w, r4.y, r4.z, r4.w);
	float m22 = (float)std::pow(-1, 2 + 2) * GetDeterminant3x3(r1.x, r1.z, r1.w, r3.x, r3.z, r3.w, r4.x, r4.z, r4.w);
	float m23 = (float)std::pow(-1, 2 + 3) * GetDeterminant3x3(r1.x, r1.y, r1.w, r3.x, r3.y, r3.w, r4.x, r4.y, r4.w);
	float m24 = (float)std::pow(-1, 2 + 4) * GetDeterminant3x3(r1.x, r1.y, r1.z, r3.x, r3.y, r3.z, r4.x, r4.y, r4.z);

	float m31 = (float)std::pow(-1, 3 + 1) * GetDeterminant3x3(r1.y, r1.z, r1.w, r2.y, r2.z, r2.w, r4.y, r4.z, r4.w);
	float m32 = (float)std::pow(-1, 3 + 2) * GetDeterminant3x3(r1.x, r1.z, r1.w, r2.x, r2.z, r2.w, r4.x, r4.z, r4.w);
	float m33 = (float)std::pow(-1, 3 + 3) * GetDeterminant3x3(r1.x, r1.y, r1.w, r2.x, r2.y, r2.w, r4.x, r4.y, r4.w);
	float m34 = (float)std::pow(-1, 3 + 4) * GetDeterminant3x3(r1.x, r1.y, r1.z, r2.x, r2.y, r2.z, r4.x, r4.y, r4.z);

	float m41 = (float)std::pow(-1, 4 + 1) * GetDeterminant3x3(r1.y, r1.z, r1.w, r2.y, r2.z, r2.w, r3.y, r3.z, r3.w);
	float m42 = (float)std::pow(-1, 4 + 2) * GetDeterminant3x3(r1.x, r1.z, r1.w, r2.x, r2.z, r2.w, r3.x, r3.z, r3.w);
	float m43 = (float)std::pow(-1, 4 + 3) * GetDeterminant3x3(r1.x, r1.y, r1.w, r2.x, r2.y, r2.w, r3.x, r3.y, r3.w);
	float m44 = (float)std::pow(-1, 4 + 4) * GetDeterminant3x3(r1.x, r1.y, r1.z, r2.x, r2.y, r2.z, r3.x, r3.y, r3.z);
	
	XMMATRIX mat;
	mat.r[0] = XMVectorSet(m11, m12, m13, m14);
	mat.r[1] = XMVectorSet(m21, m22, m23, m24);
	mat.r[2] = XMVectorSet(m31, m32, m33, m34);
	mat.r[3] = XMVectorSet(m41, m42, m43, m44);
	return mat;
}

/*
* 역행렬
* AdjMat(A*) = C(A)^T , 여인수행렬의 전치를 의미, 수반행렬
* InvMat = AdjMat * 1.0f/detA
*/
inline XMMATRIX GetMat_Inverse(const XMMATRIX& matOrigin)
{
	//수반행렬(여인수행렬의 전치행렬)
	XMMATRIX matAdj = XMMatrixTranspose(GetMat_Cofactor(matOrigin));
	//행렬식
	float detA = GetDeterminant4x4(matOrigin);
	//행렬식을 이용한 역행렬 여부 판별
	if (fabs(detA) < _EPSILON)
	{
		//역행렬이 존재하지 아니함(행렬식이 0에 한없이 가까움)
		std::cerr << "InverseMatrix NotExist!" << '\n';
		return GetMat_Identity();
	}
	float invDetA = 1.0f / detA;
	XMMATRIX mat = matAdj * invDetA;
	return mat;
}