#pragma once

//DirectX lib linking
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//d3d header include
#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
//using namespace DirectX;

//Warning C4316�ذ� ���� new, free�� 16����Ʈ������ �������ʿ�( XMMatrix, 16����Ʈ, __declspec(align(16)) )
//���������Ҵ�ɶ� 8����Ʈalign�ǹǷ� ����Ʈ���и��¹������ذ��ϱ�����
#include "AlignedAllocationPolicy.h"
