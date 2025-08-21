#pragma once
#include "Collider.h"

__declspec(align(16))
class Plane : public Collider
{
public:
	/*
	* ax + by + cz + d = 0;
	* (a, b, c) normal, d(-(ax0 + by0 +cz0))
	* n dot p + d = 0
	*/
	inline Plane() : m_vec(0.0f, 0.0f, 0.0f, 0.0f) {}
	inline Plane(Vector3 n, float d)															
	{
		_ASEERTION_NULCHK(n != Vector3(0, 0, 0), "normal is zero");
		float nlength = n.Length();
		m_vec.Set(n.GetX() / nlength, n.GetY() / nlength, n.GetZ() / nlength, d / nlength);
	}
	inline Plane(const Vector3& n, const Vector3& p) : Plane(n, DotProduct(n, p)) {}
	inline Plane(float a, float b, float c, float d) : Plane(Vector3(a, b, c), d) {}
	inline Plane(const Vector4& v) : Plane(v.ToVector3(), v.GetW()) {}
	inline Plane(const Vector3& p0, const Vector3& p1, const Vector3& p2)
	{
		Vector3 v0 = p1 - p0;
		Vector3 v1 = p2 - p0;
		Vector3 n = CrossProduct(v0, v1);
		_ASEERTION_NULCHK(n != Vector3(0, 0, 0), "normal is zero");
		n = n.Normalize();
		float d = -DotProduct(n, p0);
		m_vec.Set(n.GetX(), n.GetY(), n.GetZ(), d);
	}
	inline Vector3 GetNormal() const
	{
		return m_vec.ToVector3();
	}
	inline float GetDistance() const
	{
		return m_vec.GetW();
	}
	inline float Distance(const Vector3& p) const
	{
		return DotProduct(GetNormal(), p) + GetDistance();
	}
	inline bool IsOutside(const Vector3& p) const
	{
		float result = DotProduct(GetNormal(), p) + GetDistance();
		return result < 0.0f;
	}
	
private:
	Vector4 m_vec;
};