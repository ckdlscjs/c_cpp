#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Quaternion.h"

//선형보간(vector3)
inline Vector3 lerp(const Vector3& st, const Vector3& en, float t)
{
	return Vector3(st.GetX() * (1.0f - t) + en.GetX() * t, st.GetY() * (1.0f - t) + en.GetY() * t, st.GetZ() * (1.0f - t) + en.GetZ() * t);
}

/*
* 단위행렬
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
* 스케일행렬
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
* 회전행렬(Pitch)
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
* 회전행렬(Yaw)
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
* 회전행렬(Roll)
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
* 회전행렬(합성, UVW)
* Ux		Uy		Uz		0.0f
* Vx		Vy		Vz		0.0f
* Wx		Wy		Wz		0.0f
* 0.0f		0.0f	0.0f	1.0f
*/
inline Matrix4x4 GetMat_RotateRollPitchYaw(const Vector3& eulerDegrees)
{
	Matrix4x4 matRoll = GetMat_RotRoll(eulerDegrees.GetZ());
	Matrix4x4 matPitch = GetMat_RotPitch(eulerDegrees.GetX());
	Matrix4x4 matYaw = GetMat_RotYaw(eulerDegrees.GetY());
	return matRoll * matPitch * matYaw;
}

/*
* 이동행렬
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
* 월드행렬(모델링행렬)
* Ux*Sx		Uy*Sx		Uz*Sx		0.0f
* Vx*Sy		Vy*Sy		Vz*Sy		0.0f
* Wx*Sz		Wy*Sz		Wz*Sz		0.0f
* Tx		Ty			Tz			1.0f
*/

inline Matrix4x4 GetMat_World(const Vector3& scale, const Vector3& eulerDegrees, const Vector3& position)
{
	return GetMat_Scale(scale) * GetMat_RotateRollPitchYaw(eulerDegrees) * GetMat_Translation(position);
}

//각 회전축의 길이는 스케일값이 곱해져있으므로 길이값이 각 x, y, z스케일값을의미한다
inline Vector3 GetScaleFromWorld(const Matrix4x4& matWorld)
{
	return Vector3(Vector3(matWorld[0].GetX(), matWorld[1].GetX(), matWorld[2].GetX()).Length(), Vector3(matWorld[0].GetY(), matWorld[1].GetY(), matWorld[2].GetY()).Length(), Vector3(matWorld[0].GetZ(), matWorld[1].GetZ(), matWorld[2].GetZ()).Length());
}

//정규화된 좌표축을 가져온다 right, up, forward
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

//사원수 추가후 추가예정
//inline void DecomposeFromWorld(const Matrix4x4& matWorld, Vector3* scale, Vector3* rotate, Vector3* translation)
//{
//	*scale = GetScaleFromWorld(matWorld);
//}

/*
* 뷰행렬
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
	//dot내적의 결과가 x요소에있음
	Vector3 vPosition = posCamera;
	mat[3] = Vector4(-DotProduct(u, vPosition), -DotProduct(v, vPosition), -DotProduct(w, vPosition), 1.0f);
	return mat;
}

inline Matrix4x4 GetMat_View(const Vector3& posCamera, const float& fPitch, const float& fYaw, const float& fRoll)
{
	Matrix4x4 matRotate = GetMat_RotateRollPitchYaw({ fPitch, fYaw, fRoll }).Transpose();		//회전행렬의 전치행렬(역행렬)
	Matrix4x4 matTrans = GetMat_Translation(-posCamera);	//이동행렬의 역행렬
	Matrix4x4 mat = matTrans * matRotate; //(R*T)^-1 = T^-1 * R^-1
	return mat;
}

/*
* 원근투영행렬
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
* 직교투영행렬
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
* 직교투영행렬
* 2.0f/(r-l)		0.0f			0.0f			0.0f
* 0.0f				2.0f/(t-b)		0.0f			0.0f
* 0.0f				0.0f			1.0f/(f-n)		0.0f
* -(r+l)/(r-l)		(b+t)/(t-b)		-n/(f-n)		1.0f
*/
inline Matrix4x4 GetMat_Orthographic_OffCenter(float left, float right, float top, float bottom, float dist_near, float dist_far)
{
	Matrix4x4 mat;
	float range_x = right - left; //양수값
	float range_y = top - bottom; //음수값
	mat[0] = Vector4(2.0f / range_x, 0.0f, 0.0f, 0.0f);
	mat[1] = Vector4(0.0f, 2.0f / range_y, 0.0f, 0.0f);
	mat[2] = Vector4(0.0f, 0.0f, 1.0f / (dist_far - dist_near), 0.0f);
	mat[3] = Vector4(-(right + left) / range_x, -(top + bottom) / range_y, -dist_near / (dist_far - dist_near), 1.0f);
	return mat;
}

