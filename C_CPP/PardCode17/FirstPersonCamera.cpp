#include "FirstPersonCamera.h"

FirstPersonCamera::FirstPersonCamera()
{
	std::cout << "FirstPersonCamera" << " Class" << " 생성자 호출" << '\n';
}

FirstPersonCamera::~FirstPersonCamera()
{
	std::cout << "FirstPersonCamera" << " Class" << " 생성자 호출" << '\n';
}

void FirstPersonCamera::Frame(float deltaTime)
{
	/*
	m_Properties.m_fYaw += deltaTime;
	m_bDirtyFlag_View = true;
	*/
}
