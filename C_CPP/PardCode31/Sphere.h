#pragma once
#include "Collider.h"

class Sphere : public Collider
{
public:
	inline Sphere() = default;
	inline ~Sphere() { std::cout << "Release : " << "Collider <" << "Sphere" << "> Class" << '\n'; }
	inline Sphere(float radius, Vector3 center = Vector3(0.0f, 0.0f, 0.0f)) : m_vec(center, radius) { SetType(E_Colliders::SPHERE); }
	//��� ������ ����ؼ� ���� �� �������� ����Ѵ�(��������� ���� �� �� �������� �������� ��´�)
	inline Sphere(const std::vector<Vector3>* vertices)
	{
		SetType(E_Colliders::SPHERE);
		if (vertices->size() <= 0) return;
		Vector3 sum;
		for (auto iter = vertices->begin(); iter != vertices->end(); iter++)
			sum += *iter;
		Vector3 center = sum / (float)vertices->size();
		Vector3 farthestPoint = (*std::max_element(vertices->begin(), vertices->end(),
			[&](Vector3 const& lhs, Vector3 const& rhs)->bool
			{
				return (center - lhs).LengthSquared() < (center - rhs).LengthSquared();
			}));
		float radius = (farthestPoint - center).Length();
		m_vec.Set(center.GetX(), center.GetY(), center.GetZ(), radius);
	}
	inline void GetWorldSphere(const Matrix4x4& matWorld, Vector3& center, float& radius) const
	{
		center = m_vec.ToVector3() * matWorld;
		float maxScale = std::max({ matWorld[0].GetX(), matWorld[1].GetY(), matWorld[2].GetZ() });
		radius = m_vec.GetW() * maxScale;
	}
private:
	Vector4 m_vec;	//Center, Radius
};