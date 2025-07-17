#include "stdafx.h"
#include "D3DClass.h"

D3DClass::D3DClass()
{
}

D3DClass::D3DClass(const D3DClass& d3dClass)
{
}

D3DClass::~D3DClass()
{
}

bool D3DClass::Initialize(int screenWidth, int screenHeight, HWND hwnd, bool vsync, bool isFullScreen, float screenDepth, float screenNear)
{
	//���� ����ȭ ���¸� ����
	m_vsync_enabled = vsync;

	//DirectX Grahpic Interface Factory�� ����(DXGI FACTORY)
	IDXGIFactory* factory = nullptr;
	assert(SUCCEEDED(CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory)) && "CreateDXGIFactory failed!");
	
	//���丮 ��ü�� �̿��Ͽ� ù���� �׷���ī�� �������̽� ����͸� ����
	IDXGIAdapter* adapter = nullptr;
	assert(SUCCEEDED(factory->EnumAdapters(0, &adapter)) && "CreateDXGIAdapater failed!");

	//���(�����)�� ���� ù��° ����͸� ����
	IDXGIOutput* adapterOutput = nullptr;
	assert(SUCCEEDED(adapter->EnumOutputs(0, &adapterOutput)) && "CreateAdapterOutput failed!");

	//���(�����)�� ���� DXGI_FORMAT_R8G8B8A8_UNORMǥ�� ���Ŀ� �´� ��� ���� �����´�
	unsigned int numModes = 0;
	assert(SUCCEEDED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL)) && "GeDisplayModeList failed!");

	// ������ ��� ����Ϳ� �׷���ī�� ������ ������ ����Ʈ�� �����մϴ�
	DXGI_MODE_DESC* displayModeList = new DXGI_MODE_DESC[numModes];
	assert(displayModeList && "DXGIModeList Init failed!");

	// ���� ���÷��� ��忡 ���� ����Ʈ�� ä��ϴ�
	assert(SUCCEEDED(adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList)) && "InitDisplayModeList failed");

	// ���� ��� ���÷��� ��忡 ���� ȭ�� �ʺ�/���̿� �´� ���÷��� ��带 ã���ϴ�.
	// ������ ���� ã���� ������� ���ΰ�ħ ������ �и�� ���� ���� �����մϴ�.
	unsigned int numerator = 0;
	unsigned int denominator = 0;
	for (unsigned int i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// ����ī���� ����ü�� ����ϴ�
	DXGI_ADAPTER_DESC adapterDesc;
	assert(SUCCEEDED(adapter->GetDesc(&adapterDesc)) && "AdapterDescription init failed!");
	
	// ����ī�� �޸� �뷮 ������ �ް�����Ʈ ������ �����մϴ�
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// ����ī���� �̸��� �����մϴ�
	size_t stringLength = 0;
	assert(wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128) == 0);

	// ���÷��� ��� ����Ʈ�� �����մϴ�
	delete[] displayModeList;
	displayModeList = 0;

	// ��� ��͸� �����մϴ�
	adapterOutput->Release();
	adapterOutput = 0;

	// ��͸� �����մϴ�
	adapter->Release();
	adapter = 0;

	// ���丮 ��ü�� �����մϴ�
	factory->Release();
	factory = 0;

	// ����ü�� ����ü�� �ʱ�ȭ�մϴ�
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// ����۸� 1���� ����ϵ��� �����մϴ�
	swapChainDesc.BufferCount = 1;

	// ������� ���̿� ���̸� �����մϴ�
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// 32bit �����̽��� �����մϴ�
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ������� ���ΰ�ħ ������ �����մϴ�
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// ������� ���뵵�� �����մϴ�
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// �������� ���� ������ �ڵ��� �����մϴ�
	swapChainDesc.OutputWindow = hwnd;

	// ��Ƽ���ø��� ���ϴ�
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// â��� or Ǯ��ũ�� ��带 �����մϴ�
	if (isFullScreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// ��ĵ ���� ���� �� ũ�⸦ �������� �������� �����մϴ�.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// ��µ� ���� ����۸� ��쵵�� �����մϴ�
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// �߰� �ɼ� �÷��׸� ������� �ʽ��ϴ�
	swapChainDesc.Flags = 0;

	// ��ó������ DirectX 11 �� �����մϴ�
	D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

	// ���� ü��, Direct3D ��ġ �� Direct3D ��ġ ���ؽ�Ʈ�� ����ϴ�.
	assert(SUCCEEDED(D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_SwapChain, &m_Device, NULL, &m_DeviceContext)) && "Create SwapChian, Device, DeviceContext failed!");
	

	// ����� �����͸� ���ɴϴ�
	ID3D11Texture2D* backBufferPtr = nullptr;
	assert(SUCCEEDED(m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr)) && "GetBackBufferFailed!");

	// �� ���� �����ͷ� ���� Ÿ�� �並 �����Ѵ�.
	assert(SUCCEEDED(m_Device->CreateRenderTargetView(backBufferPtr, NULL, &m_RTV)) && "Create RTV failed!");

	// ����� �����͸� �����մϴ�
	backBufferPtr->Release();
	backBufferPtr = 0;

	// ���� ���� ����ü�� �ʱ�ȭ�մϴ�
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// ���� ���� ����ü�� �ۼ��մϴ�
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// ������ ���̹��� ����ü�� ����Ͽ� ���� ���� �ؽ��ĸ� �����մϴ�
	assert(SUCCEEDED(m_Device->CreateTexture2D(&depthBufferDesc, NULL, &m_DetphStencilBuffer)) && "Create Depth StencilBuffer failed!");

	// ���� ���ٽ� ���� ����ü�� �ʱ�ȭ�մϴ�
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// ���� ���ٽ� �� ����ü�� �����մϴ�
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// ���� ���ٽ� �並 �����մϴ�
	assert(SUCCEEDED(m_Device->CreateDepthStencilView(m_DetphStencilBuffer, &depthStencilViewDesc, &m_DSV)) && "Create DSV failed!");

	// ������ ��� ��� ���� ���ٽ� ���۸� ��� ���� ������ ���ο� ���ε��մϴ�
	m_DeviceContext->OMSetRenderTargets(1, &m_RTV, m_DSV);

	// ���ٽ� ���� ����ü�� �ʱ�ȭ�մϴ�
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// ���ٽ� ���� ����ü�� �ۼ��մϴ�
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// �ȼ� ������ ���ٽ� �����Դϴ�
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// �ȼ� �޸��� ���ٽ� �����Դϴ�
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// ���� ���ٽ� ���¸� �����մϴ�
	assert(SUCCEEDED(m_Device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilState)) && "Create Detph Stencil State failed!");


	// ���� ���ٽ� ���¸� �����մϴ�
	m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState, 1);

	// �׷����� ������� ����� ������ ������ ����ü�� �����մϴ�
	D3D11_RASTERIZER_DESC rasterDesc;
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// ��� �ۼ��� ����ü���� ������ ������ ���¸� ����ϴ�
	assert(SUCCEEDED(m_Device->CreateRasterizerState(&rasterDesc, &m_rasterizerState)) && "Create Rasterizer State failed!");

	// ���� ������ ������ ���¸� �����մϴ�
	m_DeviceContext->RSSetState(m_rasterizerState);

	// �������� ���� ����Ʈ�� �����մϴ�
	D3D11_VIEWPORT viewport;
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// ����Ʈ�� �����մϴ�
	m_DeviceContext->RSSetViewports(1, &viewport);

	// ���� ����� �����մϴ�
	float fieldOfView = 3.141592654f / 4.0f;
	float screenAspect = (float)screenWidth / (float)screenHeight;

	// 3D ������������ ���� ����� ����ϴ�
	m_ProjectionMatrix = DirectX::XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth);

	// ���� ����� �׵� ��ķ� �ʱ�ȭ�մϴ�
	m_WorldMatrix = DirectX::XMMatrixIdentity();

	// 2D ������������ ���� ���� ����� ����ϴ�
	M_OrthoMatrix = DirectX::XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}

