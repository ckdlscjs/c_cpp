#pragma once

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;					//��üȭ��
const bool VSYNC_ENABLED = true;				//��������ȭ
const float SCREEN_DEPTH = 1000.0f;				//��ũ������
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