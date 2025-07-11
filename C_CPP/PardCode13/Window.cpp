#include "Window.h"
#include "InputSystem.h"

Window* g_pWindow = nullptr;
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	/*
	WPARAM wParam (Word Parameter):
	Ÿ��: WPARAM (Word Parameter)
	����: �޽����� Ư���� �߰� ������ ��� �Ű������Դϴ�. "Word"�� ���� 16��Ʈ �����͸� �ǹ�������, ����� �ý��� ��Ű��ó�� ���� 32��Ʈ �Ǵ� 64��Ʈ ���� ���� ���� �� �ֽ��ϴ� (���� UINT_PTR�� ���ǵ�).
	�뵵: � �޽����Ŀ� ���� �� �ǹ̰� �޶����ϴ�.

	Ű���� �޽��� (WM_KEYDOWN, WM_KEYUP): �����ų� ������ Ű�� ���� Ű �ڵ� (Virtual Key Code)�� ����ϴ� (��: VK_SPACE, VK_A).
	���콺 �޽��� (WM_MOUSEMOVE): ���� ���� �ִ� ���콺 ��ư �� Shift/Ctrl Ű�� ���¸� ��Ʈ �÷��׷� ����ϴ� (��: MK_LBUTTON, MK_SHIFT).
	������ ũ�� ���� �޽��� (WM_SIZE): �����찡 ��� ũ�� ����Ǿ����� (�ּ�ȭ, �ִ�ȭ, �Ϲ� ũ��)�� ���� ������ ����ϴ�.
	���콺 �� �޽��� (WM_MOUSEWHEEL): ���콺 ���� ȸ����(��Ÿ)�� ����ϴ�.

	LPARAM lParam (Long Parameter):
	Ÿ��: LPARAM (Long Parameter)
	����: wParam�� ���������� �޽����� Ư���� �߰� ������ ��� �Ű������Դϴ�. "Long"�� ���� 32��Ʈ �����͸� �ǹ�������, ����� �ý��� ��Ű��ó�� ���� 32��Ʈ �Ǵ� 64��Ʈ ���� ���� ���� �� �ֽ��ϴ� (���� LONG_PTR�� ���ǵ�).
	�뵵: wParam�� ���������� �޽����� ���� �� �ǹ̰� �޶����ϴ�.

	Ű���� �޽���: Ű�� �ݺ� Ƚ��, ��ĵ �ڵ�, Ȯ�� Ű �÷��� �� �� ���� Ű���� ���� ������ ��Ʈ �ʵ�� ����ϴ�.
	���콺 �޽���: ���콺 Ŀ���� X, Y ��ǥ�� ����ϴ�. LOWORD(lParam)���� X��ǥ, HIWORD(lParam)���� Y��ǥ�� ����ϴ�. (�Ϻ� �޽����� Ŭ���̾�Ʈ ��ǥ, �Ϻδ� ȭ�� ��ǥ).
	������ ũ�� ���� �޽���: ����� �������� ���ο� �ʺ�� ���̸� ����ϴ�.
	*/
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

		//Ű�������
		case WM_KEYDOWN:
		{
			_InputSystem.OnKeyDown(wParam);
		}break;

		case WM_KEYUP:
		{
			_InputSystem.OnKeyUp(wParam);
		}break;

		//���콺����
		case WM_MOUSEMOVE:
		{
			//�»��0,0~���ϴ�����
			_InputSystem.OnMouseMove(LOWORD(lParam), HIWORD(lParam));
		}break;

		/*
		case WM_LBUTTONDOWN:
		{
			std::cout << "btn";
		}break;

		case WM_LBUTTONUP:
		{
			std::cout << "btn";
		}break;

		case WM_RBUTTONDOWN:
		{
			std::cout << "btn";
		}break;

		case WM_RBUTTONUP:
		{
			std::cout << "btn";
		}break;
		
		case WM_SETFOCUS:
		{
		}break;
		case WM_KILLFOCUS:
		{
		}break;
		*/
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
	HINSTANCE hInstance = GetModuleHandle(NULL);
	//Window�� �������� WNDCLASSEX����ü�� �ʱ�ȭ
	WNDCLASSEX descWnd;
	ZeroMemory(&descWnd, sizeof(WNDCLASSEX));
	descWnd.cbSize = sizeof(WNDCLASSEX);					//�ش籸��ü�� ũ��
	descWnd.style = NULL;									//Ŭ������Ÿ��
	descWnd.lpfnWndProc = &WndProc;							//â ���ν��������� ������, �ش��Լ��� ���ؼ� ������ â������ �Է��� ����
	descWnd.cbClsExtra = NULL;								//Ŭ������ �Ҵ��� �߰� ����Ʈ �⺻�� 0
	descWnd.cbWndExtra = NULL;								//�����쿡 �Ҵ��� �߰� ����Ʈ �⺻�� 0
	descWnd.hInstance = hInstance;							//�ش� �ν��Ͻ��� ���� �ڵ�
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
		hInstance,
		NULL
	);
	if (!m_hWnd)
		return false;
	//������â������
	::ShowWindow(m_hWnd, SW_SHOW);
	::UpdateWindow(m_hWnd);
	::SetFocus(m_hWnd);
	RECT rect = GetClientWindowRect();
	m_iWidth = rect.right - rect.left;
	m_iHeight = rect.bottom - rect.top;
	g_pWindow->OnCreate();
	return m_bIsRun = true;
}

bool Window::BroadCast()
{
	MSG msg;
	memset(&msg, 0, sizeof(MSG));
	if (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) > 0)
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
