#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Quaternion.h"

//��������(vector3)
inline Vector3 lerp(const Vector3& st, const Vector3& en, float t)
{
	return Vector3(st.GetX() * (1.0f - t) + en.GetX() * t, st.GetY() * (1.0f - t) + en.GetY() * t, st.GetZ() * (1.0f - t) + en.GetZ() * t);
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
inline Matrix4x4 GetMat_Scale(const Vector3& scale)
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
* Vx		Vy		Vz		0.0f
* Wx		Wy		Wz		0.0f
* 0.0f		0.0f	0.0f	1.0f
*/
inline Matrix4x4 GetMat_RotRollPitchYaw(const Vector3& eulerDegrees)
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
inline Matrix4x4 GetMat_Translation(const Vector3& translation)
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
* Vx*Sy		Vy*Sy		Vz*Sy		0.0f
* Wx*Sz		Wy*Sz		Wz*Sz		0.0f
* Tx		Ty			Tz			1.0f
*/

inline Matrix4x4 GetMat_WorldMatrix(const Vector3& scale, const Vector3& eulerDegrees, const Vector3& position)
{
	return GetMat_Scale(scale) * GetMat_RotRollPitchYaw(eulerDegrees) * GetMat_Translation(position);
}

//�� ȸ������ ���̴� �����ϰ��� �����������Ƿ� ���̰��� �� x, y, z�����ϰ����ǹ��Ѵ�
inline Vector3 GetScaleFromWorld(const Matrix4x4& matWorld)
{
	return Vector3(Vector3(matWorld[0].GetX(), matWorld[1].GetX(), matWorld[2].GetX()).Length(), Vector3(matWorld[0].GetY(), matWorld[1].GetY(), matWorld[2].GetY()).Length(), Vector3(matWorld[0].GetZ(), matWorld[1].GetZ(), matWorld[2].GetZ()).Length());
}

//����ȭ�� ��ǥ���� �����´� right, up, forward
inline Vector3 GetAxesRightFromWorld(const Matrix4x4& matWorld)
{
	Vector3 vScale = GetScaleFromWorld(matWorld);
	return Vector3(matWorld[0].GetX(), matWorld[0].GetY(), matWorld[0].GetZ()) / vScale.GetX();
}

inline Vector3 GetAxesUpFromWorld(const Matrix4x4& matWorld)
{
	Vector3 vScale = GetScaleFromWorld(matWorld);
	return Vector3(matWorld[1].GetX(), matWorld[1].GetY(), matWorld[1].GetZ()).Normalize() / vScale.GetY();
}

inline Vector3 GetAxesForwardFromWorld(const Matrix4x4& matWorld)
{
	Vector3 vScale = GetScaleFromWorld(matWorld);
	return Vector3(matWorld[2].GetX(), matWorld[2].GetY(), matWorld[2].GetZ()).Normalize() / vScale.GetZ();
}

inline Vector3 GetTranslationFromWorld(const Matrix4x4& matWorld)
{
	return matWorld[3].ToVector3();
}
inline Matrix4x4 GetMat_RotFromMatrix(const Matrix4x4& matSource)
{
	Matrix4x4 mat;
	mat[0] = Vector4(matSource[0].ToVector3(), 0.0f);
	mat[1] = Vector4(matSource[1].ToVector3(), 0.0f);
	mat[2] = Vector4(matSource[2].ToVector3(), 0.0f);
	mat[3] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	return mat;
}

//����� �߰��� �߰�����
//inline void DecomposeFromWorld(const Matrix4x4& matWorld, Vector3* scale, Vector3* rotate, Vector3* translation)
//{
//	*scale = GetScaleFromWorld(matWorld);
//}

/*
* �����
* Ux		Vx			Wx			0.0f
* Uy		Vy			Wy			0.0f
* Uz		Vz			Wz			0.0f
* -QdotU	-QdotV		-QdotW		1.0f
*/

inline Matrix4x4 GetMat_View(const Vector3& posCamera, const Vector3& posTarget, const Vector3& upVector = Vector3(0.0f, 1.0f, 0.0f))
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
	Vector3 vPosition = posCamera;
	mat[3] = Vector4(-DotProduct(u, vPosition), -DotProduct(v, vPosition), -DotProduct(w, vPosition), 1.0f);
	return mat;
}

