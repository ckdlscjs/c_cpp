#pragma once
#include "std.h"

__declspec(align(16))
struct Vector2
{
public:
	// 생성자 (복사/이동 생성자는 컴파일러의 자동 생성에 맡긴다)
	inline Vector2() : m_vec(DirectX::XMVectorZero()) {} // (0,0,0,0)으로 초기화
	inline Vector2(float x, float y) : m_vec(DirectX::XMVectorSet(x, y, 0.0f, 0.0f)) {} // Vector2의 경우 z와 w를 0으로 한다
	inline Vector2(const DirectX::XMFLOAT2& v) : Vector2(v.x, v.y) {} // XMLoadFloat2는 z,w를 0으로 설정
	inline Vector2(const DirectX::XMVECTOR& v) : m_vec(v) {} // XMVECTOR를 직접 받을 때

	// 성분별 접근
	inline float GetX() const { return DirectX::XMVectorGetX(m_vec); }
	inline float GetY() const { return DirectX::XMVectorGetY(m_vec); }
	inline void Set(float x = 0, float y = 0) { SetX(x); SetY(y); }
	inline void SetX(float x) { m_vec = DirectX::XMVectorSetX(m_vec, x); }
	inline void SetY(float y) { m_vec = DirectX::XMVectorSetY(m_vec, y); }

	// 연산자 오버로드
	inline Vector2 operator+(const Vector2& other) const { return Vector2(DirectX::XMVectorAdd(m_vec, other.m_vec)); }
	inline Vector2& operator+=(const Vector2& other)
	{
		m_vec = DirectX::XMVectorAdd(m_vec, other.m_vec);
		return *this;
	}
	inline Vector2 operator-() const { return Vector2(-GetX(), -GetY()); }
	inline Vector2 operator-(const Vector2& other) const { return Vector2(DirectX::XMVectorSubtract(m_vec, other.m_vec)); }
	inline Vector2& operator-=(const Vector2& other)
	{
		m_vec = DirectX::XMVectorSubtract(m_vec, other.m_vec);
		return *this;
	}
	// 성분별 곱셈
	inline Vector2 operator*(const Vector2& other) const { return Vector2(DirectX::XMVectorMultiply(m_vec, other.m_vec)); }
	inline Vector2& operator*=(const Vector2& other)
	{
		m_vec = DirectX::XMVectorMultiply(m_vec, other.m_vec);
		return *this;
	}
	inline Vector2 operator*(float scalar) const { return Vector2(DirectX::XMVectorScale(m_vec, scalar)); }
	inline Vector2& operator*=(float scalar)
	{
		m_vec = DirectX::XMVectorScale(m_vec, scalar);
		return *this;
	}
	inline Vector2 operator/(float scalar) const { return Vector2(DirectX::XMVectorScale(m_vec, 1.0f / scalar)); }
	inline Vector2& operator/=(float scalar)
	{
		m_vec = DirectX::XMVectorScale(m_vec, 1.0f / scalar);
		return *this;
	}
	inline bool operator==(const Vector2& other) const { return DirectX::XMVector2NearEqual(m_vec, other.m_vec, DirectX::XMVectorSplatEpsilon()); }
	inline bool operator!=(const Vector2& other) const { return !(*this == other); }

	// 유틸리티 함수
	inline float Length() const { return DirectX::XMVectorGetX(DirectX::XMVector2Length(m_vec)); }
	inline float LengthSquared() const { return DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(m_vec)); }
	inline Vector2 Normalize() const { return Vector2(DirectX::XMVector2Normalize(m_vec)); }
	inline Vector2 ABS() const { return Vector2(DirectX::XMVectorAbs(m_vec)); }
	inline float DotProduct(const Vector2& other) const { return DirectX::XMVectorGetX(DirectX::XMVector2Dot(m_vec, other.m_vec)); }
	// 2D 벡터의 외적은 스칼라 값을 반환합니다 (실제로 2D 평면에 수직인 3D 벡터의 Z 성분)
	inline float CrossProduct(const Vector2& other) const {
		return DirectX::XMVectorGetZ(DirectX::XMVector2Cross(m_vec, other.m_vec));
	}


	// 외부 연산자/유틸리티를 위한 friend 함수
	friend inline float DotProduct(const Vector2& v1, const Vector2& v2);
	friend inline float CrossProduct(const Vector2& v1, const Vector2& v2); // 2D Cross는 스칼라를 반환합니다.
	friend inline Vector2 operator*(float scalar, const Vector2& vec);

	// 변환 함수
	inline DirectX::XMVECTOR ToXMVECTOR() const { return m_vec; }
	inline DirectX::XMFLOAT2 ToXMFLOAT2() const
	{
		DirectX::XMFLOAT2 result;
		DirectX::XMStoreFloat2(&result, m_vec);
		return result;
	}

	//배열첨자재정의
	inline float& operator[](size_t i) {
		_ASEERTION_NULCHK(i < 2, "Out Of Index");
		return reinterpret_cast<float*>(&m_vec)[i];
	}

private:
		DirectX::XMVECTOR m_vec;

};

// 닷 프로덕트 (내적)
inline float DotProduct(const Vector2& v1, const Vector2& v2) { return DirectX::XMVectorGetX(DirectX::XMVector2Dot(v1.m_vec, v2.m_vec)); }

// 2D 외적 (스칼라 값 반환)
inline float CrossProduct(const Vector2& v1, const Vector2& v2) { return DirectX::XMVectorGetZ(DirectX::XMVector2Cross(v1.m_vec, v2.m_vec)); }

// 외부 스칼라 곱셈 (float * Vector2)
inline Vector2 operator*(float scalar, const Vector2& vec) { return vec * scalar; }
inline Vector2 operator/(float scalar, const Vector2& vec) { return vec / scalar; }