/*
* 반사행렬
* D = n dot v + d -> 평면의방정식(ax + by + cz + d)
* 반사벡터 v' = v - 2*D*n, 이를 v * M 형태로 연산시킨다, 행렬의 한 열이 반사벡터식의 분해요소곱을 의미한다
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
* 스크린좌표(뷰포트)행렬
* [스크린 좌표 행렬 (Viewport Matrix)]
* - (0.0, 0.0) ~ (width, height) (픽셀 단위)
* - GPU 하드웨어가 3D 점을 실제 모니터의 어느 '픽셀'에 점을 찍을지 결정할 때 사용.
* - 윈도우 창 크기(w, h)에 직접적으로 의존합니다.
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
* NDC(-1 ~ 1) to TextureSpace
* [텍스처 변환 행렬 (Bias Matrix)]
* - (0.0, 0.0) ~ (1.0, 1.0)
* - 쉐이더 코드 내부에서 `Texture.Sample(uv)`를 호출하기 위한 좌표 생성.
*/
inline Matrix4x4 GetMat_TextureSpace()
{
	Matrix4x4 mat;
	mat[0] = Vector4(0.5f, 0.0f, 0.0f, 0.0f);
	mat[1] = Vector4(0.0f, -0.5f, 0.0f, 0.0f);
	mat[2] = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
	mat[3] = Vector4(0.5f, 0.5f, 0.0f, 1.0f);
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
	float ret11 = m11 * +m22;
	float ret12 = m12 * -m21;
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
	float ret11 = m11 * +GetDeterminant2x2(m22, m23, m32, m33);
	float ret12 = m12 * -GetDeterminant2x2(m21, m23, m31, m33);
	float ret13 = m13 * +GetDeterminant2x2(m21, m22, m31, m32);
	return ret11 + ret12 + ret13;
}
//4*4행렬식
inline float GetDeterminant4x4(const Matrix4x4& mat4x4)
{
	Vector4 r1(mat4x4[0]);
	Vector4 r2(mat4x4[1]);
	Vector4 r3(mat4x4[2]);
	Vector4 r4(mat4x4[3]);
	float ret11 = r1.GetX() * +GetDeterminant3x3(r2.GetY(), r2.GetZ(), r2.GetW(), r3.GetY(), r3.GetZ(), r3.GetW(), r4.GetY(), r4.GetZ(), r4.GetW());
	float ret12 = r1.GetY() * -GetDeterminant3x3(r2.GetX(), r2.GetZ(), r2.GetW(), r3.GetX(), r3.GetZ(), r3.GetW(), r4.GetX(), r4.GetZ(), r4.GetW());
	float ret13 = r1.GetZ() * +GetDeterminant3x3(r2.GetX(), r2.GetY(), r2.GetW(), r3.GetX(), r3.GetY(), r3.GetW(), r4.GetX(), r4.GetY(), r4.GetW());
	float ret14 = r1.GetW() * -GetDeterminant3x3(r2.GetX(), r2.GetY(), r2.GetZ(), r3.GetX(), r3.GetY(), r3.GetZ(), r4.GetX(), r4.GetY(), r4.GetZ());
	return ret11 + ret12 + ret13 + ret14;
}

