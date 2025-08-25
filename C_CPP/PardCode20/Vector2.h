#pragma once
#include <DirectXMath.h>

__declspec(align(16))
struct Vector2
{
public:
	// ������ (����/�̵� �����ڴ� �����Ϸ��� �ڵ� ������ �ñ��)
	inline Vector2() : m_vec(DirectX::XMVectorZero()) {} // (0,0,0,0)���� �ʱ�ȭ
	inline Vector2(float x, float y) : m_vec(DirectX::XMVectorSet(x, y, 0.0f, 0.0f)) {} // Vector2�� ��� z�� w�� 0���� �Ѵ�
	inline Vector2(const DirectX::XMFLOAT2& v) : Vector2(v.x, v.y) {} // XMLoadFloat2�� z,w�� 0���� ����
	inline Vector2(const DirectX::XMVECTOR& v) : m_vec(v) {} // XMVECTOR�� ���� ���� ��

	// ���к� ����
	inline float GetX() const { return DirectX::XMVectorGetX(m_vec); }
	inline float GetY() const { return DirectX::XMVectorGetY(m_vec); }
	inline void Set(float x = 0, float y = 0) { SetX(x); SetY(y); }
	inline void SetX(float x) { m_vec = DirectX::XMVectorSetX(m_vec, x); }
	inline void SetY(float y) { m_vec = DirectX::XMVectorSetY(m_vec, y); }

	// ������ �����ε�
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
	// ���к� ����
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

	// ��ƿ��Ƽ �Լ�
	inline float Length() const { return DirectX::XMVectorGetX(DirectX::XMVector2Length(m_vec)); }
	inline float LengthSquared() const { return DirectX::XMVectorGetX(DirectX::XMVector2LengthSq(m_vec)); }
	inline Vector2 Normalize() const { return Vector2(DirectX::XMVector2Normalize(m_vec)); }
	inline Vector2 ABS() const { return Vector2(DirectX::XMVectorAbs(m_vec)); }
	inline float DotProduct(const Vector2& other) const { return DirectX::XMVectorGetX(DirectX::XMVector2Dot(m_vec, other.m_vec)); }
	// 2D ������ ������ ��Į�� ���� ��ȯ�մϴ� (������ 2D ��鿡 ������ 3D ������ Z ����)
	inline float CrossProduct(const Vector2& other) const {
		return DirectX::XMVectorGetZ(DirectX::XMVector2Cross(m_vec, other.m_vec));
	}


	// �ܺ� ������/��ƿ��Ƽ�� ���� friend �Լ�
	friend inline float DotProduct(const Vector2& v1, const Vector2& v2);
	friend inline float CrossProduct(const Vector2& v1, const Vector2& v2); // 2D Cross�� ��Į�� ��ȯ�մϴ�.
	friend inline Vector2 operator*(float scalar, const Vector2& vec);

	// ��ȯ �Լ�
	inline DirectX::XMVECTOR ToXMVECTOR() const { return m_vec; }
	inline DirectX::XMFLOAT2 ToXMFLOAT2() const
	{
		DirectX::XMFLOAT2 result;
		DirectX::XMStoreFloat2(&result, m_vec);
		return result;
	}

private:
		DirectX::XMVECTOR m_vec;

};

// �� ���δ�Ʈ (����)
inline float DotProduct(const Vector2& v1, const Vector2& v2) { return DirectX::XMVectorGetX(DirectX::XMVector2Dot(v1.m_vec, v2.m_vec)); }

// 2D ���� (��Į�� �� ��ȯ)
inline float CrossProduct(const Vector2& v1, const Vector2& v2) { return DirectX::XMVectorGetZ(DirectX::XMVector2Cross(v1.m_vec, v2.m_vec)); }

// �ܺ� ��Į�� ���� (float * Vector2)
inline Vector2 operator*(float scalar, const Vector2& vec) { return vec * scalar; }