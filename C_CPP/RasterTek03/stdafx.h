#pragma once

// SDKDDKVer.h를 포함하면 최고 수준의 가용성을 가진 Windows 플랫폼이 정의됩니다.

// 이전 Windows 플랫폼에 대해 응용 프로그램을 빌드하려는 경우에는 SDKDDKVer.h를 포함하기 전에
// WinSDKVer.h를 포함하고 _WIN32_WINNT 매크로를 지원하려는 플랫폼으로 설정하십시오.

#include <SDKDDKVer.h> //#include targetver.h의내용

//표준시스템포함파일, 프로젝트관련 포함파일이 들어있는 헤더파일

#define WIN32_LEAN_AND_MEAN

//windows 헤더파일
#include <Windows.h>

//C의 헤더파일
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

//프로그램에 필요한 추가헤더들은 여기서부터 추가한다
#include "DxDefine.h"