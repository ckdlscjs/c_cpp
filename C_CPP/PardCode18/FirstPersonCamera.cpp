#include "FirstPersonCamera.h"

FirstPersonCamera::FirstPersonCamera()
{
	std::cout << "Initialize : " << "FirstPersonCamera" << " Class" << '\n';
}

FirstPersonCamera::~FirstPersonCamera()
{
	std::cout << "Release : " << "FirstPersonCamera" << " Class" << '\n';
}

void FirstPersonCamera::Frame(float deltaTime)
{
	/*
	m_Properties.m_fYaw += deltaTime;
	m_bDirtyFlag_View = true;
	*/
}
