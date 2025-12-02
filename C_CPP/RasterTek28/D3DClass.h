#pragma once

class D3DClass : public AlignedAllocationPolicy<16>
{
public:
	D3DClass();
	D3DClass(const D3DClass&);
	~D3DClass();

	bool Initialize(int, int, bool, HWND, bool, float, float);
	void Shutdown();

	void BeginScene(float, float, float, float);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

	void TurnZBufferOn();
	void TurnZBufferOff();

	void TurnOnAlphaBlending();
	void TurnOffAlphaBlending();

	ID3D11DepthStencilView* GetDepthStencilView();
	void SetBackBufferRenderTarget();

private:
	bool m_vsync_enabled = false;

	IDXGISwapChain* m_swapChain = nullptr;
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;
	ID3D11RenderTargetView* m_renderTargetView = nullptr;
	ID3D11Texture2D* m_depthStencilBuffer = nullptr;
	ID3D11DepthStencilState* m_depthStencilState = nullptr;
	ID3D11DepthStencilView* m_depthStencilView = nullptr;
	ID3D11RasterizerState* m_rasterState = nullptr;

	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;

	ID3D11DepthStencilState* m_depthDisabledStencilState = nullptr;
	ID3D11BlendState* m_alphaEnableBlendingState = nullptr;
	ID3D11BlendState* m_alphaDisableBlendingState = nullptr;
};