#pragma once
#include "Vector2.h"
__declspec(align(16))
struct Vector3
{
public:
	//생성자(복사/이동 생성자는 컴파일러의 자동생성에 맡긴다)
	inline Vector3() : m_vec(DirectX::XMVectorZero()) {}
	inline Vector3(float x , float y , float z) : m_vec(DirectX::XMVectorSet(x, y, z, 0.0f)) {} //vector3의 경우 w를 0으로한다
	inline Vector3(const DirectX::XMFLOAT3& v) : Vector3(v.x, v.y, v.z) {}
	inline Vector3(const DirectX::XMVECTOR& v) : m_vec(v) {}

	inline Vector3(const Vector2& v, float z = 0.0f) : Vector3( v.GetX(), v.GetY(), z) {}

	// 성분별 접근
	inline float GetX() const { return DirectX::XMVectorGetX(m_vec); }
	inline float GetY() const { return DirectX::XMVectorGetY(m_vec); }
	inline float GetZ() const { return DirectX::XMVectorGetZ(m_vec); }
	inline void Set(float x = 0, float y = 0, float z = 0) { SetX(x); SetY(y); SetZ(z); }
	inline void SetX(float x) { m_vec = DirectX::XMVectorSetX(m_vec, x); }
	inline void SetY(float y) { m_vec = DirectX::XMVectorSetY(m_vec, y); }
	inline void SetZ(float z) { m_vec = DirectX::XMVectorSetZ(m_vec, z); }

	// 연산자 오버로드
	inline Vector3 operator+(const Vector3& other) const { return Vector3(DirectX::XMVectorAdd(m_vec, other.m_vec)); }
	inline Vector3& operator+=(const Vector3& other)
	{
		m_vec = DirectX::XMVectorAdd(m_vec, other.m_vec);
		return *this;
	}
	inline Vector3 operator-() const { return Vector3(-GetX(), -GetY(), -GetZ()); }
	inline Vector3 operator-(const Vector3& other) const { return Vector3(DirectX::XMVectorSubtract(m_vec, other.m_vec)); }
	inline Vector3& operator-=(const Vector3& other)
	{
		m_vec = DirectX::XMVectorSubtract(m_vec, other.m_vec);
		return *this;
	}
	// 성분별 곱셈
	inline Vector3 operator*(const Vector3& other) const { return Vector3(DirectX::XMVectorMultiply(m_vec, other.m_vec)); }
	inline Vector3& operator*=(const Vector3& other)
	{
		m_vec = DirectX::XMVectorMultiply(m_vec, other.m_vec);
		return *this;
	}
	inline Vector3 operator*(float scalar) const { return Vector3(DirectX::XMVectorScale(m_vec, scalar)); }
	inline Vector3& operator*=(float scalar)
	{
		m_vec = DirectX::XMVectorScale(m_vec, scalar);
		return *this;
	}
	inline Vector3 operator/(float scalar) const { return Vector3(DirectX::XMVectorScale(m_vec, 1.0f / scalar)); }
	inline Vector3& operator/=(float scalar)
	{
		m_vec = DirectX::XMVectorScale(m_vec, 1.0f / scalar);
		return *this;
	}
	inline bool operator==(const Vector3& other) const { return DirectX::XMVector3NearEqual(m_vec, other.m_vec, DirectX::XMVectorSplatEpsilon()); }
	inline bool operator!=(const Vector3& other) const { return !(*this == other); }

	// 유틸리티 함수
	inline float Length() const { return DirectX::XMVectorGetX(DirectX::XMVector3Length(m_vec)); }
	inline float LengthSquared() const { return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(m_vec)); }
	inline Vector3 Normalize() const { return Vector3(DirectX::XMVector3Normalize(m_vec)); }
	inline Vector3 ABS() const { return Vector3(DirectX::XMVectorAbs(m_vec)); }
	inline float DotProduct(const Vector3& other) const { return DirectX::XMVectorGetX(DirectX::XMVector3Dot(m_vec, other.m_vec)); }
	inline Vector3 CrossProduct(const Vector3& other) const { return Vector3(DirectX::XMVector3Cross(m_vec, other.m_vec)); }

	// 외부 연산자/유틸리티를 위한 friend 함수
	friend inline float DotProduct(const Vector3& v1, const Vector3& v2);
	friend inline Vector3 CrossProduct(const Vector3& v1, const Vector3& v2);
	friend inline Vector3 operator*(float scalar, const Vector3& vec);

	// 변환 함수
	inline DirectX::XMVECTOR ToXMVECTOR() const { return m_vec; }
	inline DirectX::XMFLOAT3 ToXMFLOAT3() const
	{
		DirectX::XMFLOAT3 result;
		DirectX::XMStoreFloat3(&result, m_vec);
		return result;
	}
	inline Vector2 ToVector2() const { return Vector2(GetX(), GetY()); }

private:
	DirectX::XMVECTOR m_vec;
};

inline float DotProduct(const Vector3& v1, const Vector3& v2) { return DirectX::XMVectorGetX(DirectX::XMVector3Dot(v1.m_vec, v2.m_vec)); }
inline Vector3 CrossProduct(const Vector3& v1, const Vector3& v2) { return Vector3(DirectX::XMVector3Cross(v1.m_vec, v2.m_vec)); }
inline Vector3 operator*(float scalar, const Vector3& vec) { return vec * scalar; }