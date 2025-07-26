#pragma once
#include "Vector3.h"
#include "Vector4.h"

struct Position
{
public:
	//생성자(복사/이동 생성자는 컴파일러의 자동생성에 맡긴다)
	inline Position() : m_vec(DirectX::XMVectorZero()) {}
	inline Position(float x, float y, float z) : m_vec(x, y, z, 1.0f) {}
	inline Position(const Vector3& v) : m_vec(v, 1.0f) {}

	inline float GetX() const { return m_vec.GetX(); }
	inline float GetY() const { return m_vec.GetY(); }
	inline float GetZ() const { return m_vec.GetZ(); }
	inline void SetX(float x)  { return m_vec.SetX(x); }
	inline void SetY(float y)  { return m_vec.SetY(y); }
	inline void SetZ(float z)  { return m_vec.SetZ(z); }

	inline Position& operator+=(const Vector3& other)
	{
		m_vec += Vector4(other);
		return *this;
	}
	inline Position operator+(const Vector3& other)
	{
		Position ret = *this;
		ret += other;
		return ret;
	}
	inline Position& operator-=(const Vector3& other)
	{
		m_vec -= Vector4(other);
		return *this;
	}
	inline Position operator-(const Vector3& other)
	{
		Position ret = *this;
		ret -= other;
		return ret;
	}
	inline Vector3 operator-(const Position& other)
	{
		return (m_vec - other.m_vec).ToVector3();
	}

	inline Position& operator*=(float scalar)
	{
		m_vec *= scalar;
		return *this;
	}
	inline Position operator*(float scalar)
	{
		Position ret = *this;
		ret *= scalar;
		return ret;
	}
	inline Vector3 ToVector3() const { return m_vec.ToVector3(); }
	friend inline Vector3 operator-(const Position& p1, const Position& p2);
private:
	Vector4 m_vec;
};
inline Vector3 operator-(const Position& p1, const Position& p2)
{
	return (p1.m_vec - p2.m_vec).ToVector3();
}