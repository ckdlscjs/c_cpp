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
#include "commonMath.h"

//d3d Header
#include <d3d11.h>
#include <DirectXMath.h>					//XMMATRIX
#include <d3dcompiler.h>					//d3dcomplie, ���̴�����������߰�
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")		//d3dcomplie, ���̴�����������߰�
#include <DirectXTex.h>						//Include/DXTEX, �ؽ��Ļ���������߰�
#pragma comment(lib, "DirectXTex.lib")

//Macro
#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#define _ASEERTION_NULCHK(result, message) assert(result && message)
#define _ASEERTION_CREATE(result, message) assert(result == S_OK && message)
#define _DEGTORAD(DEG) DEG / 180.0f * XM_PI
#define _RADTODEG(RAD) RAD / XM_PI * 180.0f
#define _EPSILON 1e-6f
#define _CLAMP(val, low, high) Clamp(val, low, high)

using namespace DirectX;


//enum classes
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

enum class Samplers
{
	WRAP_LINEAR,
	WRAP_ANISOTROPIC,

};

//Resources data struct
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
	Position pos0;
	Vector4 color0;
};

struct Vertex_PCT
{
	Position pos0;
	Vector4 color0;
	Vector2 tex0;
};

struct Vertex_PPCC
{
	Position pos0;
	Position pos1;
	Vector4 color0;
	Vector4 color1;
};

//16����Ʈ ������ gpu�޸𸮿��� �е��ǹǷ� ������ �����
__declspec(align(16))
struct Constant_time
{
	float fTime;
};

__declspec(align(16))
struct Constant_wvp
{
	Matrix4x4 matWorld;
	Matrix4x4 matView;
	Matrix4x4 matProj;
};

//�����Լ���

