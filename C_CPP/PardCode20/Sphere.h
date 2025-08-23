#pragma once
#include "Collider.h"

class Sphere : public Collider
{
public:
	inline Sphere() = default;
	inline Sphere(float radius, Vector3 center = Vector3(0.0f, 0.0f, 0.0f)) : m_vec(center, radius) {}
	//��� ������ ����ؼ� ���� �� �������� ����Ѵ�(��������� ���� �� �� �������� �������� ��´�)
	inline Sphere(const std::vector<Vector3>* vertices)
	{
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
	inline Vector3 GetCenter() const { return m_vec.ToVector3(); }
	inline float GetRadius() const { return m_vec.GetW(); }
private:
	Vector4 m_vec;	//Center, Radius
};