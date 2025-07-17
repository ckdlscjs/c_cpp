#pragma once

class D3DClass : public AlignedAllocationPolicy<16>
{
public:
	D3DClass();
	D3DClass(const D3DClass& d3dClass);
	~D3DClass();

	bool Initialize(int screenWidth, int screenHeight, HWND hwnd, bool vsync, bool isFullScreen, float screenDepth, float screenNear);
	void Shutdown();
	void BeginScene(float r, float g, float b, float a);
	void EndScene();

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(DirectX::XMMATRIX& projectionMatrix);
	void GetWorldMatrix(DirectX::XMMATRIX& worldMatrix);
	void GetOrthoMatrix(DirectX::XMMATRIX& orthoMatrix);

	void GetVideoCardInfo(char*, int&);
private:
	bool m_vsync_enabled = false;
	int m_videoCardMemory = 0;
	char m_videoCardDescription[128] = { 0, };
	IDXGISwapChain* m_SwapChain = nullptr;
	ID3D11Device* m_Device = nullptr;
	ID3D11DeviceContext* m_DeviceContext = nullptr;
	ID3D11RenderTargetView* m_RTV = nullptr;
	ID3D11DepthStencilView* m_DSV = nullptr;
	ID3D11Texture2D* m_DetphStencilBuffer = nullptr;
	ID3D11DepthStencilState* m_DepthStencilState = nullptr;
	ID3D11RasterizerState* m_rasterizerState = nullptr;
	//ViewMatrix는 카메라클래스에서 사용하기위해 D3DClass내부에 선언하지않음
	DirectX::XMMATRIX m_ProjectionMatrix;
	DirectX::XMMATRIX m_WorldMatrix;
	DirectX::XMMATRIX M_OrthoMatrix;
};