//��������(vector3)
inline Vector3 lerp(const Vector3& st, const Vector3& en, float t)
{
	return Vector3(st.GetX() * (1.0f - t) + en.GetX() * t, st.GetY() * (1.0f - t) + en.GetY()*t, st.GetZ() * (1.0f - t) + en.GetZ() * t);
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
* �������
* 1.0f	0.0f	0.0f	0.0f
* 0.0f	1.0f	0.0f	0.0f
* 0.0f	0.0f	1.0f	0.0f
* 0.0f	0.0f	0.0f	1.0f
*/
inline Matrix4x4 GetMat_Identity()
{
	Matrix4x4 mat;
	mat[0] = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	mat[1] = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
	mat[2] = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
	mat[3] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	return mat;
}

/*
* ���������
* Sx	0.0f	0.0f	0.0f
* 0.0f	Sy		0.0f	0.0f
* 0.0f	0.0f	Sz		0.0f
* 0.0f	0.0f	0.0f	1.0f
*/
inline Matrix4x4 GetMat_Scale(const Vector3 scale)
{
	Matrix4x4 mat;
	mat[0] = Vector4(scale.GetX(), 0.0f, 0.0f, 0.0f);
	mat[1] = Vector4(0.0f, scale.GetY(), 0.0f, 0.0f);
	mat[2] = Vector4(0.0f, 0.0f, scale.GetZ(), 0.0f);
	mat[3] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	return mat;
}

/*
* ȸ�����(Pitch)
* 1.0f	0.0f	0.0f	0.0f
* 0.0f	cos		sin		0.0f
* 0.0f	-sin	cos		0.0f
* 0.0f	0.0f	0.0f	1.0f
*/
inline Matrix4x4 GetMat_RotPitch(const float degree)
{
	Matrix4x4 mat;
	float radAngle = degree / 180.0f * DirectX::XM_PI;
	float cost = cosf(radAngle);
	float sint = sinf(radAngle);
	mat[0] = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	mat[1] = Vector4(0.0f, cost, sint, 0.0f);
	mat[2] = Vector4(0.0f, -sint, cost, 0.0f);
	mat[3] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	return mat;
}

/*
* ȸ�����(Yaw)
* cos	0.0f	-sin	0.0f
* 0.0f	1.0f	0.0f	0.0f
* sin	0.0f	cos		0.0f
* 0.0f	0.0f	0.0f	1.0f
*/
inline Matrix4x4 GetMat_RotYaw(const float degree)
{
	Matrix4x4 mat;
	float radAngle = _DEGTORAD(degree);
	float cost = cosf(radAngle);
	float sint = sinf(radAngle);
	mat[0] = Vector4(cost, 0.0f, -sint, 0.0f);
	mat[1] = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
	mat[2] = Vector4(sint, 0.0f, cost, 0.0f);
	mat[3] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	return mat;
}

/*
* ȸ�����(Roll)
* cos	sin		0.0f	0.0f
* -sin	cos		0.0f	0.0f
* 0.0f	0.0f	1.0f	0.0f
* 0.0f	0.0f	0.0f	1.0f
*/
inline Matrix4x4 GetMat_RotRoll(const float degree)
{
	Matrix4x4 mat;
	float radAngle = _DEGTORAD(degree);
	float cost = cosf(radAngle);
	float sint = sinf(radAngle);
	mat[0] = Vector4(cost, sint, 0.0f, 0.0f);
	mat[1] = Vector4(-sint, cost, 0.0f, 0.0f);
	mat[2] = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
	mat[3] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	return mat;
}

/*
* ȸ�����(�ռ�, UVW)
* Ux		Uy		Uz		0.0f
* Vx		Vx		Vz		0.0f
* Wx		Wy		Wz		0.0f
* 0.0f		0.0f	0.0f	1.0f
*/
inline Matrix4x4 GetMat_RotRollPitchYaw(const Vector3 eulerDegrees)
{
	Matrix4x4 matRoll = GetMat_RotRoll(eulerDegrees.GetZ());
	Matrix4x4 matPitch = GetMat_RotPitch(eulerDegrees.GetX());
	Matrix4x4 matYaw = GetMat_RotYaw(eulerDegrees.GetY());
	return matRoll * matPitch * matYaw;
}

/*
* �̵����
* 1.0f	0.0f	0.0f	0.0f
* 0.0f	1.0f	0.0f	0.0f
* 0.0f	0.0f	1.0f	0.0f
* Tx	Ty		Tz		1.0f
*/
inline Matrix4x4 GetMat_Translation(const Vector3 translation)
{
	Matrix4x4 mat;
	mat[0] = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	mat[1] = Vector4(0.0f, 1.0f, 0.0f, 0.0f);
	mat[2] = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
	mat[3] = Vector4(translation.GetX(), translation.GetY(), translation.GetZ(), 1.0f);
	return mat;
}

/*
* �������(�𵨸����)
* Ux*Sx		Uy*Sx		Uz*Sx		0.0f
* Vx*Sy		Vx*Sy		Vz*Sy		0.0f
* Wx*Sz		Wy*Sz		Wz*Sz		0.0f
* Tx		Ty			Tz			1.0f
*/

inline Matrix4x4 GetMat_WorldMatrix(const Vector3 scale, const Vector3 eulerDegrees, const Position position)
{
	return GetMat_Scale(scale) * GetMat_RotRollPitchYaw(eulerDegrees) * GetMat_Translation(position.ToVector3());
}

/*
* �����
* Ux		Vx			Wx			0.0f
* Uy		Vy			Wy			0.0f
* Uz		Vz			Wz			0.0f
* -QdotU	-QdotV		-QdotW		1.0f
*/

inline Matrix4x4 GetMat_ViewMatrix(const Position posCamera, const Position posTarget, const Vector3 upVector = Vector3(0.0f, 1.0f, 0.0f))
{
	Matrix4x4 mat;
	//forward, z
	Vector3 w = posTarget - posCamera;
	w = w.Normalize();
	//worldup
	Vector3 j = upVector;
	//right, x
	Vector3 u = CrossProduct(j, w).Normalize();
	//up, y
	Vector3 v = CrossProduct(w, u);
	mat[0] = Vector4(u.GetX(), v.GetX(), w.GetX(), 0.0f);
	mat[1] = Vector4(u.GetY(), v.GetY(), w.GetY(), 0.0f);
	mat[2] = Vector4(u.GetZ(), v.GetZ(), w.GetZ(), 0.0f);
	//-(Q dot u), -(Q dot v), -(Q dot w), 1.0f
	//dot������ ����� x��ҿ�����
	Vector3 camPos = posCamera.ToVector3();
	mat[3] = Vector4(-DotProduct(u, camPos), -DotProduct(v, camPos), -DotProduct(w, camPos), 1.0f);
	return mat;
}

inline Matrix4x4 GetMat_ViewMatrix(const Position posCamera, const float fPitch, const float fYaw, const float fRoll)
{
	Matrix4x4 matRotate = GetMat_RotRollPitchYaw({fPitch, fYaw, fRoll}).Transpose();		//ȸ������� ��ġ���(�����)
	Matrix4x4 matTrans = GetMat_Translation(-posCamera.ToVector3());	//�̵������ �����
	Matrix4x4 mat = matTrans * matRotate; //(R*T)^-1 = T^-1 * R^-1
	return mat;
}

/*
* �����������
* 1.0f/(r*d)	0.0f		0.0f			0.0f
* 0.0f			1.0f/d		0.0f			0.0f
* 0.0f			0.0f		f/(f-n)			1.0f
* 0.0f			0.0f		-nf/(f-n)		0.0f
*/
inline Matrix4x4 GetMat_Perspective(float width, float height, float fov, float dist_near, float dist_far)
{
	Matrix4x4 mat;
	float aspectRatio = width / height;
	float d = tanf(_DEGTORAD(fov) * 0.5f);
	mat[0] = Vector4(1.0f/(aspectRatio * d), 0.0f, 0.0f, 0.0f);
	mat[1] = Vector4(0.0f, 1.0f / d, 0.0f, 0.0f);
	mat[2] = Vector4(0.0f, 0.0f, dist_far / (dist_far-dist_near), 1.0f);
	mat[3] = Vector4(0.0f, 0.0f, -(dist_near*dist_far) / (dist_far - dist_near), 0.0f);
	return mat;
}

inline Matrix4x4 GetMat_Perspective(float aspectRatio, float fov, float dist_near, float dist_far)
{
	Matrix4x4 mat;
	float d = tanf(_DEGTORAD(fov) * 0.5f);
	mat[0] = Vector4(1.0f / (aspectRatio * d), 0.0f, 0.0f, 0.0f);
	mat[1] = Vector4(0.0f, 1.0f / d, 0.0f, 0.0f);
	mat[2] = Vector4(0.0f, 0.0f, dist_far / (dist_far - dist_near), 1.0f);
	mat[3] = Vector4(0.0f, 0.0f, -(dist_near * dist_far) / (dist_far - dist_near), 0.0f);
	return mat;
}

/*
* �����������
* 2.0f/w		0.0f		0.0f			0.0f
* 0.0f			2.0f/h		0.0f			0.0f
* 0.0f			0.0f		1.0f/(f-n)		0.0f
* 0.0f			0.0f		-n/(f-n)		1.0f
*/
inline Matrix4x4 GetMat_Ortho(float width, float height, float dist_near, float dist_far)
{
	Matrix4x4 mat;
	mat[0] = Vector4(2.0f/width, 0.0f, 0.0f, 0.0f);
	mat[1] = Vector4(0.0f, 2.0f/height, 0.0f, 0.0f);
	mat[2] = Vector4(0.0f, 0.0f, 1.0f / (dist_far - dist_near), 0.0f);
	mat[3] = Vector4(0.0f, 0.0f, -dist_near  / (dist_far - dist_near), 1.0f);
	return mat;
}

/*
* ������� ����� ��ȯ�ϴ� �Լ�
* ��Ľ� 2x2 ~ 4x4
* detA = sigma(j to n) A(ij) * (-1)^(i+j) * cofMatA(ij)
*/
//2*2��Ľ�
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
//3*3��Ľ�
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
//4*4��Ľ�
inline float GetDeterminant4x4(const Matrix4x4& mat4x4)
{
	Vector4 r1(mat4x4[0].GetX(), mat4x4[0].GetY(), mat4x4[0].GetZ(), mat4x4[0].GetW());
	Vector4 r2(mat4x4[1].GetX(), mat4x4[1].GetY(), mat4x4[1].GetZ(), mat4x4[1].GetW());
	Vector4 r3(mat4x4[2].GetX(), mat4x4[2].GetY(), mat4x4[2].GetZ(), mat4x4[2].GetW());
	Vector4 r4(mat4x4[3].GetX(), mat4x4[3].GetY(), mat4x4[3].GetZ(), mat4x4[3].GetW());
	float ret11 = r1.GetX() * (float)std::pow(-1, 1 + 1) * GetDeterminant3x3(r2.GetY(), r2.GetZ(), r2.GetW(), r3.GetY(), r3.GetZ(), r3.GetW(), r4.GetY(), r4.GetZ(), r4.GetW());
	float ret12 = r1.GetY() * (float)std::pow(-1, 1 + 2) * GetDeterminant3x3(r2.GetX(), r2.GetZ(), r2.GetW(), r3.GetX(), r3.GetZ(), r3.GetW(), r4.GetX(), r4.GetZ(), r4.GetW());
	float ret13 = r1.GetZ() * (float)std::pow(-1, 1 + 3) * GetDeterminant3x3(r2.GetX(), r2.GetY(), r2.GetW(), r3.GetX(), r3.GetY(), r3.GetW(), r4.GetX(), r4.GetY(), r4.GetW());
	float ret14 = r1.GetW() * (float)std::pow(-1, 1 + 4) * GetDeterminant3x3(r2.GetX(), r2.GetY(), r2.GetZ(), r3.GetX(), r3.GetY(), r3.GetZ(), r4.GetX(), r4.GetY(), r4.GetZ());
	return ret11 + ret12 + ret13 + ret14;
}

/*
* ���μ���� C(A)
* (-1)^(i+j) * cofMatA(ij) �� ����� �� ��Ҹ� ������Ų��
*/
inline Matrix4x4 GetMat_Cofactor(const Matrix4x4& mat4x4)
{
	// ��Ʈ���� ���� Vector4�� ��ȯ (�� ���� ������ �Լ� ���)
	Vector4 r1(mat4x4[0].GetX(), mat4x4[0].GetY(), mat4x4[0].GetZ(), mat4x4[0].GetW());
	Vector4 r2(mat4x4[1].GetX(), mat4x4[1].GetY(), mat4x4[1].GetZ(), mat4x4[1].GetW());
	Vector4 r3(mat4x4[2].GetX(), mat4x4[2].GetY(), mat4x4[2].GetZ(), mat4x4[2].GetW());
	Vector4 r4(mat4x4[3].GetX(), mat4x4[3].GetY(), mat4x4[3].GetZ(), mat4x4[3].GetW());
	// 1���� ���μ�(Cofactor) ��� (Getters ���)
	float m11 = (float)std::pow(-1, 1 + 1) * GetDeterminant3x3(r2.GetY(), r2.GetZ(), r2.GetW(), r3.GetY(), r3.GetZ(), r3.GetW(), r4.GetY(), r4.GetZ(), r4.GetW());
	float m12 = (float)std::pow(-1, 1 + 2) * GetDeterminant3x3(r2.GetX(), r2.GetZ(), r2.GetW(), r3.GetX(), r3.GetZ(), r3.GetW(), r4.GetX(), r4.GetZ(), r4.GetW());
	float m13 = (float)std::pow(-1, 1 + 3) * GetDeterminant3x3(r2.GetX(), r2.GetY(), r2.GetW(), r3.GetX(), r3.GetY(), r3.GetW(), r4.GetX(), r4.GetY(), r4.GetW());
	float m14 = (float)std::pow(-1, 1 + 4) * GetDeterminant3x3(r2.GetX(), r2.GetY(), r2.GetZ(), r3.GetX(), r3.GetY(), r3.GetZ(), r4.GetX(), r4.GetY(), r4.GetZ());

	// 2���� ���μ� ��� (Getters ���)
	float m21 = (float)std::pow(-1, 2 + 1) * GetDeterminant3x3(r1.GetY(), r1.GetZ(), r1.GetW(), r3.GetY(), r3.GetZ(), r3.GetW(), r4.GetY(), r4.GetZ(), r4.GetW());
	float m22 = (float)std::pow(-1, 2 + 2) * GetDeterminant3x3(r1.GetX(), r1.GetZ(), r1.GetW(), r3.GetX(), r3.GetZ(), r3.GetW(), r4.GetX(), r4.GetZ(), r4.GetW());
	float m23 = (float)std::pow(-1, 2 + 3) * GetDeterminant3x3(r1.GetX(), r1.GetY(), r1.GetW(), r3.GetX(), r3.GetY(), r3.GetW(), r4.GetX(), r4.GetY(), r4.GetW());
	float m24 = (float)std::pow(-1, 2 + 4) * GetDeterminant3x3(r1.GetX(), r1.GetY(), r1.GetZ(), r3.GetX(), r3.GetY(), r3.GetZ(), r4.GetX(), r4.GetY(), r4.GetZ());

	// 3���� ���μ� ��� (Getters ���)
	float m31 = (float)std::pow(-1, 3 + 1) * GetDeterminant3x3(r1.GetY(), r1.GetZ(), r1.GetW(), r2.GetY(), r2.GetZ(), r2.GetW(), r4.GetY(), r4.GetZ(), r4.GetW());
	float m32 = (float)std::pow(-1, 3 + 2) * GetDeterminant3x3(r1.GetX(), r1.GetZ(), r1.GetW(), r2.GetX(), r2.GetZ(), r2.GetW(), r4.GetX(), r4.GetZ(), r4.GetW());
	float m33 = (float)std::pow(-1, 3 + 3) * GetDeterminant3x3(r1.GetX(), r1.GetY(), r1.GetW(), r2.GetX(), r2.GetY(), r2.GetW(), r4.GetX(), r4.GetY(), r4.GetW());
	float m34 = (float)std::pow(-1, 3 + 4) * GetDeterminant3x3(r1.GetX(), r1.GetY(), r1.GetZ(), r2.GetX(), r2.GetY(), r2.GetZ(), r4.GetX(), r4.GetY(), r4.GetZ());

	// 4���� ���μ� ��� (Getters ���)
	float m41 = (float)std::pow(-1, 4 + 1) * GetDeterminant3x3(r1.GetY(), r1.GetZ(), r1.GetW(), r2.GetY(), r2.GetZ(), r2.GetW(), r3.GetY(), r3.GetZ(), r3.GetW());
	float m42 = (float)std::pow(-1, 4 + 2) * GetDeterminant3x3(r1.GetX(), r1.GetZ(), r1.GetW(), r2.GetX(), r2.GetZ(), r2.GetW(), r3.GetX(), r3.GetZ(), r3.GetW());
	float m43 = (float)std::pow(-1, 4 + 3) * GetDeterminant3x3(r1.GetX(), r1.GetY(), r1.GetW(), r2.GetX(), r2.GetY(), r2.GetW(), r3.GetX(), r3.GetY(), r3.GetW());
	float m44 = (float)std::pow(-1, 4 + 4) * GetDeterminant3x3(r1.GetX(), r1.GetY(), r1.GetZ(), r2.GetX(), r2.GetY(), r2.GetZ(), r3.GetX(), r3.GetY(), r3.GetZ());
	
	Matrix4x4 mat;
	mat[0] = Vector4(m11, m12, m13, m14);
	mat[1] = Vector4(m21, m22, m23, m24);
	mat[2] = Vector4(m31, m32, m33, m34);
	mat[3] = Vector4(m41, m42, m43, m44);
	return mat;
}

/*
* �����
* AdjMat(A*) = C(A)^T , ���μ������ ��ġ�� �ǹ�, �������
* InvMat = AdjMat * 1.0f/detA
*/
inline Matrix4x4 GetMat_Inverse(const Matrix4x4& matOrigin)
{
#ifndef __OPTIMIZED
	//�������(���μ������ ��ġ���)
	Matrix4x4 matAdj = GetMat_Cofactor(matOrigin).Transpose();
	//��Ľ�
	float detA = GetDeterminant4x4(matOrigin);
	//��Ľ��� �̿��� ����� ���� �Ǻ�
	if (fabs(detA) < _EPSILON)
	{
		//������� �������� �ƴ���(��Ľ��� 0�� �Ѿ��� �����)
		std::cerr << "InverseMatrix NotExist!" << '\n';
		return GetMat_Identity();
	}
	float invDetA = 1.0f / detA;
	Matrix4x4 matInverse = matAdj * invDetA;
	return matInverse;
#else
	XMVECTOR deteminant;
	Matrix4x4 matInverse = XMMatrixInverse(&deteminant, matOrigin.ToXMMATRIX());
	if (XMVector4NearEqual(deteminant, XMVectorZero(), DirectX::XMVectorSplatEpsilon()))
	{
		//������� �������� �ƴ���(��Ľ��� 0�� �Ѿ��� �����)
		std::cerr << "InverseMatrix NotExist!" << '\n';
		return GetMat_Identity();
	}	

	return matInverse;
#endif 
}