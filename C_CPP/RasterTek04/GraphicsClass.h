#pragma once

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;					//��üȭ��
const bool VSYNC_ENABLED = true;				//��������ȭ
const float SCREEN_DEPTH = 1000.0f;				//��ũ������
const float SCREEN_NEAR = 0.1f;					

//���漱��
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
	D3DClass* m_Direct3D = nullptr; //DirectX3D��ü �������
};