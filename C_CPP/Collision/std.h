#pragma once
//d3d Header
#include <d3d11.h>
#include <DirectXMath.h>					//XMMATRIX
#include <iostream>
using namespace DirectX;
//Macro
#define WIN32_LEAN_AND_MEAN             //거의 사용되지 않는 내용은 Windows 헤더에서 제외합니다.
#define NOMINMAX						//window의 min,max가 아닌 std의 min,max사용을위한 매크로
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#define _ASEERTION_NULCHK(result, message) assert(result && message)
#define _ASEERTION_CREATE(result, message) assert(result == S_OK && message)
#define _DEGTORAD(DEG) DEG / 180.0f * XM_PI
#define _RADTODEG(RAD) RAD / XM_PI * 180.0f
#define _EPSILON 1e-6f
#define _CLAMP(val, low, high) Clamp(val, low, high)

template<typename T>
inline const T& Clamp(const T& val, const T& low, const T& high)
{
	_ASEERTION_NULCHK(low <= high, "low larger then high!");
	if (val < low) return low;
	else if (val > high) return high;
	else return val;
}