#pragma once

// SDKDDKVer.h�� �����ϸ� �ְ� ������ ���뼺�� ���� Windows �÷����� ���ǵ˴ϴ�.

// ���� Windows �÷����� ���� ���� ���α׷��� �����Ϸ��� ��쿡�� SDKDDKVer.h�� �����ϱ� ����
// WinSDKVer.h�� �����ϰ� _WIN32_WINNT ��ũ�θ� �����Ϸ��� �÷������� �����Ͻʽÿ�.

#include <SDKDDKVer.h> //#include targetver.h�ǳ���

//ǥ�ؽý�����������, ������Ʈ���� ���������� ����ִ� �������

#define WIN32_LEAN_AND_MEAN

//windows �������
#include <Windows.h>

//C�� �������
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//���α׷��� �ʿ��� �߰�������� ���⼭���� �߰��Ѵ�
#include "DxDefine.h"