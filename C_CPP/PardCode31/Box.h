#pragma once
#include "Collider.h"
/*
// Box.h
#pragma once
#include <array>
#include <memory>
#include "Collider.h"
#include "Matrix4x4.h"

// ���� AABB�� ���� ����� ���� ���� Box Ŭ����
class Box : public Collider
{
public:
    // ���� min/max�� ���� ��ȯ ��ķ� �ڽ� �ʱ�ȭ
    inline Box(const Vector3& localMin, const Vector3& localMax, const Matrix4x4& worldMatrix)
        : m_localMin(localMin), m_localMax(localMax), m_worldMatrix(worldMatrix) {}

    // Collider::GetType() ����
    // Box�� OBB�� AABB�� ��� ǥ���ϹǷ�, OBB Ÿ������ �����մϴ�.
    // AABB�� OBB�� Ư���� ���(ȸ���� 0)�� ������ �� �ֽ��ϴ�.
    Type GetType() const override { return Type::OBB; }

    // �ٸ� Collider���� ���� ���� ����
    bool Intersects(const Collider& other) const override;

    // Box vs Box �浹 ���� ����
    bool IntersectsBox(const Box& other) const;

    // ���� Box�� ���� ��İ� ���� ũ�⸦ ������� ���� AABB�� ���
    void GetWorldAABB(Vector3& min, Vector3& max) const;

    // ���� ��İ� ���� min/max�� ������� OBB ���� (�߽�, ��, ũ��)�� ���
    void GetWorldOBB(Vector3& center, std::array<Vector3, 3>& axes, Vector3& extents) const;

private:
    Vector3 m_localMin;
    Vector3 m_localMax;
    Matrix4x4 m_worldMatrix;
};

// Box.cpp
#include "Box.h"
#include "SphereCollider.h" // SphereCollider�� �浹 ������ ���� ����
#include <iostream>

// Box�� �ٸ� Collider�� ���� ����
bool Box::Intersects(const Collider& other) const {
    // ��Ÿ�ӿ� ������ Ÿ���� Ȯ��
    switch (other.GetType()) {
        case Collider::Type::OBB: {
            // �ٸ� OBB �浹ü���� ����
            const Box& otherBox = static_cast<const Box&>(other);
            return IntersectsBox(otherBox);
        }
        case Collider::Type::SPHERE: {
            // Box�� Sphere�� �浹 ����
            const SphereCollider& sphere = static_cast<const SphereCollider&>(other);
            // �� ������ Sphere vs Box �浹 �˰����� ����ؾ� �մϴ�.
            // ���⼭�� ���������θ� ǥ���մϴ�.
            std::cout << "Box-Sphere �浹 ���� ���� (������)\n";
            return false;
        }
        default:
            return false;
    }
}


// OBB-OBB �浹 ������ ���� �и� �� ����(SAT) �Լ�
// �� �Լ��� �� OBB�� �����ϴ��� �����մϴ�.
static bool CheckIntersectionSAT(
    const Vector3& center1, const std::array<Vector3, 3>& axes1, const Vector3& extents1,
    const Vector3& center2, const std::array<Vector3, 3>& axes2, const Vector3& extents2)
{
    // �浹�� ������ �����ϱ� ���� 15���� �и� ���� �����մϴ�.
    // 1. �� �ڽ��� ���� �� (�� 6��)
    // 2. �� �ڽ��� ���� ���� ���� (�� 9��)

    // 1�ܰ�: ù ��° �ڽ��� ���� ���� ������ ���
    for (int i = 0; i < 3; ++i) {
        const Vector3& axis = axes1[i];
        float proj1 = DotProduct(center1 - center2, axis);
        float r1 = extents1[i];
        float r2 = 0.0f;
        for (int j = 0; j < 3; ++j) {
            r2 += otherExtents[j] * std::abs(DotProduct(axes2[j], axis));
        }
        if (std::abs(proj1) > r1 + r2) return false;
    }

    // 2�ܰ�: �� ��° �ڽ��� ���� ���� ������ ���
    for (int i = 0; i < 3; ++i) {
        const Vector3& axis = axes2[i];
        float proj1 = DotProduct(center1 - center2, axis);
        float r1 = 0.0f;
        for (int j = 0; j < 3; ++j) {
            r1 += extents1[j] * std::abs(DotProduct(axes1[j], axis));
        }
        float r2 = extents2[i];
        if (std::abs(proj1) > r1 + r2) return false;
    }

    // 3�ܰ�: �� �ڽ��� ���� ����(����)�Ͽ� ���� ������ ���
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            Vector3 axis = CrossProduct(axes1[i], axes2[j]);
            if (axis.LengthSquared() < 0.00001f) continue; // ���� ������ ��� �ǳʶݴϴ�.

            float proj1 = DotProduct(center1 - center2, axis);
            float r1 = 0.0f;
            for (int k = 0; k < 3; ++k) {
                r1 += extents1[k] * std::abs(DotProduct(axes1[k], axis));
            }
            float r2 = 0.0f;
            for (int k = 0; k < 3; ++k) {
                r2 += extents2[k] * std::abs(DotProduct(axes2[k], axis));
            }
            if (std::abs(proj1) > r1 + r2) return false;
        }
    }

    return true; // ��� �࿡ ���� ��ġ�Ƿ� �浹
}

// Box vs Box �浹 ����
bool Box::IntersectsBox(const Box& other) const {
    // ȸ���� ������ AABB�� ����, ������ OBB�� �����ϴ� ����
    // Matrix4x4 Ŭ������ ȸ�� ���θ� Ȯ���ϴ� �Լ��� �߰��Ͽ� �Ǵ��� �� �ֽ��ϴ�.
    bool thisIsAABB = !m_worldMatrix.IsRotated();
    bool otherIsAABB = !other.m_worldMatrix.IsRotated();

    if (thisIsAABB && otherIsAABB) {
        // AABB vs AABB �浹 ����
        Vector3 myMin, myMax, otherMin, otherMax;
        this->GetWorldAABB(myMin, myMax);
        other.GetWorldAABB(otherMin, otherMax);
        return (myMin.x <= otherMax.x && myMax.x >= otherMin.x) &&
               (myMin.y <= otherMax.y && myMax.y >= otherMin.y) &&
               (myMin.z <= otherMax.z && myMax.z >= otherMin.z);
    } else {
        // OBB vs OBB �浹 ���� (�����δ� �и� �� ���� SAT�� ���)
        Vector3 myCenter, myExtents;
        std::array<Vector3, 3> myAxes;
        this->GetWorldOBB(myCenter, myAxes, myExtents);

        Vector3 otherCenter, otherExtents;
        std::array<Vector3, 3> otherAxes;
        other.GetWorldOBB(otherCenter, otherAxes, otherExtents);

        return CheckIntersectionSAT(myCenter, myAxes, myExtents, otherCenter, otherAxes, otherExtents);
    }
}

*/
class Box : public Collider
{
public:
	inline Box() = default;
    inline ~Box() { std::cout << "Release : " << "Collider <" << "Box" << "> Class" << '\n'; }
	inline Box(const std::vector<Vector3>* vertices)
	{
        SetType(E_Collider::AABB);
		if (vertices->size() <= 0) return;
		float minX, minY, minZ, maxX, maxY, maxZ;
		minX = minY = minZ = FLT_MAX;
		maxX = maxY = maxZ = FLT_MIN;
		for (auto iter = vertices->begin(); iter != vertices->end(); iter++)
		{
			minX = std::min(minX, iter->GetX()); maxX = std::max(maxX, iter->GetX());
			minY = std::min(minY, iter->GetY()); maxY = std::max(maxY, iter->GetY());
			minZ = std::min(minZ, iter->GetZ()); maxZ = std::max(maxZ, iter->GetZ());
		}
		m_vMin.Set(minX, minY, minZ);
		m_vMax.Set(maxX, maxY, maxZ);
	}

