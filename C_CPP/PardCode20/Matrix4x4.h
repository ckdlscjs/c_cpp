#pragma once
#include "Vector4.h"

__declspec(align(16))
struct Matrix4x4
{
    // --- 생성자 ---
    // 기본 생성자: 단위 행렬로 초기화
    inline Matrix4x4() : m_mat(DirectX::XMMatrixIdentity()) {}

    // 16개 float 성분으로 직접 초기화
    inline Matrix4x4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33)
    {
        m_mat = DirectX::XMMatrixSet(
            m00, m01, m02, m03,
            m10, m11, m12, m13,
            m20, m21, m22, m23,
            m30, m31, m32, m33
        );
    }
    // DirectX::XMFLOAT4X4에서 로드
    inline Matrix4x4(const DirectX::XMFLOAT4X4& m) : m_mat(DirectX::XMLoadFloat4x4(&m)) {}
    inline Matrix4x4(const DirectX::XMVECTOR& r0, const DirectX::XMVECTOR& r1, const DirectX::XMVECTOR& r2, const DirectX::XMVECTOR& r3) : m_mat(DirectX::XMMATRIX(r0, r1, r2, r3)) {}
    inline Matrix4x4(const Vector4& r0, const Vector4& r1, const Vector4& r2, const Vector4& r3) : m_mat(DirectX::XMMATRIX(r0.ToXMVECTOR(), r1.ToXMVECTOR(), r2.ToXMVECTOR(), r3.ToXMVECTOR())) {}

    // DirectX::XMMATRIX에서 직접 초기화
    inline Matrix4x4(const DirectX::XMMATRIX& m) : m_mat(m) {}

    // 연산자재정의
    // 행렬 덧셈
    inline Matrix4x4 operator+(const Matrix4x4& other) const {
        return Matrix4x4(
            DirectX::XMVectorAdd(m_mat.r[0], other.m_mat.r[0]),
            DirectX::XMVectorAdd(m_mat.r[1], other.m_mat.r[1]),
            DirectX::XMVectorAdd(m_mat.r[2], other.m_mat.r[2]),
            DirectX::XMVectorAdd(m_mat.r[3], other.m_mat.r[3])
        );
    }
    inline Matrix4x4& operator+=(const Matrix4x4& other) {
        m_mat.r[0] = DirectX::XMVectorAdd(m_mat.r[0], other.m_mat.r[0]);
        m_mat.r[1] = DirectX::XMVectorAdd(m_mat.r[1], other.m_mat.r[1]);
        m_mat.r[2] = DirectX::XMVectorAdd(m_mat.r[2], other.m_mat.r[2]);
        m_mat.r[3] = DirectX::XMVectorAdd(m_mat.r[3], other.m_mat.r[3]);
        return *this;
    }
    // 행렬 뺄셈
    inline Matrix4x4 operator-(const Matrix4x4& other) const {
        return Matrix4x4(
            DirectX::XMVectorSubtract(m_mat.r[0], other.m_mat.r[0]),
            DirectX::XMVectorSubtract(m_mat.r[1], other.m_mat.r[1]),
            DirectX::XMVectorSubtract(m_mat.r[2], other.m_mat.r[2]),
            DirectX::XMVectorSubtract(m_mat.r[3], other.m_mat.r[3])
        );
    }
    inline Matrix4x4& operator-=(const Matrix4x4& other) {
        m_mat.r[0] = DirectX::XMVectorSubtract(m_mat.r[0], other.m_mat.r[0]);
        m_mat.r[1] = DirectX::XMVectorSubtract(m_mat.r[1], other.m_mat.r[1]);
        m_mat.r[2] = DirectX::XMVectorSubtract(m_mat.r[2], other.m_mat.r[2]);
        m_mat.r[3] = DirectX::XMVectorSubtract(m_mat.r[3], other.m_mat.r[3]);
        return *this;
    }
    // 행렬 곱셈
    inline Matrix4x4 operator*(const Matrix4x4& other) const { return Matrix4x4(DirectX::XMMatrixMultiply(m_mat, other.m_mat)); }
    inline Matrix4x4& operator*=(const Matrix4x4& other) { m_mat = DirectX::XMMatrixMultiply(m_mat, other.m_mat); return *this; }
    inline Matrix4x4 operator*(float scalar) const {
        return Matrix4x4(
            DirectX::XMVectorScale(m_mat.r[0], scalar),
            DirectX::XMVectorScale(m_mat.r[1], scalar),
            DirectX::XMVectorScale(m_mat.r[2], scalar),
            DirectX::XMVectorScale(m_mat.r[3], scalar)
        );
    }
    inline Matrix4x4& operator*=(float scalar) {
        m_mat.r[0] = DirectX::XMVectorScale(m_mat.r[0], scalar);
        m_mat.r[1] = DirectX::XMVectorScale(m_mat.r[1], scalar);
        m_mat.r[2] = DirectX::XMVectorScale(m_mat.r[2], scalar);
        m_mat.r[3] = DirectX::XMVectorScale(m_mat.r[3], scalar);
        return *this;
    }

    // 스칼라 나눗셈
    inline Matrix4x4 operator/(float scalar) const {
        float invScalar = 1.0f / scalar;
        return (*this) * invScalar;
    }
    inline Matrix4x4& operator/=(float scalar) {
        float invScalar = 1.0f / scalar;
        return (*this) *= invScalar;
    }
    //벡터*행렬 계산
    inline Vector4 operator*(const Vector4& vec) const {
        return Vector4(DirectX::XMVector4Transform(vec.ToXMVECTOR(), m_mat));
    }
    
    //행 접근 (읽기/쓰기 가능)
    inline Vector4& operator[](int idx) { return r[idx]; }
    inline const Vector4& operator[](int idx) const { return r[idx]; }

    //유틸함수들
    inline Matrix4x4 Transpose() const { return Matrix4x4(DirectX::XMMatrixTranspose(m_mat)); }
    inline float Determinant() const { return DirectX::XMVectorGetX(DirectX::XMMatrixDeterminant(m_mat)); }
    static inline Matrix4x4 Identity() { return Matrix4x4(DirectX::XMMatrixIdentity()); }
    inline DirectX::XMMATRIX ToXMMATRIX() const { return m_mat; }
    inline DirectX::XMFLOAT4X4 ToXMFLOAT4X4() const
    {
        DirectX::XMFLOAT4X4 result;
        DirectX::XMStoreFloat4x4(&result, m_mat);
        return result;
    }
    friend inline Vector3 operator*(const Vector3& v, const Matrix4x4& m);
    friend inline Vector4 operator*(const Vector4& v, const Matrix4x4& m);
