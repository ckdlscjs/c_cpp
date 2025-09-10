#pragma once
//Macro
#define WIN32_LEAN_AND_MEAN             // ���� ������ �ʴ� ������ Windows ������� �����մϴ�.
#define NOMINMAX						//	window�� min,max�� �ƴ� std�� min,max��������� ��ũ��
#define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING

#define _ASEERTION_NULCHK(result, message) assert(result && message)
#define _ASEERTION_CREATE(result, message) assert(result == S_OK && message)
#define _DEGTORAD(DEG) DEG / 180.0f * XM_PI
#define _RADTODEG(RAD) RAD / XM_PI * 180.0f
#define _EPSILON 1e-6f
#define _CLAMP(val, low, high) Clamp(val, low, high)

//App Header
#include <cstdlib>
#include <crtdbg.h>
#include <Windows.h>
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
#include <random>							//������������������(mt19937)
#include <ctime>							//������������������(mt19937)
#include <typeindex>						//std::type_index
#include <array>
#include <fstream>

//d3d Header
#include <d3d11.h>
#include <DirectXMath.h>					//XMMATRIX
#include <d3dcompiler.h>					//d3dcomplie, ���̴�����������߰�
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")		//d3dcomplie, ���̴�����������߰�

#include <DirectXTex.h>						//Include/DXTEX, �ؽ��Ļ���������߰�
#pragma comment(lib, "DirectXTex.lib")

using namespace DirectX;

//#include "bits/stdc++.h"
#include "CommonMath.h"

//�����Լ���
//��������(vector3)
inline Vector3 lerp(const Vector3& st, const Vector3& en, float t)
{
	return Vector3(st.GetX() * (1.0f - t) + en.GetX() * t, st.GetY() * (1.0f - t) + en.GetY()*t, st.GetZ() * (1.0f - t) + en.GetZ() * t);
}
template<typename T>
inline const T& Clamp(const T& val, const T& low, const T& high)
{
	_ASEERTION_NULCHK(low <= high, "low larger then high!");
	if (val < low) return low;
	else if (val > high) return high;
	else return val;
}

inline static std::wstring _tomw(std::string str)
{
	std::wstring content;
	content.assign(str.begin(), str.end());
	return content;
}

inline static std::string _towm(std::wstring wstr)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> myconv;
	return myconv.to_bytes(wstr);
}

inline size_t HashingFile(const std::wstring& path)
{
	// FNV-1a (64��Ʈ) ���� ����
	size_t hash = 14695981039346656037ULL; // FNV_PRIME_64
	for (wchar_t c : path) {
		hash ^= static_cast<size_t>(c);
		hash *= 1099511628211ULL; // FNV_OFFSET_64
	}
	return hash;
}