    inline void GetWorldCorners(const Matrix4x4& matWorld, std::array<Vector3, 8>& corners) const
    {
        corners =
        {
             m_vMin * matWorld,
             Vector3(m_vMin.GetX(), m_vMin.GetY(), m_vMax.GetZ()) * matWorld,
             Vector3(m_vMin.GetX(), m_vMax.GetY(), m_vMin.GetZ()) * matWorld,
             Vector3(m_vMin.GetX(), m_vMax.GetY(), m_vMax.GetZ()) * matWorld,
             Vector3(m_vMax.GetX(), m_vMin.GetY(), m_vMin.GetZ()) * matWorld,
             Vector3(m_vMax.GetX(), m_vMin.GetY(), m_vMax.GetZ()) * matWorld,
             Vector3(m_vMax.GetX(), m_vMax.GetY(), m_vMin.GetZ()) * matWorld,
             m_vMax * matWorld
        };
    }
    // ���� AABB ���
    inline void GetWorldAABB(const Matrix4x4& matWorld, Vector3& vMin, Vector3& vMax) const 
    {
        //8���� ���庯ȯ�� �������� �����
        std::array<Vector3, 8> corners;
        GetWorldCorners(matWorld, corners);
        
        //�ش� �ڳʸ� ������� �ϴ� AABB�� �����ؼ� ��ȯ�Ѵ�
        float minX, minY, minZ, maxX, maxY, maxZ;
        minX = minY = minZ = FLT_MAX;
        maxX = maxY = maxZ = FLT_MIN;
        for (const auto& iter : corners)
        {
            minX = std::min(minX, iter.GetX()); maxX = std::max(maxX, iter.GetX());
            minY = std::min(minY, iter.GetY()); maxY = std::max(maxY, iter.GetY());
            minZ = std::min(minZ, iter.GetZ()); maxZ = std::max(maxZ, iter.GetZ());
        }
        vMin.Set(minX, minY, minZ);
        vMax.Set(maxX, maxY, maxZ);
    }
    
    // ���� OBB ���
    inline void GetWorldOBB(const Matrix4x4& matWorld, Vector3& center, std::array<Vector3, 3>& axes, Vector3& extent) const 
    {
        // ���� �߽����� ���� ��ķ� ��ȯ
        center = ((m_vMin + m_vMax) * 0.5f) * matWorld;

        // ���� ��Ŀ��� �� ���� ����
        axes[0] = GetAxesRightFromWorld(matWorld);
        axes[1] = GetAxesUpFromWorld(matWorld);
        axes[2] = GetAxesForwardFromWorld(matWorld);

        // ���� ũ�⸦ ���
        extent = ((m_vMax - m_vMin) * 0.5f).ABS();  //extent�� ���ȭ�� ���� ���밪ó��
        extent *= GetScaleFromWorld(matWorld);      //scale�� length�� ó���Ǿ� �����⶧���� ������� ���´�
    }
	
private:
	Vector3 m_vMin;
	Vector3 m_vMax;
};