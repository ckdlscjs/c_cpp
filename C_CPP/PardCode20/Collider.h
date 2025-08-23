#pragma once
#include "CommonHeader.h"
enum class ColliderType
{
    SPHERE,
    AABB,
    OBB,
    RAY,
    PLANE
};
class Collider
{
public:
	Collider() {}
	virtual ~Collider() {}
};