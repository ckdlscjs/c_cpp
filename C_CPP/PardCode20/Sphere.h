#pragma once
#include "Collider.h"

class Sphere : public Collider
{
public:
	inline Sphere() = default;
	inline Sphere(float radius, Vector3 center = Vector3(0.0f, 0.0f, 0.0f)) : m_vec(center, radius) {}
	//모든 정점을 계산해서 중점 및 반지름을 계산한다(중점계산후 가장 먼 점 기준으로 반지름을 잡는다)
	inline Sphere(const std::vector<Vector3>* poss)
	{
		if (poss->size() <= 0) return;
		Vector3 sum;
		for (auto iter = poss->begin(); iter != poss->end(); iter++)
			sum += *iter;
		Vector3 center = sum / (float)poss->size();
		Vector3 farthestPoint = (*std::max_element(poss->begin(), poss->end(),
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