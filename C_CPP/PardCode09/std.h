#pragma once

//Macro
#define _ASEERTION_NULLCHK(result, message) assert(result && message)
#define _ASEERTION_CREATE(result, message) assert(result == S_OK && message)

//App Header
#include <cstdlib>
#include <crtdbg.h>
#include <Windows.h>
#include <string>
#include <iostream>

//d3d Header
#include <d3d11.h>
#include <DirectXMath.h>					//XMMATRIX
#include <d3dcompiler.h>					//d3dcomplie, ���̴�����������߰�
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")		//d3dcomplie, ���̴�����������߰�

using namespace DirectX;

struct Vertex_PC
{
	XMFLOAT3 pos;
	XMFLOAT4 color;
};

//16����Ʈ ������ gpu�޸𸮿��� �е��ǹǷ� ������ �����
__declspec(align(16))
struct Constant_time
{
	float fTime;
};

struct Vertex_PPCC
{
	XMFLOAT3 pos0;
	XMFLOAT3 pos1;
	XMFLOAT4 color0;
	XMFLOAT4 color1;
};
