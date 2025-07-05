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
	//������������ ������ü ���ù� VertexBuffer����
	Vertex_PPCC vertices[] =
	{
		{{-0.5f, -0.5f, 0.0f},	{-0.32f, -0.11f, 0.0f},		{1, 0, 0, 1.0f},	{0, 1, 0, 1.0f},},
		{{-0.5f, 0.5f, 0.0f},	{-0.11f, 0.78f, 0.0f},		{0, 1, 0, 1.0f},	{1, 1, 0, 1.0f},},
		{{0.5f, -0.5, 0.0f},	{0.75f, -0.73f, 0.0f},		{0, 0, 1, 1.0f},	{1, 0, 0, 1.0f},},
		{{0.5f, 0.5f, 0.0f},	{0.88f, 0.77f, 0.0f},		{1, 1, 0, 1.0f},	{0, 0, 1, 1.0f},},
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
	GraphicEngine::Get().CreateVertexBuffer(vertices, sizeof(Vertex_PPCC), ARRAYSIZE(vertices));
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
