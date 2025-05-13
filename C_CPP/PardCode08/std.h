#pragma once

//Macro
#define _ASEERTION_NULLCHK(result, message) assert(result != nullptr && message)
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
#include <d3dcompiler.h>					//d3dcomplie, 쉐이더사용을위한추가
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")		//d3dcomplie, 쉐이더사용을위한추가

using namespace DirectX;