private:
    union
    {
        DirectX::XMMATRIX m_mat;
        Vector4 r[4];
    };
};

// v * m 연산을 위한 함수
inline Vector3 operator*(const Vector3& v, const Matrix4x4& m)
{
    // 결과를 다시 Vector3 객체로 반환, w를 1로 처리하고 연산해준다
    return Vector3(DirectX::XMVector3Transform(v.ToXMVECTOR(), m.ToXMMATRIX()));
}
inline Vector4 operator*(const Vector4& v, const Matrix4x4& m)
{
    // DirectXMath의 XMVector4Transform은 Vector * Matrix 연산을 수행합니다.
    return Vector4(DirectX::XMVector4Transform(v.ToXMVECTOR(), m.ToXMMATRIX()));
}

// 스칼라 * 행렬 연산 (이전에 있었던 내용, 다시 한번 확인차 포함)
inline Matrix4x4 operator*(float scalar, const Matrix4x4& mat) {
    return mat * scalar;
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
* 이동행렬
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
* 월드행렬(모델링행렬)
* Ux*Sx		Uy*Sx		Uz*Sx		0.0f
* Vx*Sy		Vx*Sy		Vz*Sy		0.0f
* Wx*Sz		Wy*Sz		Wz*Sz		0.0f
* Tx		Ty			Tz			1.0f
*/

inline Matrix4x4 GetMat_WorldMatrix(const Vector3 scale, const Vector3 eulerDegrees, const Vector3 position)
{
	return GetMat_Scale(scale) * GetMat_RotRollPitchYaw(eulerDegrees) * GetMat_Translation(position);
}

/*
* 뷰행렬
* Ux		Vx			Wx			0.0f
* Uy		Vy			Wy			0.0f
* Uz		Vz			Wz			0.0f
* -QdotU	-QdotV		-QdotW		1.0f
*/

inline Matrix4x4 GetMat_ViewMatrix(const Vector3 posCamera, const Vector3 posTarget, const Vector3 upVector = Vector3(0.0f, 1.0f, 0.0f))
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
	Vector3 camPos = posCamera;
	mat[3] = Vector4(-DotProduct(u, camPos), -DotProduct(v, camPos), -DotProduct(w, camPos), 1.0f);
	return mat;
}

