#pragma once
#include "Window.h"
class TempObj;
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