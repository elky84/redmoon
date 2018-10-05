#pragma once

//#pragma comment(linker, "/entry:WinMainCRTStartup /subsystem:console")

//������ �ӵ� ���
#define WIN32_LEAN_AND_MEAN

//������ ��ũ�� ���� ����
#include "Macro.h"

//#include <windows.h> ���� ���� ����Ǿ����.
#ifndef _WINSOCKAPI_
#define _WINSOCKAPI_
#endif

//_WINSOCKAPI_ ��ũ�� �����ϰ� ���� ������ ��� ����~
#include <Windows.h>

//#include <tchar.h> ���� ���� ����Ǿ����.
#include <io.h>

//io.h �ڿ� tchar.h�� ������ Str.h�� ����
#include "Str.h"

//iostream ��ɵ� �⺻ ����
#include <iostream>

//�α� ���
#include "LogManager.h"

//timeGetTime�� ����
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
@brief �����ڰ� ���� ����ü�� �ʱ�ȭ ����� �⺻ �����ϱ� ���� Ȯ���� ����ü
*/
template <typename T>
struct STRUCT_EX : public T
{
	/**
	@brief ������. ���� ����ü�� �ʱ�ȭ���ش�.
	*/
	STRUCT_EX()
	{
		memset(this, 0, sizeof(*this));
	}
};

/**
@brief 49�ϱ��� ��ȿ�� ���� ��ȯ���ִ� ����� �Լ�
@return ���α׷� ������ ������ ƽ
*/
inline DWORD GetTickLimit49Day()
{
	static DWORD beginTick = ::timeGetTime();
	return ::timeGetTime() - beginTick;
}

/**
@brief pid�� ������ �ڵ��� ������ �޼ҵ�
@param dwPid ã������ ���α׷��� Pid
@return ã�� ���α׷��� ������ �ڵ�. NULL�� ���� �� �ִ�.
*/
inline HWND GetHwndFromPid(DWORD dwPid)
{
	HWND tempHwnd = FindWindow(NULL,NULL); // �ֻ��� ������ �ڵ� ã��  
	while( tempHwnd != NULL )  
	{  
		if( GetParent(tempHwnd) == NULL ) // �ֻ��� �ڵ����� üũ, ��ư � �ڵ��� ���� �� �����Ƿ� �����ϱ� ����  
		{
			DWORD dwTempPid;
			::GetWindowThreadProcessId(tempHwnd, &dwTempPid);
			if( dwPid ==  dwTempPid)  
			{
				return tempHwnd;  
			}
		}
		tempHwnd = GetWindow(tempHwnd, GW_HWNDNEXT); // ���� ������ �ڵ� ã��  
	}  
	return NULL;  
}

} //namespace Redmoon