#pragma once

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;


class D3DClass;
class CameraClass;
class ModelClass;
class TextureShaderClass;
class HorizontalBlurShaderClass;
class VerticalBlurShaderClass;
class RenderTextureClass;
class OrthoWindowClass;

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
	bool Render(float);

	bool RenderSceneToTexture(float);
	bool DownSampleTexture();
	bool RenderHorizontalBlurToTexture();
	bool RenderVerticalBlurToTexture();
	bool UpSampleTexture();
	bool Render2DTextureScene();

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	ModelClass* m_Model = nullptr;
	TextureShaderClass* m_TextureShader = nullptr;
	HorizontalBlurShaderClass* m_HorizontalBlurShader = nullptr;
	VerticalBlurShaderClass* m_VerticalBlurShader = nullptr;
	RenderTextureClass *m_RenderTexture = nullptr;
	RenderTextureClass *m_DownSampleTexure = nullptr;
	RenderTextureClass *m_HorizontalBlurTexture = nullptr;
	RenderTextureClass *m_VerticalBlurTexture = nullptr;
	RenderTextureClass *m_UpSampleTexure = nullptr;
	OrthoWindowClass *m_SmallWindow = nullptr;
	OrthoWindowClass *m_FullScreenWindow = nullptr;
};