#pragma once
#include "std.h"
class Component
{
};

class TransformComponent : public Component
{
public:
	Position m_vPosition;
	Vector3 m_vRotation;
	Vector3 m_vScale;
};

class InputComponent : public Component
{

};