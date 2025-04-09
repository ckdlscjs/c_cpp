#include "stdafx.h"
#include "GraphicsClass.h"
#include "D3DClass.h"

GraphicsClass::GraphicsClass()
{
}

GraphicsClass::GraphicsClass(const GraphicsClass&)
{
}

GraphicsClass::~GraphicsClass()
{
}

bool GraphicsClass::Initialize(int, int, HWND)
{
	//D3D °´Ã¼ »ý¼º
	m_Direct3D = (D3DClass*)_aligned_malloc(sizeof(D3DClass), 16);
	if (!m_Direct3D)
		return false;

	return true;
}

void GraphicsClass::Shutdown()
{
}

bool GraphicsClass::Frame()
{
	return true;
}

bool GraphicsClass::Render()
{
	return true;
}
