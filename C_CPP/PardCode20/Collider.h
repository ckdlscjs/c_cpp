#pragma once
#include "CommonHeader.h"
enum class ColliderType
{
    NONE,
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