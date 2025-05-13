#include "stdafx.h"
#include "SystemClass.h"
#include "InputClass.h"
#include "GraphicsClass.h"

SystemClass::SystemClass()
{
	
}

SystemClass::SystemClass(const SystemClass& _systemClass)
{
}

SystemClass::~SystemClass()
{
}

bool SystemClass::Initialize()
{
	//�������� ����, ���� ���� �ʱ�ȭ
	int screenWidth = 0;
	int screenHeight = 0;

	//������ ����, �ʱ�ȭ �� ����(ShowWindow�Լ�)
	InitializeWindows(screenWidth, screenHeight);

	//�Է�Ŭ���� ��ü ���� �� �ʱ�ȭ
	m_Input = new InputClass;
	if (!m_Input) return false;
	m_Input->Initialize();

	//�׷���Ŭ���� ��ü ���� �� �ʱ�ȭ
	m_Graphics = new GraphicsClass;
	if (!m_Graphics) return false;
	
	return m_Graphics->Initialize(screenWidth, screenHeight, m_hwnd);
}

void SystemClass::Shutdown()
{
	// m_Graphics ��ü ��ȯ
	if (m_Graphics)
	{
		m_Graphics->Shutdown();
		delete m_Graphics;
		m_Graphics = nullptr;
	}

	// m_Input ��ü ��ȯ
	if (m_Input)
	{
		delete m_Input;
		m_Input = nullptr;
	}

	// Window ���� ó��
	ShutdownWindows();
}

void SystemClass::Run()
{
	// �޽��� ����ü ���� �� �ʱ�ȭ
	MSG msg;
	memset(&msg, 0, sizeof(MSG));

	// ����޽����� ���� �� ���� �޽����� �����Ѵ�
	while (true)
	{
		// ������ �޽����� ó���Ѵ�
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// �޽����� ����޽����ϰ�� �����Ѵ�
			if (msg.message == WM_QUIT) break;
			// ������ �޽��� ť�� �����Ͱ� WM_QUIT�̿��� �ٸ� �Էµ��� �������� �����Ѵ�
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			// �޽������ͷ�Ʈ�� �߻����� ������ frame�� ����, �� �޽��� ť�� ������ ����ȴ�(���ӷ���, �浹, ������ ��..)
			if (!Frame()) break;
		}
	}
}

LRESULT SystemClass::MessageHandler(HWND hwnd, UINT umsg, WPARAM wparam, LPARAM lparam)
{
	switch (umsg)
	{
		// Ű���尡 ���ȴٸ�
		case WM_KEYDOWN :
		{
			m_Input->KeyDown(wparam);
			return 0;
		}
		
		// Ű���尡 �������°� ó��
		case WM_KEYUP:
		{
			m_Input->KeyUp(wparam);
			return 0;
		}

		//�̿��� �޽����� �⺻�޽��� ó���� �ѱ��
		default :
		{
			return DefWindowProc(hwnd, umsg, wparam, lparam);
		}
	}
}

bool SystemClass::Frame()
{
	//ESC�� ������ ����
	if(m_Input->IsKeyDown(VK_ESCAPE))
		return false;
	return m_Graphics->Frame();
}