void D3DClass::Shutdown()
{
	// ���� �� ������ ���� �������� ������ ���� ü���� ���� �� �� ���ܰ� �߻��մϴ�.
	if (m_SwapChain)
	{
		m_SwapChain->SetFullscreenState(false, NULL);
	}

	if (m_rasterizerState)
	{
		m_rasterizerState->Release();
		m_rasterizerState = 0;
	}

	if (m_DSV)
	{
		m_DSV->Release();
		m_DSV = 0;
	}

	if (m_DepthStencilState)
	{
		m_DepthStencilState->Release();
		m_DepthStencilState = 0;
	}

	if (m_DetphStencilBuffer)
	{
		m_DetphStencilBuffer->Release();
		m_DetphStencilBuffer = 0;
	}

	if (m_RTV)
	{
		m_RTV->Release();
		m_RTV = 0;
	}

	if (m_DeviceContext)
	{
		m_DeviceContext->Release();
		m_DeviceContext = 0;
	}

	if (m_Device)
	{
		m_Device->Release();
		m_Device = 0;
	}

	if (m_SwapChain)
	{
		m_SwapChain->Release();
		m_SwapChain = 0;
	}
}

void D3DClass::BeginScene(float red, float green, float blue, float alpha)
{
	//���۸� ���� ���� �����Ѵ�
	float color[4] = { red, green, blue, alpha };

	// ����۸� ����ϴ�
	m_DeviceContext->ClearRenderTargetView(m_RTV, color);

	// ���� ���۸� ����ϴ�
	m_DeviceContext->ClearDepthStencilView(m_DSV, D3D11_CLEAR_DEPTH, 1.0f, 0);

}

void D3DClass::EndScene()
{
	// �������� �Ϸ�Ǿ����Ƿ� ȭ�鿡 �� ���۸� ǥ���մϴ�.
	if (m_vsync_enabled)
	{
		// ȭ�� ���� ��ħ ������ �����մϴ�.
		m_SwapChain->Present(1, 0);
	}
	else
	{
		// ������ ������ ����մϴ�
		m_SwapChain->Present(0, 0);
	}
}

ID3D11Device* D3DClass::GetDevice()
{
	return m_Device;
}

ID3D11DeviceContext* D3DClass::GetDeviceContext()
{
	return m_DeviceContext;
}

void D3DClass::GetProjectionMatrix(DirectX::XMMATRIX& projectionMatrix)
{
	projectionMatrix = m_ProjectionMatrix;
}

void D3DClass::GetWorldMatrix(DirectX::XMMATRIX& worldMatrix)
{
	worldMatrix = m_WorldMatrix;
}

void D3DClass::GetOrthoMatrix(DirectX::XMMATRIX& orthoMatrix)
{
	orthoMatrix = M_OrthoMatrix;
}

//�׷���ī�������� ��ȯ
void D3DClass::GetVideoCardInfo(char* cardName, int& memory)
{
	strcpy_s(cardName, 128, m_videoCardDescription);
	memory = m_videoCardMemory;
}
