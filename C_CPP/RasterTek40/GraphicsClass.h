#pragma once

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 1.0f;
const int SHADOWMAP_WIDTH = 1024;
const int SHADOWMAP_HEIGHT = 1024;

class D3DClass;
class CameraClass;
class ModelClass;
class LightClass;
class RenderTextureClass;
class DepthShaderClass;
class ShadowShaderClass;

class GraphicsClass
{
public:
	GraphicsClass();
	GraphicsClass(const GraphicsClass&);
	~GraphicsClass();

	bool Initialize(int, int, HWND);
	void Shutdown();
	bool Frame(float, float, float, float, float, float);

private:
	bool RenderSceneToTexture();
	bool Render();

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;;
	ModelClass *m_CubeModel = nullptr;
	ModelClass *m_GroundModel = nullptr;
	ModelClass *m_SphereModel = nullptr;
	LightClass* m_Light = nullptr;
	RenderTextureClass* m_RenderTexture = nullptr;
	DepthShaderClass* m_DepthShader = nullptr;
	ShadowShaderClass* m_ShadowShader = nullptr;
};