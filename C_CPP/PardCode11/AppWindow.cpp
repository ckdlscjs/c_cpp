#include "AppWindow.h"
#include "GraphicEngine.h"

AppWindow::AppWindow()
{
	std::cout << "AppWindow" << " Class" << " ������ ȣ��" << '\n';
}

AppWindow::~AppWindow()
{
	std::cout << "AppWindow" << " Class" << " �Ҹ��� ȣ��" << '\n';
}

void AppWindow::OnCreate()
{
	std::cout << "OnCreate" << '\n';
	GraphicEngine::Get().Init(m_hWnd, m_iWidth, m_iHeight);

	//������������ ������ü
	Vertex_PC vertices[] =
	{
		{{-0.5f, 0.5f, -0.5f}		,{1, 0, 0, 1.0f}},
		{{0.5f, 0.5f, -0.5f}		,{0, 1, 0, 1.0f}},
		{{-0.5f, -0.5f, -0.5f}		,{0, 0, 1, 1.0f}},
		{{0.5f, -0.5f, -0.5f}		,{1, 1, 0, 1.0f}},
		{{-0.5f, 0.5f, 0.5f}		,{1, 0, 0, 1.0f}},
		{{0.5f, 0.5f, 0.5f}			,{0, 1, 0, 1.0f}},
		{{-0.5f, -0.5f, 0.5f}		,{0, 0, 1, 1.0f}},
		{{0.5f, -0.5f, 0.5f}		,{1, 1, 0, 1.0f}},
	};
	//�ε���������������
	UINT indices[] =
	{
		//front
		0,1,2,
		2,1,3,
		
		//right
		1,5,3,
		3,5,7,

		//back
		5,4,7,
		7,4,6,

		//left
		4,0,6,
		6,0,2,

		//up
		4,5,0,
		0,5,1,

		//bottom
		2,3,6,
		6,3,7,
	};
	//�Է¹�ġ ��ü ���� �� �ʱ�ȭ
	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		//SEMANTIC NAME, SEMANTIC INDEX, FORMAT, INPUT SLOT, ALIGNED BYTE OFFSET, INPUT SLOT CLASS, INSTANCE DATA STEP RATE, 
		{"POSITION",	0, DXGI_FORMAT_R32G32B32_FLOAT,		0, 0,	D3D11_INPUT_PER_VERTEX_DATA, 0},
		{"COLOR",		0, DXGI_FORMAT_R32G32B32A32_FLOAT,	0, 12,	D3D11_INPUT_PER_VERTEX_DATA, 0},
	};
	GraphicEngine::Get().CreateVertexBuffer(vertices, sizeof(Vertex_PC), ARRAYSIZE(vertices));
	GraphicEngine::Get().CreateIndexBuffer(indices, ARRAYSIZE(indices));
	GraphicEngine::Get().CreateVertexShader(L"VertexShader.hlsl", "vsmain", "vs_5_0");
	GraphicEngine::Get().CreatePixelShader(L"PixelShader.hlsl", "psmain", "ps_5_0");
	GraphicEngine::Get().CreateInputLayout(layout, ARRAYSIZE(layout));
	GraphicEngine::Get().CreateConstantBuffer();
}

void AppWindow::OnUpdate()
{
	std::cout << "OnUpdate" << '\n';
	GraphicEngine::Get().Frame();
	GraphicEngine::Get().Render();
}

void AppWindow::OnDestroy()
{
	std::cout << "OnDestroy" << '\n';
	//GraphicEngine::Get().Release();
	m_bIsRun = false;
}
