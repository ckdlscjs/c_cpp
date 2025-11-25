#pragma once
#include "Collider.h"
/*
// Box.h
#pragma once
#include <array>
#include <memory>
#include "Collider.h"
#include "Matrix4x4.h"

// 로컬 AABB와 월드 행렬을 가진 단일 Box 클래스
class Box : public Collider
{
public:
    // 로컬 min/max와 월드 변환 행렬로 박스 초기화
    inline Box(const Vector3& localMin, const Vector3& localMax, const Matrix4x4& worldMatrix)
        : m_localMin(localMin), m_localMax(localMax), m_worldMatrix(worldMatrix) {}

    // Collider::GetType() 구현
    // Box는 OBB와 AABB를 모두 표현하므로, OBB 타입으로 통일합니다.
    // AABB는 OBB의 특별한 경우(회전이 0)로 간주할 수 있습니다.
    Type GetType() const override { return Type::OBB; }

    // 다른 Collider와의 교차 판정 로직
    bool Intersects(const Collider& other) const override;

    // Box vs Box 충돌 판정 로직
    bool IntersectsBox(const Box& other) const;

    // 현재 Box의 월드 행렬과 로컬 크기를 기반으로 월드 AABB를 계산
    void GetWorldAABB(Vector3& min, Vector3& max) const;

    // 월드 행렬과 로컬 min/max를 기반으로 OBB 정보 (중심, 축, 크기)를 계산
    void GetWorldOBB(Vector3& center, std::array<Vector3, 3>& axes, Vector3& extents) const;

private:
    Vector3 m_localMin;
    Vector3 m_localMax;
    Matrix4x4 m_worldMatrix;
};

// Box.cpp
#include "Box.h"
#include "SphereCollider.h" // SphereCollider와 충돌 판정을 위해 포함
#include <iostream>

// Box와 다른 Collider의 교차 판정
bool Box::Intersects(const Collider& other) const {
    // 런타임에 상대방의 타입을 확인
    switch (other.GetType()) {
        case Collider::Type::OBB: {
            // 다른 OBB 충돌체와의 판정
            const Box& otherBox = static_cast<const Box&>(other);
            return IntersectsBox(otherBox);
        }
        case Collider::Type::SPHERE: {
            // Box와 Sphere의 충돌 판정
            const SphereCollider& sphere = static_cast<const SphereCollider&>(other);
            // 이 로직은 Sphere vs Box 충돌 알고리즘을 사용해야 합니다.
            // 여기서는 개념적으로만 표시합니다.
            std::cout << "Box-Sphere 충돌 판정 실행 (개념적)\n";
            return false;
        }
        default:
            return false;
    }
}


// OBB-OBB 충돌 판정을 위한 분리 축 정리(SAT) 함수
// 이 함수는 두 OBB가 교차하는지 판정합니다.
static bool CheckIntersectionSAT(
    const Vector3& center1, const std::array<Vector3, 3>& axes1, const Vector3& extents1,
    const Vector3& center2, const std::array<Vector3, 3>& axes2, const Vector3& extents2)
{
    // 충돌이 없음을 판정하기 위한 15개의 분리 축을 정의합니다.
    // 1. 각 박스의 로컬 축 (총 6개)
    // 2. 각 박스의 로컬 축의 외적 (총 9개)

    // 1단계: 첫 번째 박스의 축을 투영 축으로 사용
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

    // 2단계: 두 번째 박스의 축을 투영 축으로 사용
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

    // 3단계: 두 박스의 축을 교차(외적)하여 투영 축으로 사용
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            Vector3 axis = CrossProduct(axes1[i], axes2[j]);
            if (axis.LengthSquared() < 0.00001f) continue; // 축이 평행한 경우 건너뜁니다.

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

    return true; // 모든 축에 대해 겹치므로 충돌
}

// Box vs Box 충돌 판정
bool Box::IntersectsBox(const Box& other) const {
    // 회전이 없으면 AABB로 간주, 있으면 OBB로 간주하는 로직
    // Matrix4x4 클래스에 회전 여부를 확인하는 함수를 추가하여 판단할 수 있습니다.
    bool thisIsAABB = !m_worldMatrix.IsRotated();
    bool otherIsAABB = !other.m_worldMatrix.IsRotated();

    if (thisIsAABB && otherIsAABB) {
        // AABB vs AABB 충돌 판정
        Vector3 myMin, myMax, otherMin, otherMax;
        this->GetWorldAABB(myMin, myMax);
        other.GetWorldAABB(otherMin, otherMax);
        return (myMin.x <= otherMax.x && myMax.x >= otherMin.x) &&
               (myMin.y <= otherMax.y && myMax.y >= otherMin.y) &&
               (myMin.z <= otherMax.z && myMax.z >= otherMin.z);
    } else {
        // OBB vs OBB 충돌 판정 (실제로는 분리 축 정리 SAT를 사용)
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
    // 월드 AABB 계산
    inline void GetWorldAABB(const Matrix4x4& matWorld, Vector3& vMin, Vector3& vMax) const 
    {
        //8개의 월드변환된 꼭지점을 만든다
        std::array<Vector3, 8> corners;
        GetWorldCorners(matWorld, corners);
        
        //해당 코너를 기반으로 하는 AABB를 생성해서 반환한다
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
    
    // 월드 OBB 계산
    inline void GetWorldOBB(const Matrix4x4& matWorld, Vector3& center, std::array<Vector3, 3>& axes, Vector3& extent) const 
    {
        // 로컬 중심점을 월드 행렬로 변환
        center = ((m_vMin + m_vMax) * 0.5f) * matWorld;

        // 월드 행렬에서 축 정보 추출
        axes[0] = GetAxesRightFromWorld(matWorld);
        axes[1] = GetAxesUpFromWorld(matWorld);
        axes[2] = GetAxesForwardFromWorld(matWorld);

        // 로컬 크기를 계산
        extent = ((m_vMax - m_vMin) * 0.5f).ABS();  //extent의 양수화를 위한 절대값처리
        extent *= GetScaleFromWorld(matWorld);      //scale은 length로 처리되어 나오기때문에 양수값이 나온다
    }
	
private:
	Vector3 m_vMin;
	Vector3 m_vMax;
};