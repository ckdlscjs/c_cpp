#include "stdafx.h"
#include "GraphicsClass.h"
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
