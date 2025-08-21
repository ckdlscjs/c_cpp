#pragma once
#include "std.h"
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