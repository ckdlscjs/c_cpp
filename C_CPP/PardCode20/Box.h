#pragma once
#include "Collider.h"

class Box : public Collider
{
public:
	inline Box() = default;
	inline Box(const std::vector<Vector3>* vertices, const Vector3& scale = Vector3(1.0f, 1.0f, 1.0f), const Vector3& degrees = Vector3(0, 0, 0), const Vector3& position = Vector3(0, 0, 0) )
	{
		if (vertices->size() <= 0) return;
		float minX, minY, minZ, maxX, maxY, maxZ;
		minX = minY = minZ = FLT_MAX;
		maxX = maxY = maxZ = FLT_MIN;
		for (auto iter = vertices->begin(); iter != vertices->end(); iter++)
		{
			minX = min(minX, iter->GetX());
			minY = min(minY, iter->GetY());
			minZ = min(minZ, iter->GetZ());
			maxX = max(maxX, iter->GetX());
			maxY = max(maxY, iter->GetY());
			maxZ = max(maxZ, iter->GetZ());
		}
		m_vMin.Set(minX, minY, minZ);
		m_vMax.Set(maxX, maxY, maxZ);
		m_matWorld = GetMat_WorldMatrix(scale, degrees, position);
	}
	
private:
	Vector3 m_vMin;
	Vector3 m_vMax;
	Matrix4x4 m_matWorld;
};