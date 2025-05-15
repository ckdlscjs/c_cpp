#include "GraphicEngine.h"
#include "Window.h"
#include "Direct3D.h"
#include "SwapChain.h"
#include "VertexBuffer.h"
#include "InputLayout.h"
#include "VertexShader.h"
#include "PixelShader.h"
#include "ConstantBuffer.h"

GraphicEngine::GraphicEngine()
{
	std::cout << "GraphicEngine" << " Class" << " 생성자 호출" << '\n';
}

GraphicEngine::~GraphicEngine()
{
	std::cout << "GraphicEngine" << " Class" << "소멸자 호출" << '\n';
	Release();
}

void GraphicEngine::Init(HWND hWnd, UINT width, UINT height)
{
	m_pCDirect3D = new Direct3D();
	m_pCDirect3D->Init();

	m_pCSwapChain = new SwapChain();
	m_pCSwapChain->Init(m_pCDirect3D->GetDevice(), hWnd, width, height);

	//렌더링을위한 정점객체 세팅및 VertexBuffer생성
	Vertex_PPCC vertices[] =
	{
		{{-0.5f, -0.5f, 0.0f}, {-0.32f, -0.11f, 0.0f}, { 1, 0, 0, 1 }, {0, 1, 0, 1}},
		{{-0.5f, 0.5f, 0.0f}, {-0.11f, 0.78f, 0.0f}, { 0, 1, 0, 0.1f }, {1, 1, 0, 1},},
		{{0.5f, -0.5, 0.0f}, {0.75f, -0.73f, 0.0f}, { 0, 0, 1, 0.1f }, {1, 0, 0, 1},},
		{{0.5f, 0.5f, 0.0f}, {0.88f, 0.77f, 0.0f}, {1, 1, 0, 0.1f}, {0, 0, 1, 1},},
	};
	//입력배치 객체 생성 및 초기화
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"POSITION", 1, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	
	UINT size_vertices = ARRAYSIZE(vertices);
	m_pCVertexBuffer = new VertexBuffer();
	m_pCVertexBuffer->Init(m_pCDirect3D->GetDevice(), vertices, sizeof(Vertex_PPCC), size_vertices);

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

	Constant_time cc;
	cc.fTime = 0;
	m_pCConstantBuffer = new ConstantBuffer();
	m_pCConstantBuffer->Init(m_pCDirect3D->GetDevice(), &cc, sizeof(Constant_time));

	m_pCDirect3D->SetViewportSize(width, height);
}

void GraphicEngine::Frame()
{
	std::cout << "GraphicEngine" << " Class" << " Frame 호출" << '\n';
	
	m_pCSwapChain->ClearRenderTargetColor(m_pCDirect3D->GetDeviceContext(), 0, 0.3f, 0.4f, 1);

	m_pCInputLayout->SetInputLayout(m_pCDirect3D->GetDeviceContext());

	m_pCVertexBuffer->SetVertexBuffer(m_pCDirect3D->GetDeviceContext());

	m_pCVertexShader->SetVertexShader(m_pCDirect3D->GetDeviceContext());
	m_pCPixelShader->SetPixelShader(m_pCDirect3D->GetDeviceContext());

	unsigned long new_time = 0;
	if (m_lOldtime)
		new_time = ::GetTickCount() - m_lOldtime;
	m_fDeltatime = new_time * 0.001f;
	m_lOldtime = ::GetTickCount();
	m_fAngle += 1.57f * m_fDeltatime;

	Constant_time cc;
	cc.fTime = m_fAngle;
	m_pCConstantBuffer->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc);
	m_pCConstantBuffer->SetVS(m_pCDirect3D->GetDeviceContext());
	m_pCConstantBuffer->SetPS(m_pCDirect3D->GetDeviceContext());
}

void GraphicEngine::Render()
{
	std::cout << "GraphicEngine" << " Class" << " Render 호출" << '\n';
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

	delete m_pCConstantBuffer;
	m_pCConstantBuffer = nullptr;
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


//싱글톤객체, static선언으로 컴파일타임에 객체가 생성된다
GraphicEngine& GraphicEngine::Get()
{
	static GraphicEngine engine;
	return engine;
}
