#pragma once
#include "Vector3.h"
__declspec(align(16))
struct Vector4
{
public:
	//생성자(복사/이동 생성자는 컴파일러의 자동생성에 맡긴다)
	inline Vector4() : m_vec(DirectX::XMVectorZero()) {} 
	inline Vector4(float x , float y , float z , float w) : m_vec(DirectX::XMVectorSet(x, y, z, w)) {}
	inline Vector4(const Vector3& v, float w = 0.0f) : Vector4(v.GetX(), v.GetY(), v.GetZ(), w) {}
	inline Vector4(const DirectX::XMFLOAT4& v) : m_vec(DirectX::XMLoadFloat4(&v)) {}
	inline Vector4(const DirectX::XMVECTOR& v) : m_vec(v) {}

	//성분별 접근
	inline float GetX() const { return DirectX::XMVectorGetX(m_vec); }
	inline float GetY() const { return DirectX::XMVectorGetY(m_vec); }
	inline float GetZ() const { return DirectX::XMVectorGetZ(m_vec); }
	inline float GetW() const { return DirectX::XMVectorGetW(m_vec); }
	inline void SetX(float x) { m_vec = DirectX::XMVectorSetX(m_vec, x); }
	inline void SetY(float y) { m_vec = DirectX::XMVectorSetY(m_vec, y); }
	inline void SetZ(float z) { m_vec = DirectX::XMVectorSetZ(m_vec, z); }
	inline void SetW(float w) { m_vec = DirectX::XMVectorSetW(m_vec, w); }

	//연산자재정의
	inline Vector4 operator+(const Vector4& other) const { return Vector4(DirectX::XMVectorAdd(m_vec, other.m_vec)); }
	inline Vector4& operator+=(const Vector4& other)
	{
		m_vec = DirectX::XMVectorAdd(m_vec, other.m_vec);
		return *this;
	}
	inline Vector4 operator-(const Vector4& other) const { return Vector4(DirectX::XMVectorSubtract(m_vec, other.m_vec)); }
	inline Vector4& operator-=(const Vector4& other)
	{
		m_vec = DirectX::XMVectorSubtract(m_vec, other.m_vec);
		return *this;
	}
	inline Vector4 operator*(const Vector4& other) const { return Vector4(DirectX::XMVectorMultiply(m_vec, other.m_vec)); }
	inline Vector4& operator*=(const Vector4& other)
	{
		m_vec = DirectX::XMVectorMultiply(m_vec, other.m_vec);
		return *this;
	}
	inline Vector4 operator*(float scalar) const { return Vector4(DirectX::XMVectorScale(m_vec, scalar)); }
	inline Vector4& operator*=(float scalar)
	{
		m_vec = DirectX::XMVectorScale(m_vec, scalar);
		return *this;
	}
	inline Vector4 operator/(float scalar) const { return Vector4(DirectX::XMVectorScale(m_vec, 1.0f / scalar)); }
	inline Vector4& operator/=(float scalar)
	{
		m_vec = DirectX::XMVectorScale(m_vec, 1.0f / scalar);
		return *this;
	}
	inline bool operator==(const Vector4& other) { return DirectX::XMVector4NearEqual(m_vec, other.m_vec, DirectX::XMVectorSplatEpsilon()); }

	//유틸함수들
	inline float Length() const { return DirectX::XMVectorGetX(DirectX::XMVector4Length(m_vec)); }
	inline float LengthSquared() const { return DirectX::XMVectorGetX(DirectX::XMVector4LengthSq(m_vec)); }
	inline Vector4 Normalize() const { return Vector4(DirectX::XMVector4Normalize(m_vec)); }
	inline float DotProduct(const Vector4& other) const { return DirectX::XMVectorGetX(DirectX::XMVector4Dot(m_vec, other.m_vec)); }
	friend inline float DotProduct(const Vector4& v1, const Vector4& v2);

	//변환함수들
	inline DirectX::XMVECTOR ToXMVECTOR() const { return m_vec; }
	inline DirectX::XMFLOAT4 ToXMFLOAT4() const
	{
		DirectX::XMFLOAT4 result;
		DirectX::XMStoreFloat4(&result, m_vec);
		return result;
	}
	inline Vector3 ToVector3() const { return Vector3(GetX(), GetY(), GetZ()); }
	inline Vector2 ToVector2() const { return Vector2(GetX(), GetY()); }
private:
	DirectX::XMVECTOR m_vec;
};

// 닷 프로덕트 (내적)
inline float DotProduct(const Vector4& v1, const Vector4& v2)
{
	return DirectX::XMVectorGetX(DirectX::XMVector4Dot(v1.m_vec, v2.m_vec));
}
inline Vector4 operator*(float scalar, const Vector4& vec) { return vec * scalar; }