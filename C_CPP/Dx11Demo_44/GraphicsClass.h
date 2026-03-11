#pragma once

/////////////
// GLOBALS //
/////////////
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 100.0f;
const float SCREEN_NEAR = 1.0f;


class D3DClass;
class CameraClass;
class ModelClass;
class LightClass;
class ProjectionShaderClass;
class TextureClass;
class ViewPointClass;

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

private:
	D3DClass* m_Direct3D = nullptr;
	CameraClass* m_Camera = nullptr;
	ModelClass *m_GroundModel = nullptr;
	ModelClass *m_CubeModel = nullptr;
	LightClass* m_Light = nullptr;
	ProjectionShaderClass* m_ProjectionShader = nullptr;
	TextureClass* m_ProjectionTexture = nullptr;
	ViewPointClass* m_ViewPoint = nullptr;
};