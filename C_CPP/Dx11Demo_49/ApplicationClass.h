#pragma once

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;
const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;
const float SHADOWMAP_DEPTH = 50.0f;
const float SHADOWMAP_NEAR = 1.0f;


class InputClass;
class D3DClass;
class TimerClass;
class PositionClass;
class CameraClass;
class LightClass;
class ModelClass;
class TreeClass;
class RenderTextureClass;
class DepthShaderClass;
class TransparentDepthShaderClass;
class ShadowShaderClass;

class ApplicationClass
{
public:
	ApplicationClass();
	ApplicationClass(const ApplicationClass&);
	~ApplicationClass();

	bool Initialize(HINSTANCE, HWND, int, int);
	void Shutdown();
	bool Frame();

private:
	bool HandleMovementInput(float);
	void UpdateLighting();
	bool Render();
	bool RenderSceneToTexture();

private:
	InputClass* m_Input = nullptr;
	D3DClass* m_Direct3D = nullptr;
	TimerClass* m_Timer = nullptr;
	PositionClass* m_Position = nullptr;
	CameraClass* m_Camera = nullptr;
	LightClass* m_Light = nullptr;
	ModelClass* m_GroundModel = nullptr;
	TreeClass* m_Tree = nullptr;
	RenderTextureClass* m_RenderTexture = nullptr;
	DepthShaderClass* m_DepthShader = nullptr;
	TransparentDepthShaderClass* m_TransparentDepthShader = nullptr;
	ShadowShaderClass* m_ShadowShader = nullptr;
};