inline Matrix4x4 GetMat_View(const Vector3& posCamera, const float& fPitch, const float& fYaw, const float& fRoll)
{
	Matrix4x4 matRotate = GetMat_RotRollPitchYaw({ fPitch, fYaw, fRoll }).Transpose();		//ȸ������� ��ġ���(�����)
	Matrix4x4 matTrans = GetMat_Translation(-posCamera);	//�̵������ �����
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
	mat[0] = Vector4(1.0f / (aspectRatio * d), 0.0f, 0.0f, 0.0f);
	mat[1] = Vector4(0.0f, 1.0f / d, 0.0f, 0.0f);
	mat[2] = Vector4(0.0f, 0.0f, dist_far / (dist_far - dist_near), 1.0f);
	mat[3] = Vector4(0.0f, 0.0f, -(dist_near * dist_far) / (dist_far - dist_near), 0.0f);
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
inline Matrix4x4 GetMat_Orthographic(float width, float height, float dist_near, float dist_far)
{
	Matrix4x4 mat;
	mat[0] = Vector4(2.0f / width, 0.0f, 0.0f, 0.0f);
	mat[1] = Vector4(0.0f, 2.0f / height, 0.0f, 0.0f);
	mat[2] = Vector4(0.0f, 0.0f, 1.0f / (dist_far - dist_near), 0.0f);
	mat[3] = Vector4(0.0f, 0.0f, -dist_near / (dist_far - dist_near), 1.0f);
	return mat;
}

/*
* -0.5~0.5 ->0.0~1.0
*/
inline Matrix4x4 GetMat_ConvertGeometryOrtho()
{
	Matrix4x4 mat;
	mat[0] = Vector4(1.0f, 0.0f, 0.0f, 0.0f);
	mat[1] = Vector4(0.0f, -1.0f, 0.0f, 0.0f);
	mat[2] = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
	mat[3] = Vector4(0.5f, 0.5f, 0.0f, 1.0f);
	return mat;
}
/*
* �����������
* 2.0f/(r-l)		0.0f			0.0f			0.0f
* 0.0f				2.0f/(t-b)		0.0f			0.0f
* 0.0f				0.0f			1.0f/(f-n)		0.0f
* -(r+l)/(r-l)		(b+t)/(t-b)		-n/(f-n)		1.0f
*/
inline Matrix4x4 GetMat_Orthographic_OffCenter(float left, float right, float top, float bottom, float dist_near, float dist_far)
{
	Matrix4x4 mat;
	float range_x = right - left; //�����
	float range_y = top - bottom; //������
	mat[0] = Vector4(2.0f / range_x, 0.0f, 0.0f, 0.0f);
	mat[1] = Vector4(0.0f, 2.0f / range_y, 0.0f, 0.0f);
	mat[2] = Vector4(0.0f, 0.0f, 1.0f / (dist_far - dist_near), 0.0f);
	mat[3] = Vector4(-(right + left) / range_x, -(top + bottom) / range_y, -dist_near / (dist_far - dist_near), 1.0f);
	return mat;
}

/*
* �ݻ����
* D = n dot v + d -> ����ǹ�����(ax + by + cz + d)
* �ݻ纤�� v' = v - 2*D*n, �̸� v * M ���·� �����Ų��, ����� �� ���� �ݻ纤�ͽ��� ���ؿ�Ұ��� �ǹ��Ѵ�
* 1-2a^2			-2ab			-2ac				0.0f
* -2ab				1-2b^2			-2bc				0.0f
* -2ac				-2bc			1-2c^2				0.0f
* -2ad				-2bd			-2cd				1.0f
*/
inline Matrix4x4 GetMat_Reflect(const Vector4& plane)
{
	float a = plane.GetX(); float b = plane.GetY(); float c = plane.GetZ(); float d = plane.GetW();
	Matrix4x4 mat;
	mat[0] = Vector4(1.0f - 2.0f * a * a, -2.0f * a * b, -2.0f * a * c, 0.0f);
	mat[1] = Vector4(-2.0f * a * b, 1.0f - 2.0f * b * b, -2.0f * b * c, 0.0f);
	mat[2] = Vector4(-2.0f * a * c, -2.0f * b * c, 1.0f - 2.0f * c * c, 0.0f);
	mat[3] = Vector4(-2.0f * a * d, -2.0f * b * d, -2.0f * c * d, 1.0f);
	return mat;
}

/*
* ��ũ����ǥ���
* w/2.0f			0.0f			0.0f				0.0f
* 0.0f				-h/2.0f			0.0f				0.0f
* 0.0f				0.0f			MaxDepth-MinDepth	0.0f
* w/2.0f+Left		h/2.0f+Top		MinDepth			1.0f
*/
inline Matrix4x4 GetMat_ScreenSpace(float width, float height, float left = 0.0f, float top = 0.0f, float depth_min = 0.0f, float depth_max = 1.0f)
{
	Matrix4x4 mat;
	mat[0] = Vector4(width / 2.0f, 0.0f, 0.0f, 0.0f);
	mat[1] = Vector4(0.0f, -height / 2.0f, 0.0f, 0.0f);
	mat[2] = Vector4(0.0f, 0.0f, depth_max - depth_min, 0.0f);
	mat[3] = Vector4(width / 2.0f + left, height / 2.0f + top, depth_min, 1.0f);
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

inline Matrix4x4 GetMat_InverseTranspose(Matrix4x4 mat)
{
	mat[3] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	return GetMat_Inverse(mat).Transpose();
}

/*
* 1.0f-2.0f(y^2 + z^2)		2.0f(xy + wz)			2.0f(xz - wy)			0.0f
* 2.0f(xy - wz)				1.0f-2.0f(x^2+z^2)		2.0f(yz + wx)			0.0f
* 2.0f(xz + wy)				2.0f(yz - wx)			1.0f-2.0f(x^2+y^2)		0.0f
* 0.0f						0.0f					0.0f					1.0f
*/
inline Matrix4x4 GetMat_FromQuarternion(const Quarternion& q)
{
	Matrix4x4 mat;
	float w = q.GetW();
	float x = q.GetV().GetX();
	float y = q.GetV().GetY();
	float z = q.GetV().GetZ();

	float xs = x * x;
	float ys = y * y;
	float zs = z * z;

	float xy = x * y;
	float xz = x * z;
	float wx = w * x;

	float yz = y * z;
	float wy = w * y;

	float wz = w * z;

	mat[0] = Vector4(1.0f - 2.0f * (ys + zs), 2.0f * (xy + wz), 2.0f * (xz - wy), 0.0f);
	mat[1] = Vector4(2.0f * (xy - wz), 1.0f - 2.0f * (xs + zs), 2.0f * (yz + wx), 0.0f);
	mat[2] = Vector4(2.0f * (xz + wy), 2.0f * (yz - wx), 1.0f - 2.0f * (xs + ys), 0.0f);
	mat[3] = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
	return mat;
}

inline Matrix4x4 GetMat_View(const Vector3& posCamera, const Quarternion& q)
{
	Matrix4x4 matRotate = GetMat_FromQuarternion(q).Transpose();	//ȸ������� �����
	Matrix4x4 matTrans = GetMat_Translation(-posCamera);	//�̵������ �����
	Matrix4x4 mat = matTrans * matRotate; //(R*T)^-1 = T^-1 * R^-1
	return mat;
}
/*
* ���鼱������
* q' = ���� * q1 + ��Ÿ * q2
* v(����������)�� ��ʽ� ����*x + ��Ÿ*y
* Ư������ŭ ȸ��, �̶� ��üȸ���� t��ŭ�Ͻ� t*theta�̹Ƿ�
* v = cos(t theta) * x + sin(t theta) * y
* q2(u����, ��ü����Ÿ��ŭȸ����) -> cos(theta) * x + sin(theta) * y �̹Ƿ�
* y = (u - cos(theta) * x) / sin(theta), �̸� v������ sin���������� �̿���
* (sin(theta)cos(t theta) - cos(theta)sin(t theta)) / sin(theta) * x + sin(t theta) / sin(theta) * u
* ���� -> sin((1-t) theta) / sin(theta)
* ��Ÿ -> sin(t theta) / sin(theta)
* q' = sin(1-t(theta)) / sin(theta) * q1 + sin(t theta) / sin(theta) * q2
* 
* ������ �̿��ؼ� theta�� ���Ѵ�
* dotproduct = |a||b|cos(theta) -> a dot b, a, b�� q1/q2 �� ȸ������� �̹Ƿ� ũ�Ⱑ1�̴�
* �� a dot b = cost(theta) �� �ǹǷ� acosf���� theta�� �����Ѵ�
* 
* �̶� ������ �̿��� ����� ����ϰ�� �״�λ��, �����ϰ�� �ݴ��θ�����ϸ� ������ ����� 1�� ������� �����ϹǷ�
* ���������� ����Ѵ�
*/
inline Quarternion Slerp(Quarternion q1, Quarternion q2, float t)
{
	float a = 0.0f, b = 0.0f;
	float dot = q1.DotProduct(q2);
	if (dot < 0.0f)
	{
		q2 = -q2;
		dot = -dot;
	}
	if (std::abs(dot - 1.0f) <= _EPSILON)
	{
		//�������� q' = (1-t)*q1 + t*q2
		a = 1.0f - t;
		b = t;
	}
	else
	{
		float theta = acosf(dot);
		float sin_theta = sinf(theta);
		a = sinf((1.0f - t) * theta) / sin_theta;
		b = sinf(t * theta) / sin_theta;
	}
	return Quarternion(q1 * a + q2 * b);
}