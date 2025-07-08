#pragma once
#include "Window.h"

class AppWindow : public Window
{
public:
	AppWindow();
	virtual ~AppWindow();

	//상속된가상함수들
	virtual void OnCreate() override;
	virtual void OnUpdate() override;
	virtual void OnDestroy() override;
};