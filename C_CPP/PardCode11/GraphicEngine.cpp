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
	std::cout << "GraphicEngine" << " Class" << " ������ ȣ��" << '\n';
}

GraphicEngine::~GraphicEngine()
{
	std::cout << "GraphicEngine" << " Class" << "�Ҹ��� ȣ��" << '\n';
	Release();
}

void GraphicEngine::Init(HWND hWnd, UINT width, UINT height)
{
	m_fWidth = width;
	m_fHeight = height;
	m_pCDirect3D = new Direct3D();
	m_pCDirect3D->Init();

	m_pCSwapChain = new SwapChain();
	m_pCSwapChain->Init(m_pCDirect3D->GetDevice(), hWnd, width, height);

	/*
	//������������ ������ü ���ù� VertexBuffer����
	Vertex_PPCC vertices[] =
	{
		{{-0.5f, -0.5f, 0.0f},	{-0.32f, -0.11f, 0.0f},		{ 1, 0, 0, 1},		{0, 1, 0, 1},},
		{{-0.5f, 0.5f, 0.0f},	{-0.11f, 0.78f, 0.0f},		{ 0, 1, 0, 0.1f},	{1, 1, 0, 1},},
		{{0.5f, -0.5, 0.0f},	{0.75f, -0.73f, 0.0f},		{ 0, 0, 1, 0.1f},	{1, 0, 0, 1},},
		{{0.5f, 0.5f, 0.0f},	{0.88f, 0.77f, 0.0f},		{1, 1, 0, 0.1f},	{0, 0, 1, 1},},
	};
	//�Է¹�ġ ��ü ���� �� �ʱ�ȭ
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"POSITION",	1, DXGI_FORMAT_R32G32B32_FLOAT,		0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 24,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		1, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 40,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	UINT size_vertices = ARRAYSIZE(vertices);
	m_pCVertexBuffer = new VertexBuffer();
	m_pCVertexBuffer->Init(m_pCDirect3D->GetDevice(), vertices, sizeof(Vertex_PPCC), size_vertices);
	
	m_pCInputLayout = new InputLayout();
	m_pCInputLayout->Init(m_pCDirect3D->GetDevice(), layout, ARRAYSIZE(layout), m_pBlob_VS->GetBufferPointer(), m_pBlob_VS->GetBufferSize());

	m_pBlob_VS = CompileShader(L"VertexShader.hlsl", "vsmain", "vs_5_0");
	m_pCVertexShader = new VertexShader();
	m_pCVertexShader->Init(m_pCDirect3D->GetDevice(), m_pBlob_VS);
	m_pBlob_VS->Release();

	m_pBlob_PS = CompileShader(L"PixelShader.hlsl", "psmain", "ps_5_0");
	m_pCPixelShader = new PixelShader();
	m_pCPixelShader->Init(m_pCDirect3D->GetDevice(), m_pBlob_PS);
	m_pBlob_PS->Release();

	Constant_time cc;
	cc.fTime = 0;
	m_pCConstantBuffer = new ConstantBuffer();
	m_pCConstantBuffer->Init(m_pCDirect3D->GetDevice(), &cc, sizeof(Constant_time));
	*/
	
	m_pCDirect3D->SetViewportSize(width, height);
}

void GraphicEngine::Frame()
{
	std::cout << "GraphicEngine" << " Class" << " Frame ȣ��" << '\n';
	//RTV�ʱ�ȭ
	m_pCSwapChain->ClearRenderTargetColor(m_pCDirect3D->GetDeviceContext(), 0, 0.3f, 0.4f, 1);

	m_pCInputLayout->SetInputLayout(m_pCDirect3D->GetDeviceContext());

	m_pCVertexBuffer->SetVertexBuffer(m_pCDirect3D->GetDeviceContext());

	m_pCVertexShader->SetVertexShader(m_pCDirect3D->GetDeviceContext());
	m_pCPixelShader->SetPixelShader(m_pCDirect3D->GetDeviceContext());


	/*10��, �簢���� �ð���������� ���º�ȯ������ ���*/
	Constant_WVP cc0;
	Constant_time cc1;
	cc1.fTime = ::GetTickCount();
	m_fDeltaPos += m_fDeltatime;
	if (m_fDeltaPos > 1.0f) m_fDeltaPos = 0.0f;
	m_fDeltaScale += m_fDeltatime / 0.15f;
	//���~Ȯ��, ��������
	XMMATRIX matScale = GetMat_Scale(lerp(XMFLOAT3(0.5f*400.0f, 0.5f*400.0f, 0.0f), XMFLOAT3(1.0f*400.0f, 1.0f*400.0f, 0.0f), 0.5f * (sinf(m_fDeltaScale) + 1.0f)));
	//�̵�, ��������
	XMMATRIX matTrans = GetMat_Translation(lerp(XMFLOAT3(-1.5f*400.0f, -1.5f*400.0f, 0), XMFLOAT3(1.5f*400.0f, 1.5f*400.0f, 0), m_fDeltaPos));
	//��ȯ���, ��� ������ ������ ��������������Ѵ�
	cc0.matWorld = matScale * matTrans;		
	cc0.matView = GetMat_Identity();
	cc0.matProj = GetMat_Ortho(m_fWidth, m_fHeight, -4.0f, 4.0f);
	//������ۿ� cc0(wvp mat), cc1(�ð�) �� �����Ѵ�
	m_pCConstantBuffers[0]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc0);
	m_pCConstantBuffers[0]->SetVS(m_pCDirect3D->GetDeviceContext(), 0);
	//m_pCConstantBuffers[0]->SetPS(m_pCDirect3D->GetDeviceContext(), 0);

	m_pCConstantBuffers[1]->UpdateBufferData(m_pCDirect3D->GetDeviceContext(), &cc1);
	//m_pCConstantBuffers[1]->SetVS(m_pCDirect3D->GetDeviceContext(), 1);
	m_pCConstantBuffers[1]->SetPS(m_pCDirect3D->GetDeviceContext(), 1);
}

