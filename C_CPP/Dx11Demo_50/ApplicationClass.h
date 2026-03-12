#pragma once

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


class InputClass;
class D3DClass;
class CameraClass;
class LightClass;
class ModelClass;
class OrthoWindowClass;
class DeferredBuffersClass;
class DeferredShaderClass;
class LightShaderClass;


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
	bool Render();
	bool RenderSceneToTexture();

private:
	InputClass* m_Input = nullptr;
	D3DClass* m_D3D = nullptr;
	CameraClass* m_Camera = nullptr;
	LightClass* m_Light = nullptr;
	ModelClass* m_Model = nullptr;
	OrthoWindowClass* m_FullScreenWindow = nullptr;
	DeferredBuffersClass* m_DeferredBuffers = nullptr;
	DeferredShaderClass* m_DeferredShader = nullptr;
	LightShaderClass* m_LightShader = nullptr;
};