void SystemClass::InitializeWindows(int& _screenWidth, int& _screenHeight)
{
	// ������ü �ڵ��� ���� ��ü�� �����Ѵ�
	ApplicationHandle = this;

	// �ش� ���α׷��� �ν��Ͻ��� ������ �����Ѵ�
	m_hInstance = GetModuleHandle(NULL);

	// �ش� ���α׷��� �̸��� �����Ѵ�
	m_applicationName = L"RastekTek02";

	// windows Ŭ������ �����Ѵ�
	WNDCLASSEX wc;
	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;				// â ��Ÿ��, ����/���� �������� �� �ٽ� �׸���, ���� DC ���
	wc.lpfnWndProc = WndProc;									// ������ �޽����� ó���� �Լ� ����, ���������Լ� WndProc�� ���
	wc.cbClsExtra = 0;											// Ŭ���� �޸𸮿� ������ �޸��� �߰� ���� (��� �� ��)
	wc.cbWndExtra = 0;
	wc.hInstance = m_hInstance;									// ���� ���� ���� �ν��Ͻ� �ڵ�
	wc.hIcon = LoadIcon(NULL, IDI_WINLOGO);						// ������ ���� (������ ���� ��� �� Alt+Tab ������)
	wc.hIconSm = wc.hIcon;				
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);					// �⺻ ���콺 Ŀ�� ���� (ȭ��ǥ)
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);		// â�� ��� �귯�� (������)
	wc.lpszMenuName = NULL;										// �޴� ���ҽ� �̸� (����)
	wc.lpszClassName = m_applicationName;						// ������ Ŭ���� �̸� (��� �� ���)
	wc.cbSize = sizeof(WNDCLASSEX);								// ����ü��ũ�⼳��

	// windows class�� ����մϴ�
	RegisterClassEx(&wc);

	// ����� ȭ���� �ػ󵵸� �о�� ������ �ѱ� ������ ����� �Ѱ��ش�
	_screenWidth = GetSystemMetrics(SM_CXSCREEN);
	_screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// �������� ���� ��ġ(������)
	int posX = 0;
	int posY = 0;

	// GraphicsClass���� ���������� ����Ǿ��ִ� FULL_SCREEN������ ���¿� ���� ȭ���� �����Ѵ�
	if (FULL_SCREEN)
	{
		// ��üȭ�� ���¶�� â ����� ����ũ�� �ػ󵵷� �����ϰ� ������ 32BIT���� �����Ѵ�
		DEVMODE dmScreenSettings;
		memset(&dmScreenSettings, 0, sizeof(dmScreenSettings));						//same zeromemory
		dmScreenSettings.dmSize = sizeof(dmScreenSettings);							//����ü�� ũ�⼳��
		dmScreenSettings.dmPelsWidth = (unsigned long)_screenWidth;			
		dmScreenSettings.dmPelsHeight = (unsigned long)_screenHeight;
		dmScreenSettings.dmBitsPerPel = 32;
		dmScreenSettings.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT;

		ChangeDisplaySettings(&dmScreenSettings, CDS_FULLSCREEN);
	}
	else
	{
		// ��üȭ�� ���°� �ƴҰ�� 800*600�ػ󵵷� �������ش�
		_screenWidth = 800;
		_screenHeight = 600;

		// �����찡 ����� �� �� �ֵ��� �������� ������ ��ġ�� â ũ��(800, 600)�� ����� ���������ġ�������Ѵ�
		posX = (GetSystemMetrics(SM_CXSCREEN) - _screenWidth) / 2;	//(1920 - 800) / 2; 
		posY = (GetSystemMetrics(SM_CYSCREEN) - _screenHeight) / 2; //(1080 - 600) / 2;

		// �����찴ü�� �����ϰ� �ڵ��� ���Ѵ�
		m_hwnd = CreateWindowEx
		(
			WS_EX_APPWINDOW,									// Ȯ�� ��Ÿ��
			m_applicationName,									// ��ϵ� Ŭ���� �̸�
			m_applicationName,									// ������ �̸�
			WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_POPUP,		// ������ ��Ÿ��
			posX,												//���� ���
			posY, 
			_screenWidth,										// â ũ��
			_screenHeight, 
			NULL,												// �θ� ������ �ڵ�(������ null)
			NULL,												// �޴� �ڵ�(������ null)
			m_hInstance,										// ���� ���ø����̼� �ν��Ͻ� �ڵ�
			NULL												// �߰� ������(�Ϲ������� null)
		);

		// �����츦 ȭ�鿡 ǥ���ϰ� ��Ŀ���� �����մϴ�
		ShowWindow(m_hwnd, SW_SHOW);							// �����츦 �����ϰ� �Ѱܹ��� �ڵ�(�����찴ü)�� ����
		SetForegroundWindow(m_hwnd);
		SetFocus(m_hwnd);
	}
}

void SystemClass::ShutdownWindows()
{
	// ��üȭ�� ��忴���� ������ �ʱ�ȭ��Ų��
	if (FULL_SCREEN)
	{
		ChangeDisplaySettings(NULL, 0);
	}

	// ��� â�� �����Ѵ�
	DestroyWindow(m_hwnd);
	m_hwnd = NULL;

	// ���α׷� �ν��Ͻ��� �����Ѵ�
	UnregisterClass(m_applicationName, m_hInstance);
	m_hInstance = NULL;

	// ������ü�� ������ ������ �ʱ�ȭ�Ѵ�
	ApplicationHandle = NULL;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch(umessage)
	{
		// ������ ���Ḧ Ȯ���մϴ�
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		// �����찡 �������� Ȯ���մϴ�
		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		// �� ���� ��� �޽������� �ý��� Ŭ������ �޽��� ó���� �ѱ�ϴ�.
		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, umessage, wparam, lparam);
		}
	}
}
