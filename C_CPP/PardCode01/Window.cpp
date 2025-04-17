#include "Window.h"

Window* gWindow = nullptr;
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		//�����������
		case WM_CREATE:
		{
			//Event When WindowCreated
			gWindow->OnCreate();
		}break;
		//������Ҹ��
		case WM_DESTROY:
		{
			//Event When WindowDestroyed
			gWindow->OnDestroy();
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
	Release();
}

bool Window::Init()
{

	//�������� gWindow�� ���� ��ü�� �����Ѵ�, �ش� ���������� WndProc���� ȣ��ǹǷ� ������ ������� �ʱ�ȭ�����ش�
	if (!gWindow)
		gWindow = this;

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
	descWnd.lpszClassName = L"PardCode01";					//â Ŭ�����̸��� ����
	
	//WNDCLASSEX ����ü �������� �����츦 ����Ѵ�, �з����Ϳ� ������ ����ü�� �ּҸ� �ѱ��, �ش� �Լ� ȣ�����ĺ��� WndProc�� �ü�������ؼ� ��� ȣ��ȴ�
	if (!::RegisterClassEx(&descWnd))
		return false;

	//������ â�� ����� �ڵ��� ������� mHwnd�� �ѱ��
	//CreateWindowExW(0L, lpClassName, lpWindowName, dwStyle, x, y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam)
	mHwnd = ::CreateWindowEx
	(
		WS_EX_OVERLAPPEDWINDOW,
		L"PardCode01",										//���� â Ŭ���� �̸��� �� ���ƾ��Ѵ�
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
	if (!mHwnd)
		return false;

	//������â������
	::ShowWindow(mHwnd, SW_SHOW);
	::UpdateWindow(mHwnd);
	return mIsRun = true;
}

bool Window::BroadCast()
{
	MSG msg;
	while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	OnUpdate();
	Sleep(0);
	return true;
}

bool Window::Release()
{
	return ::DestroyWindow(mHwnd);
}

bool Window::IsRun()
{
	return mIsRun;
}