/*
* 여인수행렬 C(A)
* (-1)^(i+j) * cofMatA(ij) 로 행렬의 각 요소를 구성시킨다
*/
inline Matrix4x4 GetMat_Cofactor(const Matrix4x4& mat4x4)
{
	// 매트릭스 행을 Vector4로 변환 (각 성분 접근자 함수 사용)
	Vector4 r1(mat4x4[0]);
	Vector4 r2(mat4x4[1]);
	Vector4 r3(mat4x4[2]);
	Vector4 r4(mat4x4[3]);

	// 1행의 여인수(Cofactor) 계산 (Getters 사용)
	float m11 = +GetDeterminant3x3(r2.GetY(), r2.GetZ(), r2.GetW(), r3.GetY(), r3.GetZ(), r3.GetW(), r4.GetY(), r4.GetZ(), r4.GetW());
	float m12 = -GetDeterminant3x3(r2.GetX(), r2.GetZ(), r2.GetW(), r3.GetX(), r3.GetZ(), r3.GetW(), r4.GetX(), r4.GetZ(), r4.GetW());
	float m13 = +GetDeterminant3x3(r2.GetX(), r2.GetY(), r2.GetW(), r3.GetX(), r3.GetY(), r3.GetW(), r4.GetX(), r4.GetY(), r4.GetW());
	float m14 = -GetDeterminant3x3(r2.GetX(), r2.GetY(), r2.GetZ(), r3.GetX(), r3.GetY(), r3.GetZ(), r4.GetX(), r4.GetY(), r4.GetZ());

	// 2행의 여인수 계산 (Getters 사용)
	float m21 = -GetDeterminant3x3(r1.GetY(), r1.GetZ(), r1.GetW(), r3.GetY(), r3.GetZ(), r3.GetW(), r4.GetY(), r4.GetZ(), r4.GetW());
	float m22 = +GetDeterminant3x3(r1.GetX(), r1.GetZ(), r1.GetW(), r3.GetX(), r3.GetZ(), r3.GetW(), r4.GetX(), r4.GetZ(), r4.GetW());
	float m23 = -GetDeterminant3x3(r1.GetX(), r1.GetY(), r1.GetW(), r3.GetX(), r3.GetY(), r3.GetW(), r4.GetX(), r4.GetY(), r4.GetW());
	float m24 = +GetDeterminant3x3(r1.GetX(), r1.GetY(), r1.GetZ(), r3.GetX(), r3.GetY(), r3.GetZ(), r4.GetX(), r4.GetY(), r4.GetZ());

	// 3행의 여인수 계산 (Getters 사용)
	float m31 = +GetDeterminant3x3(r1.GetY(), r1.GetZ(), r1.GetW(), r2.GetY(), r2.GetZ(), r2.GetW(), r4.GetY(), r4.GetZ(), r4.GetW());
	float m32 = -GetDeterminant3x3(r1.GetX(), r1.GetZ(), r1.GetW(), r2.GetX(), r2.GetZ(), r2.GetW(), r4.GetX(), r4.GetZ(), r4.GetW());
	float m33 = +GetDeterminant3x3(r1.GetX(), r1.GetY(), r1.GetW(), r2.GetX(), r2.GetY(), r2.GetW(), r4.GetX(), r4.GetY(), r4.GetW());
	float m34 = -GetDeterminant3x3(r1.GetX(), r1.GetY(), r1.GetZ(), r2.GetX(), r2.GetY(), r2.GetZ(), r4.GetX(), r4.GetY(), r4.GetZ());

	// 4행의 여인수 계산 (Getters 사용)
	float m41 = -GetDeterminant3x3(r1.GetY(), r1.GetZ(), r1.GetW(), r2.GetY(), r2.GetZ(), r2.GetW(), r3.GetY(), r3.GetZ(), r3.GetW());
	float m42 = +GetDeterminant3x3(r1.GetX(), r1.GetZ(), r1.GetW(), r2.GetX(), r2.GetZ(), r2.GetW(), r3.GetX(), r3.GetZ(), r3.GetW());
	float m43 = -GetDeterminant3x3(r1.GetX(), r1.GetY(), r1.GetW(), r2.GetX(), r2.GetY(), r2.GetW(), r3.GetX(), r3.GetY(), r3.GetW());
	float m44 = +GetDeterminant3x3(r1.GetX(), r1.GetY(), r1.GetZ(), r2.GetX(), r2.GetY(), r2.GetZ(), r3.GetX(), r3.GetY(), r3.GetZ());

	Matrix4x4 mat;
	mat[0] = Vector4(m11, m12, m13, m14);
	mat[1] = Vector4(m21, m22, m23, m24);
	mat[2] = Vector4(m31, m32, m33, m34);
	mat[3] = Vector4(m41, m42, m43, m44);
	return mat;
}

