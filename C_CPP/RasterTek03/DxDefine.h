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

//Warning C4316해결 위해 new, free를 16바이트에따른 정렬이필요( XMMatrix, 16바이트, __declspec(align(16)) )
//힙에동적할당될때 8바이트align되므로 바이트가밀리는문제를해결하기위함
#include "AlignedAllocationPolicy.h"
