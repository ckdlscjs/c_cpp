#include "Window.h"

Window* g_pWindow = nullptr;
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		//�����������
		case WM_CREATE:
		{
			//Event When WindowCreated
			//g_pWindow->SetHWND(hWnd);
			//g_pWindow->OnCreate();
		}break;
		//������Ҹ��
		case WM_DESTROY:
		{
			//Event When WindowDestroyed
			g_pWindow->Release();
			::PostQuitMessage(0);
		}break;

		default:
		{
			return ::DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}
}

Window::Window()
{
	std::cout << "Window" << " Class" << " ������ ȣ��" << '\n';
}

Window::~Window()
{
	std::cout << "Window" << " Class" << "�Ҹ��� ȣ��" << '\n';
}

bool Window::Init()
{

	//�������� gWindow�� ���� ��ü�� �����Ѵ�, �ش� ���������� WndProc���� ȣ��ǹǷ� ������ ������� �ʱ�ȭ�����ش�
	if (!g_pWindow)
		g_pWindow = this;

	//Window�� �������� WNDCLASSEX����ü�� �ʱ�ȭ
	WNDCLASSEX descWnd;
	ZeroMemory(&descWnd, sizeof(WNDCLASSEX));
	descWnd.cbSize = sizeof(WNDCLASSEX);					//�ش籸��ü�� ũ��
	descWnd.style = NULL;									//Ŭ������Ÿ��
	descWnd.lpfnWndProc = &WndProc;							//â ���ν��������� ������, �ش��Լ��� ���ؼ� ������ â������ �Է��� ����
	descWnd.cbClsExtra = NULL;								//Ŭ������ �Ҵ��� �߰� ����Ʈ �⺻�� 0
	descWnd.cbWndExtra = NULL;								//�����쿡 �Ҵ��� �߰� ����Ʈ �⺻�� 0
	descWnd.hInstance = NULL;								//�ش� �ν��Ͻ��� ���� �ڵ�
	descWnd.hIcon = LoadIcon(NULL, IDI_APPLICATION);		//Ŭ���� ������, 1)�ν��Ͻ��ڵ�, 2)�ش����� NULL�̸� �ý��ۿ��� �⺻ �������� ����
	descWnd.hCursor = LoadCursor(NULL, IDC_ARROW);			//���콺 ������, 1)�ν��Ͻ��ڵ�, 2)�ش����� NULL�̸� �ý��ۿ��� �⺻ �������� ����
	descWnd.hIconSm = LoadIcon(NULL, IDI_APPLICATION);		//���� ������, 1)�ν��Ͻ��ڵ�, 2)�ش����� NULL�̸� �ý��ۿ��� �⺻ �������� ����
	descWnd.hbrBackground = (HBRUSH)COLOR_WINDOW;			//Ŭ���� ��� �귯�ÿ� ���� �ڵ�
	descWnd.lpszMenuName = L"";								//Ŭ���� �޴��� ���ҽ��̸��� ����, NULL�Ͻ� �ش�Ŭ�������� �⺻�޴�������
	descWnd.lpszClassName = L"PardCode";					//â Ŭ�����̸��� ����
	
	//WNDCLASSEX ����ü �������� �����츦 ����Ѵ�, �з����Ϳ� ������ ����ü�� �ּҸ� �ѱ��, �ش� �Լ� ȣ�����ĺ��� WndProc�� �ü�������ؼ� ��� ȣ��ȴ�
	if (!::RegisterClassEx(&descWnd))
		return false;

	//������ â�� ����� �ڵ��� ������� mHwnd�� �ѱ��
	//CreateWindowExW(0L, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)
	m_hWnd = ::CreateWindowEx
	(
		WS_EX_OVERLAPPEDWINDOW,
		L"PardCode",										//���� â Ŭ���� �̸��� �� ���ƾ��Ѵ�
		L"WIN32API WINDOW",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		1024,
		768,
		NULL,
		NULL,
		NULL,
		NULL
	);
	if (!m_hWnd)
		return false;
	//������â������
	::ShowWindow(m_hWnd, SW_SHOW);
	::UpdateWindow(m_hWnd);
	RECT rect = GetClientWindowRect();
	m_iWidth = rect.right - rect.left;
	m_iHeight = rect.bottom - rect.top;
	g_pWindow->OnCreate();
	return m_bIsRun = true;
}

bool Window::BroadCast()
{
	MSG msg;
	if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	else
	{
		OnUpdate();
		Sleep(0);
	}
	return true;
}

bool Window::Release()
{
	g_pWindow->OnDestroy();
	return ::DestroyWindow(m_hWnd);
}

bool Window::IsRun()
{
	return m_bIsRun;
}

void Window::SetHWND(HWND hWnd)
{
	m_hWnd = hWnd;
}

HWND Window::GetHwnd() const
{
	return m_hWnd;
}

RECT Window::GetClientWindowRect()
{
	RECT rt;
	::GetClientRect(m_hWnd, &rt);
	return rt;
}