void GraphicEngine::Render()
{
	std::cout << "GraphicEngine" << " Class" << " Render ȣ��" << '\n';
	m_pCDirect3D->DrawVertex_TriangleStrip(m_pCVertexBuffer->GetCountVertices(), 0);
	m_pCSwapChain->Present(false);

	m_lOldtime = m_lNewtime;
	m_lNewtime = ::GetTickCount();
	m_fDeltatime = (m_lOldtime) ? ((m_lNewtime - m_lOldtime) / 10000.0f) : 0;
}

void GraphicEngine::Release()
{
	if (m_pCInputLayout)
	{
		delete m_pCInputLayout;
		m_pCInputLayout = nullptr;
	}
	
	if (m_pCVertexBuffer)
	{
		delete m_pCVertexBuffer;
		m_pCVertexBuffer = nullptr;
	}
	
	if (m_pCSwapChain)
	{
		delete m_pCSwapChain;
		m_pCSwapChain = nullptr;
	}
	
	if (m_pCDirect3D)
	{
		delete m_pCDirect3D;
		m_pCDirect3D = nullptr;
	}
	
	if (m_pBlob_VS)
	{
		m_pBlob_VS->Release();
	}
	if (m_pCVertexShader)
	{
		delete m_pCVertexShader;
		m_pCVertexShader = nullptr;
	}
	
	if (m_pBlob_PS)
	{
		m_pBlob_PS->Release();
	}
	if (m_pCPixelShader)
	{
		delete m_pCPixelShader;
		m_pCPixelShader = nullptr;
	}
	
	if (m_pCConstantBuffers.size())
	{
		for (int i = 0; i < m_pCConstantBuffers.size(); i++)
			delete m_pCConstantBuffers[i];
	}
}

void GraphicEngine::CreateVertexBuffer(void* vertices, UINT size_vertex, UINT size_vertices)
{
	m_pCVertexBuffer = new VertexBuffer();
	m_pCVertexBuffer->Init(m_pCDirect3D->GetDevice(), vertices, size_vertex, size_vertices);
}

void GraphicEngine::CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT size_layout)
{
	m_pCInputLayout = new InputLayout();
	m_pCInputLayout->Init(m_pCDirect3D->GetDevice(), pInputElementDescs, size_layout, m_pBlob_VS->GetBufferPointer(), m_pBlob_VS->GetBufferSize());
}


void GraphicEngine::CreateIndexBuffer()
{
}

void GraphicEngine::CreateConstantBuffer()
{
	Constant_WVP cc0;
	m_pCConstantBuffers.push_back(new ConstantBuffer());
	m_pCConstantBuffers.back()->Init(m_pCDirect3D->GetDevice(), &cc0, sizeof(Constant_WVP));

	Constant_time cc1;
	cc1.fTime = 0;
	m_pCConstantBuffers.push_back(new ConstantBuffer());
	m_pCConstantBuffers.back()->Init(m_pCDirect3D->GetDevice(), &cc1, sizeof(Constant_time));
}

void GraphicEngine::CreateVertexShader(std::wstring shaderName, std::string entryName, std::string target)
{
	if(m_pBlob_VS)
		m_pBlob_VS->Release();
	m_pBlob_VS = CompileShader(shaderName, entryName, target);
	m_pCVertexShader = new VertexShader();
	m_pCVertexShader->Init(m_pCDirect3D->GetDevice(), m_pBlob_VS);
}

void GraphicEngine::CreatePixelShader(std::wstring shaderName, std::string entryName, std::string target)
{
	if(m_pBlob_PS)
		m_pBlob_PS->Release();
	m_pBlob_PS = CompileShader(shaderName, entryName, target);
	m_pCPixelShader = new PixelShader();
	m_pCPixelShader->Init(m_pCDirect3D->GetDevice(), m_pBlob_PS);
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
