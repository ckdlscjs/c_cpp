#pragma once

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;					//전체화면
const bool VSYNC_ENABLED = true;				//수직동기화
const float SCREEN_DEPTH = 1000.0f;				//스크린깊이
const float SCREEN_NEAR = 0.1f;					


class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame();

private:
	bool Render();
};