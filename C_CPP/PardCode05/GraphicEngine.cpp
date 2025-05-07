#include "GraphicEngine.h"
#include "Window.h"
#include "Direct3D.h"
#include "SwapChain.h"
#include "VertexBuffer.h"
#include "InputLayout.h"
GraphicEngine::GraphicEngine()
{
	std::cout << "GraphicEngine" << " Class" << " ������ ȣ��" << '\n';
}

GraphicEngine::~GraphicEngine()
{
	std::cout << "GraphicEngine" << " Class" << "�Ҹ��� ȣ��" << '\n';
	Release();
}

bool GraphicEngine::Init(Window* pWindow)
{
	m_pCDirect3D = new Direct3D();
	m_pCDirect3D->Init();

	m_pCSwapChain = new SwapChain();
	RECT rect = pWindow->GetClientWindowRect();
	m_pCSwapChain->Init(m_pCDirect3D->GetDevice(), pWindow->GetHwnd(), rect.right - rect.left, rect.bottom - rect.top);
	

	XMFLOAT3 vertices[] =
	{
		{-0.5f, -0.5f, 0.0f},
		{0.0f, 0.5f, 0.0f},
		{0.5f, -0.5f, 0.0f},
	};
	UINT size_vertices = ARRAYSIZE(vertices);
	m_pCVertexBuffer = new VertexBuffer();
	m_pCVertexBuffer->Init(m_pCDirect3D->GetDevice(), vertices, sizeof(XMFLOAT3), size_vertices);

	//vs,ps ������, �����̰�����
	ID3DBlob* errBlob;
	HRESULT hResult;
	//compile vs
	hResult = D3DCompileFromFile(L"VertexShader.hlsl", nullptr, nullptr, "vsmain", "vs_5_0", NULL, NULL, &m_pBlob_VS, &errBlob);
	if (FAILED(hResult))
	{
		OutputDebugStringA((char*)errBlob->GetBufferPointer());
		if (errBlob)
			errBlob->Release();
		return false;
	}
	//create vs
	hResult = m_pCDirect3D->GetDevice()->CreateVertexShader(m_pBlob_VS->GetBufferPointer(), m_pBlob_VS->GetBufferSize(), nullptr, &m_pVS);
	if (FAILED(hResult))
		return false;

	//compile ps
	hResult = D3DCompileFromFile(L"PixelShader.hlsl", nullptr, nullptr, "psmain", "ps_5_0", NULL, NULL, &m_pBlob_PS, &errBlob);
	if (FAILED(hResult))
	{
		OutputDebugStringA((char*)errBlob->GetBufferPointer());
		if (errBlob)
			errBlob->Release();
		return false;
	}
	//create ps
	hResult = m_pCDirect3D->GetDevice()->CreatePixelShader(m_pBlob_PS->GetBufferPointer(), m_pBlob_PS->GetBufferSize(), nullptr, &m_pPS);
	if (FAILED(hResult))
		return false;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	m_pCInputLayout = new InputLayout();
	m_pCInputLayout->Init(m_pCDirect3D->GetDevice(), layout, ARRAYSIZE(layout), m_pBlob_VS->GetBufferPointer(), m_pBlob_VS->GetBufferSize());

	m_pCDirect3D->SetViewportSize(rect.right - rect.left, rect.bottom - rect.top);
	return true;
}

bool GraphicEngine::Frame()
{
	std::cout << "GraphicEngine" << " Class" << " Frame ȣ��" << '\n';
	//����Ÿ�� �ʱ�ȭ �� ��º��ձ⿡ ����Ÿ�ٺ� ����
	m_pCSwapChain->ClearRenderTargetColor(m_pCDirect3D->GetDeviceContext(), 0, 0.3f, 0.4f, 1);

	m_pCInputLayout->SetInputLayout(m_pCDirect3D->GetDeviceContext());

	m_pCVertexBuffer->SetVertexBuffer(m_pCDirect3D->GetDeviceContext());

	m_pCDirect3D->GetDeviceContext()->VSSetShader(m_pVS, nullptr, 0);
	m_pCDirect3D->GetDeviceContext()->PSSetShader(m_pPS, nullptr, 0);
	return true;
}

bool GraphicEngine::Render()
{
	std::cout << "GraphicEngine" << " Class" << " Render ȣ��" << '\n';
	m_pCDirect3D->DrawVertex_TriangleList(m_pCVertexBuffer->GetCountVertices(), 0);
	m_pCSwapChain->Present(false);
	return true;
}

bool GraphicEngine::Release()
{
	delete m_pCInputLayout;
	m_pCInputLayout = nullptr;
	
	delete m_pCVertexBuffer;
	m_pCVertexBuffer = nullptr;

	delete m_pCSwapChain;
	m_pCSwapChain = nullptr;
	
	delete m_pCDirect3D;
	m_pCDirect3D = nullptr;

	m_pBlob_PS->Release();
	m_pBlob_VS->Release();
	m_pVS->Release();
	m_pPS->Release();
	return true;
}

//�̱��水ü, static�������� ������Ÿ�ӿ� ��ü�� �����ȴ�
GraphicEngine& GraphicEngine::Get()
{
	static GraphicEngine engine;
	return engine;
}
