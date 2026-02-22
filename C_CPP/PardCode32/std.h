#pragma once
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
#define VK_MOUSE_MOVE 255
//App Header
#include <cstdlib>
#include <crtdbg.h>
#include <Windows.h>
#include <windowsx.h>						// GET_X_LPARAM, GET_Y_LPARAM 매크로 사용을 위해 권장
#include <string>
#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>
#include <functional>
#include <codecvt>							//need to w to m, m to w
#include <random>							//랜덤난수생성을위함(mt19937)
#include <ctime>							//랜덤난수생성을위함(mt19937)
#include <typeindex>						//std::type_index
#include <array>
#include <fstream>
#include <bitset>

//d3d Header
#include <d3d11.h>
#include <DirectXMath.h>					//XMMATRIX
#include <d3dcompiler.h>					//d3dcomplie, 쉐이더사용을위한추가
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")		//d3dcomplie, 쉐이더사용을위한추가

#include <directXTex/DirectXTex.h>			
#pragma comment(lib, "DirectXTex.lib")

#include <directXTK/DDSTextureLoader.h>
#include <directXTK/WICTextureLoader.h>
#pragma comment(lib, "DirectXTK.lib")


//assimp
#pragma comment(lib, "assimp-vc145-mtd.lib")
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/version.h>

using namespace DirectX;

//#include "bits/stdc++.h"

//공용함수들
template<typename T>
inline const T& Clamp(const T& val, const T& low, const T& high)
{
	_ASEERTION_NULCHK(low <= high, "low larger then high!");
	if (val < low) return low;
	else if (val > high) return high;
	else return val;
}

inline std::wstring _tomw(std::string str)
{
	std::wstring content;
	content.assign(str.begin(), str.end());
	return content;
}

inline std::string _towm(std::wstring wstr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(wstr);
}

inline size_t Hasing_wstring(const std::wstring& str)
{
	// FNV-1a (64비트) 구현 예시
	size_t hash = 14695981039346656037ULL; // FNV_PRIME_64
	for (wchar_t c : str) {
		hash ^= static_cast<size_t>(c);
		hash *= 1099511628211ULL; // FNV_OFFSET_64
	}
	return hash;
}
inline size_t Hasing_float(const float& value)
{
	// FNV-1a (64비트) 구현 예시
	size_t hash = 14695981039346656037ULL; // FNV_PRIME_64
	hash ^= std::hash<float>{}(value);
	hash *= 1099511628211ULL; // FNV_OFFSET_64
	return hash;
}

inline void hash_combine(std::size_t& seed, std::size_t hashValue) {
	seed ^= hashValue + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}



