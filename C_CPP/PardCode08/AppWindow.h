#pragma once
#include "Window.h"

class AppWindow : public Window
{
public:
	AppWindow();
	virtual ~AppWindow();

	//��ӵȰ����Լ���
	virtual void OnCreate() override;
	virtual void OnUpdate() override;
	virtual void OnDestroy() override;
};