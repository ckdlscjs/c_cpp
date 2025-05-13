#include "GraphicEngine.h"
#include "Window.h"
#include "Direct3D.h"
#include "SwapChain.h"
#include "VertexBuffer.h"
#include "InputLayout.h"
#include "VertexShader.h"
#include "PixelShader.h"

GraphicEngine::GraphicEngine()
{
	std::cout << "GraphicEngine" << " Class" << " ������ ȣ��" << '\n';
}

GraphicEngine::~GraphicEngine()
{
	std::cout << "GraphicEngine" << " Class" << "�Ҹ��� ȣ��" << '\n';
	Release();
}

void GraphicEngine::Init(HWND hWnd, UINT width, UINT height)
{
	m_pCDirect3D = new Direct3D();
	m_pCDirect3D->Init();

	m_pCSwapChain = new SwapChain();
	m_pCSwapChain->Init(m_pCDirect3D->GetDevice(), hWnd, width, height);

	//������������ ������ü ���ù� VertexBuffer����
	Vertex vertices[] =
	{
		{{-0.5f, -0.5f, 0.0f}, {1, 0, 0, 0.1f}},
		{{-0.5f, 0.5f, 0.0f}, {0, 1, 0, 0.1f}},
		{{0.5f, -0.5, 0.0f}, {0, 0, 1, 0.1f}},
		{{0.5f, 0.5f, 0.0f}, {1, 1, 0, 0.1f}},
	};
	//�Է¹�ġ ��ü ���� �� �ʱ�ȭ
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	
	UINT size_vertices = ARRAYSIZE(vertices);
	m_pCVertexBuffer = new VertexBuffer();
	m_pCVertexBuffer->Init(m_pCDirect3D->GetDevice(), vertices, sizeof(Vertex), size_vertices);

	m_pBlob_VS = CompileShader(L"VertexShader.hlsl", "vsmain", "vs_5_0");
	m_pCVertexShader = new VertexShader();
	m_pCVertexShader->Init(m_pCDirect3D->GetDevice(), m_pBlob_VS);

	m_pCInputLayout = new InputLayout();
	m_pCInputLayout->Init(m_pCDirect3D->GetDevice(), layout, ARRAYSIZE(layout), m_pBlob_VS->GetBufferPointer(), m_pBlob_VS->GetBufferSize());

	m_pBlob_PS = CompileShader(L"PixelShader.hlsl", "psmain", "ps_5_0");
	m_pCPixelShader = new PixelShader();
	m_pCPixelShader->Init(m_pCDirect3D->GetDevice(), m_pBlob_PS);

	m_pBlob_VS->Release();
	m_pBlob_PS->Release();

	m_pCDirect3D->SetViewportSize(width, height);
}

void GraphicEngine::Frame()
{
	std::cout << "GraphicEngine" << " Class" << " Frame ȣ��" << '\n';
	
	m_pCSwapChain->ClearRenderTargetColor(m_pCDirect3D->GetDeviceContext(), 0, 0.3f, 0.4f, 1);

	m_pCInputLayout->SetInputLayout(m_pCDirect3D->GetDeviceContext());

	m_pCVertexBuffer->SetVertexBuffer(m_pCDirect3D->GetDeviceContext());

	m_pCVertexShader->SetVertexShader(m_pCDirect3D->GetDeviceContext());
	m_pCPixelShader->SetPixelShader(m_pCDirect3D->GetDeviceContext());
}

void GraphicEngine::Render()
{
	std::cout << "GraphicEngine" << " Class" << " Render ȣ��" << '\n';
	m_pCDirect3D->DrawVertex_TriangleStrip(m_pCVertexBuffer->GetCountVertices(), 0);
	m_pCSwapChain->Present(false);
}

void GraphicEngine::Release()
{
	delete m_pCInputLayout;
	m_pCInputLayout = nullptr;
	
	delete m_pCVertexBuffer;
	m_pCVertexBuffer = nullptr;

	delete m_pCSwapChain;
	m_pCSwapChain = nullptr;
	
	delete m_pCDirect3D;
	m_pCDirect3D = nullptr;

	delete m_pCVertexShader;
	m_pCVertexShader = nullptr;

	delete m_pCPixelShader;
	m_pCPixelShader = nullptr;
}

ID3DBlob* GraphicEngine::CompileShader(std::wstring shaderName, std::string entryName, std::string target)
{
	ID3DBlob* pBlob;
	ID3DBlob* errBlob;
	HRESULT hResult;
	//compile Shader
	hResult = D3DCompileFromFile(shaderName.c_str(), nullptr, nullptr, entryName.c_str(), target.c_str(), NULL, NULL, &pBlob, &errBlob);
	if (FAILED(hResult))
	{
		OutputDebugStringA((char*)errBlob->GetBufferPointer());
		if (errBlob)
			errBlob->Release();
		_ASEERTION_CREATE(hResult, "CompileShader");
	}
	return pBlob;
}


//�̱��水ü, static�������� ������Ÿ�ӿ� ��ü�� �����ȴ�
GraphicEngine& GraphicEngine::Get()
{
	static GraphicEngine engine;
	return engine;
}
