#pragma once

//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

//컴파일 속도 향상
#define WIN32_LEAN_AND_MEAN

//유용한 매크로 내장 파일
#include "Macro.h"

//#include <windows.h> 보다 위에 선언되어야함.
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

//_WINSOCKAPI_ 매크로 선언하고 나서 윈도우 헤더 포함~
#include <Windows.h>

//#include <tchar.h> 보다 위에 선언되어야함.
#include <io.h>

//io.h 뒤에 tchar.h를 내장한 Str.h를 포함
#include "Str.h"

//iostream 기능도 기본 내장
#include <iostream>

//로그 기능
#include "LogManager.h"

//timeGetTime을 위해
#include <MMSystem.h>
#pragma comment(lib, "winmm.lib")

namespace Redmoon
{

enum IO_TYPE
{
	IOTYPE_SEND, // Send IO 
	IOTYPE_RECV, // Recv IO
	IOTYPE_CONNECT, // Accept/Connect IO
	IOTYPE_REUSE, // Reuse IO
	MAX_IOTYPE,
};

/**
@brief 생성자가 없는 구조체의 초기화 기능을 기본 제공하기 위해 확장한 구조체
*/
template <typename T>
struct STRUCT_EX : public T
{
	/**
	@brief 생성자. 원본 구조체를 초기화해준다.
	*/
	STRUCT_EX()
	{
		memset(this, 0, sizeof(*this));
	}
};

/**
@brief 49일까지 유효한 값을 반환해주는 경과값 함수
@return 프로그램 시작한 이후의 틱
*/
inline DWORD GetTickLimit49Day()
{
	static DWORD beginTick = ::timeGetTime();
	return ::timeGetTime() - beginTick;
}

/**
@brief pid로 윈도우 핸들을 얻어오는 메소드
@param dwPid 찾을려는 프로그램의 Pid
@return 찾는 프로그램의 윈도우 핸들. NULL도 있을 수 있다.
*/
inline HWND GetHwndFromPid(DWORD dwPid)
{
	HWND tempHwnd = FindWindow(NULL,NULL); // 최상위 윈도우 핸들 찾기  
	while( tempHwnd != NULL )  
	{  
		if( GetParent(tempHwnd) == NULL ) // 최상위 핸들인지 체크, 버튼 등도 핸들을 가질 수 있으므로 무시하기 위해  
		{
			DWORD dwTempPid;
			::GetWindowThreadProcessId(tempHwnd, &dwTempPid);
			if( dwPid ==  dwTempPid)  
			{
				return tempHwnd;  
			}
		}
		tempHwnd = GetWindow(tempHwnd, GW_HWNDNEXT); // 다음 윈도우 핸들 찾기  
	}  
	return NULL;  
}

} //namespace Redmoon