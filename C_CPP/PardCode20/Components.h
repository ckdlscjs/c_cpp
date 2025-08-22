#pragma once
#include "CommonHeader.h"

class Component
{
};

class TransformComponent : public Component
{
public:
	Vector3 m_vPosition;
	Vector3 m_vRotation;
	Vector3 m_vScale;
};

class InputComponent : public Component
{

};