/*
* 역행렬
* AdjMat(A*) = C(A)^T , 여인수행렬의 전치를 의미, 수반행렬
* InvMat = AdjMat * 1.0f/detA
*/
inline Matrix4x4 GetMat_Inverse(const Matrix4x4& matOrigin)
{
#ifndef __OPTIMIZED
	float detA = GetDeterminant4x4(matOrigin);

	// 행렬식을 이용한 역행렬 여부 판별 (허용 범위를 대폭 넓힘)
	if (std::abs(detA) < _EPSILON)
	{
		std::cerr << "InverseMatrix NotExist! Det: " << detA << '\n';
		return GetMat_Identity();
	}

	// 수반행렬(여인수행렬의 전치행렬)
	Matrix4x4 matAdj = GetMat_Cofactor(matOrigin).Transpose();

	float invDetA = 1.0f / detA;
	Matrix4x4 matInverse = matAdj * invDetA;
	return matInverse;
#else
	XMVECTOR deteminant;
	Matrix4x4 matInverse = XMMatrixInverse(&deteminant, matOrigin.ToXMMATRIX());
	if (XMVector4NearEqual(deteminant, XMVectorZero(), DirectX::XMVectorSplatEpsilon()))
	{
		//역행렬이 존재하지 아니함(행렬식이 0에 한없이 가까움)
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
inline Matrix4x4 GetMat_RotateFromQuarternion(const Quarternion& q)
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

inline Matrix4x4 GetMat_World(const Vector3& scale, const Quarternion& quarternion, const Vector3& position)
{
	return GetMat_Scale(scale) * GetMat_RotateFromQuarternion(quarternion) * GetMat_Translation(position);
}

inline Matrix4x4 GetMat_View(const Vector3& posCamera, const Quarternion& q)
{
	Matrix4x4 matRotate = GetMat_RotateFromQuarternion(q).Transpose();	//회전행렬의 역행렬
	Matrix4x4 matTrans = GetMat_Translation(-posCamera);	//이동행렬의 역행렬
	Matrix4x4 mat = matTrans * matRotate; //(R*T)^-1 = T^-1 * R^-1
	return mat;
}
/*
* 구면선형보간
* q' = 알파 * q1 + 베타 * q2
* v(복소평면상벡터)에 비례시 알파*x + 베타*y
* 특정각만큼 회전, 이때 전체회전의 t만큼일시 t*theta이므로
* v = cos(t theta) * x + sin(t theta) * y
* q2(u벡터, 전체값세타만큼회전됨) -> cos(theta) * x + sin(theta) * y 이므로
* y = (u - cos(theta) * x) / sin(theta), 이를 v에대입 sin합차공식을 이용해
* (sin(theta)cos(t theta) - cos(theta)sin(t theta)) / sin(theta) * x + sin(t theta) / sin(theta) * u
* 알파 -> sin((1-t) theta) / sin(theta)
* 베타 -> sin(t theta) / sin(theta)
* q' = sin(1-t(theta)) / sin(theta) * q1 + sin(t theta) / sin(theta) * q2
* 
* 내적을 이용해서 theta를 구한다
* dotproduct = |a||b|cos(theta) -> a dot b, a, b는 q1/q2 즉 회전사원수 이므로 크기가1이다
* 즉 a dot b = cost(theta) 가 되므로 acosf으로 theta를 유도한다
* 
* 이때 내적을 이용한 결과가 양수일경우 그대로사용, 음수일경우 반대경로를사용하며 내적의 결과가 1에 가까울경우 평행하므로
* 선형보간을 사용한다
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
		//선형보간 q' = (1-t)*q1 + t*q2
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

/*
* 방향으로부터 사원수를 생성한다, 임의의 방향을 기저벡터의 forward와 같은평면상에 있다고
* 가정한다면 이 둘을 외적하면 사원수의 임의회전축을 얻을수 있고 이 두벡터의 사잇각을 이용해
* (cos(t), sin(t) * n) 을 구성한다
*/
inline Quarternion GetQuarternionFromDirection(const Vector3& direction)
{
	//두 벡터를 구성한다
	Vector3 vForward(0.0f, 0.0f, 1.0f);
	Vector3 vDirection = direction.Normalize();
	
	//두 벡터의 각을 구한다
	//n dot v = |n||v| cos t n v는 정규화되어있으므로 n dot v 는 cos t
	//arccos의 범위는 -1~1이어야 하므로 오차방지를 위해 clamp시킨다
	float cost = vForward.DotProduct(vDirection);
	cost = Clamp(cost, -1.0f, 1.0f);
	float angle = acosf(cost); //radian

	//예외처리
	//1. 같은방향일경우
	if (cost > 1.0f - _EPSILON)
		return Quarternion();
	
	//2. 반대방향일경우, yaw를 기준축으로 삼는다
	if (cost < -1.0f + _EPSILON)
		return Quarternion(0.0f, 0.0f, 1.0f, 0.0f);

	//일반적인경우, 두벡터의 외적을통해 기준축을 구한다
	Vector3 rotateAxis = vForward.CrossProduct(vDirection);	//forward to dir
	rotateAxis = rotateAxis.Normalize();
	angle *= 0.5f;	//q v q', theta/2를 사용한다
	float cosTheta = cosf(angle);
	float sinTheta = sinf(angle);
	return Quarternion(cosTheta, rotateAxis * sinTheta);
}