inline Matrix4x4 GetMat_ViewMatrix(const Vector3 posCamera, const float fPitch, const float fYaw, const float fRoll)
{
	Matrix4x4 matRotate = GetMat_RotRollPitchYaw({ fPitch, fYaw, fRoll }).Transpose();		//회전행렬의 전치행렬(역행렬)
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
inline Matrix4x4 GetMat_Ortho(float width, float height, float dist_near, float dist_far)
{
	Matrix4x4 mat;
	mat[0] = Vector4(2.0f / width, 0.0f, 0.0f, 0.0f);
	mat[1] = Vector4(0.0f, 2.0f / height, 0.0f, 0.0f);
	mat[2] = Vector4(0.0f, 0.0f, 1.0f / (dist_far - dist_near), 0.0f);
	mat[3] = Vector4(0.0f, 0.0f, -dist_near / (dist_far - dist_near), 1.0f);
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
* 여인수행렬 C(A)
* (-1)^(i+j) * cofMatA(ij) 로 행렬의 각 요소를 구성시킨다
*/
inline Matrix4x4 GetMat_Cofactor(const Matrix4x4& mat4x4)
{
	// 매트릭스 행을 Vector4로 변환 (각 성분 접근자 함수 사용)
	Vector4 r1(mat4x4[0].GetX(), mat4x4[0].GetY(), mat4x4[0].GetZ(), mat4x4[0].GetW());
	Vector4 r2(mat4x4[1].GetX(), mat4x4[1].GetY(), mat4x4[1].GetZ(), mat4x4[1].GetW());
	Vector4 r3(mat4x4[2].GetX(), mat4x4[2].GetY(), mat4x4[2].GetZ(), mat4x4[2].GetW());
	Vector4 r4(mat4x4[3].GetX(), mat4x4[3].GetY(), mat4x4[3].GetZ(), mat4x4[3].GetW());
	// 1행의 여인수(Cofactor) 계산 (Getters 사용)
	float m11 = (float)std::pow(-1, 1 + 1) * GetDeterminant3x3(r2.GetY(), r2.GetZ(), r2.GetW(), r3.GetY(), r3.GetZ(), r3.GetW(), r4.GetY(), r4.GetZ(), r4.GetW());
	float m12 = (float)std::pow(-1, 1 + 2) * GetDeterminant3x3(r2.GetX(), r2.GetZ(), r2.GetW(), r3.GetX(), r3.GetZ(), r3.GetW(), r4.GetX(), r4.GetZ(), r4.GetW());
	float m13 = (float)std::pow(-1, 1 + 3) * GetDeterminant3x3(r2.GetX(), r2.GetY(), r2.GetW(), r3.GetX(), r3.GetY(), r3.GetW(), r4.GetX(), r4.GetY(), r4.GetW());
	float m14 = (float)std::pow(-1, 1 + 4) * GetDeterminant3x3(r2.GetX(), r2.GetY(), r2.GetZ(), r3.GetX(), r3.GetY(), r3.GetZ(), r4.GetX(), r4.GetY(), r4.GetZ());

	// 2행의 여인수 계산 (Getters 사용)
	float m21 = (float)std::pow(-1, 2 + 1) * GetDeterminant3x3(r1.GetY(), r1.GetZ(), r1.GetW(), r3.GetY(), r3.GetZ(), r3.GetW(), r4.GetY(), r4.GetZ(), r4.GetW());
	float m22 = (float)std::pow(-1, 2 + 2) * GetDeterminant3x3(r1.GetX(), r1.GetZ(), r1.GetW(), r3.GetX(), r3.GetZ(), r3.GetW(), r4.GetX(), r4.GetZ(), r4.GetW());
	float m23 = (float)std::pow(-1, 2 + 3) * GetDeterminant3x3(r1.GetX(), r1.GetY(), r1.GetW(), r3.GetX(), r3.GetY(), r3.GetW(), r4.GetX(), r4.GetY(), r4.GetW());
	float m24 = (float)std::pow(-1, 2 + 4) * GetDeterminant3x3(r1.GetX(), r1.GetY(), r1.GetZ(), r3.GetX(), r3.GetY(), r3.GetZ(), r4.GetX(), r4.GetY(), r4.GetZ());

	// 3행의 여인수 계산 (Getters 사용)
	float m31 = (float)std::pow(-1, 3 + 1) * GetDeterminant3x3(r1.GetY(), r1.GetZ(), r1.GetW(), r2.GetY(), r2.GetZ(), r2.GetW(), r4.GetY(), r4.GetZ(), r4.GetW());
	float m32 = (float)std::pow(-1, 3 + 2) * GetDeterminant3x3(r1.GetX(), r1.GetZ(), r1.GetW(), r2.GetX(), r2.GetZ(), r2.GetW(), r4.GetX(), r4.GetZ(), r4.GetW());
	float m33 = (float)std::pow(-1, 3 + 3) * GetDeterminant3x3(r1.GetX(), r1.GetY(), r1.GetW(), r2.GetX(), r2.GetY(), r2.GetW(), r4.GetX(), r4.GetY(), r4.GetW());
	float m34 = (float)std::pow(-1, 3 + 4) * GetDeterminant3x3(r1.GetX(), r1.GetY(), r1.GetZ(), r2.GetX(), r2.GetY(), r2.GetZ(), r4.GetX(), r4.GetY(), r4.GetZ());

	// 4행의 여인수 계산 (Getters 사용)
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
* 역행렬
* AdjMat(A*) = C(A)^T , 여인수행렬의 전치를 의미, 수반행렬
* InvMat = AdjMat * 1.0f/detA
*/
inline Matrix4x4 GetMat_Inverse(const Matrix4x4& matOrigin)
{
#ifndef __OPTIMIZED
	//수반행렬(여인수행렬의 전치행렬)
	Matrix4x4 matAdj = GetMat_Cofactor(matOrigin).Transpose();
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