#pragma once

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;					//전체화면
const bool VSYNC_ENABLED = true;				//수직동기화
const float SCREEN_DEPTH = 1000.0f;				//스크린깊이
const float SCREEN_NEAR = 0.1f;					

//전방선언
class D3DClass;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass& graphicClass);
	~GraphicsClass();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd);
	void Shutdown();
	bool Frame();

private:
	bool Render();

private:
	D3DClass* m_Direct3D = nullptr; //DirectX3D객체 멤버변수
};