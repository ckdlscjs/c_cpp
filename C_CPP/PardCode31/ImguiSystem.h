#pragma once
#include "BaseSystem.h"

class ImguiSystem : public BaseSystem<ImguiSystem>
{
	friend class BaseSystem<ImguiSystem>;	//CRTP패턴을 위한 friend선언
private:
	ImguiSystem();
	ImguiSystem(const ImguiSystem&) = delete;
	ImguiSystem& operator=(const ImguiSystem&) = delete;
	ImguiSystem(ImguiSystem&&) = delete;
	ImguiSystem& operator=(ImguiSystem&&) = delete;

public:
	~ImguiSystem();
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
	void Init(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	void Frame();
	void Render();
	

private:

};
#define _ImguiSystem ImguiSystem::GetInstance()
