#pragma once

#include "Common.h"
#include "AutoPtr.h"

namespace Redmoon
{

class CProcess
{
public:
	/**
	@brief 생성자.
	@param strCommandLine 커맨드 라인 명령
	@param strApplicationName 프로그램 이름. FullPath를 입력해야만 한다.

	*/
	CProcess(const tstring& strCommandLine, const LPSECURITY_ATTRIBUTES pProcessSecurityAttributes = NULL, const LPSECURITY_ATTRIBUTES pThreadSecurityAttributes = NULL, const tstring& strApplicationName = _T(""), const tstring& strCurrentDirectory = _T(""))
		: m_strCommandLine(strCommandLine), m_strApplicationName(strApplicationName), m_strCurrentDirectory(strCurrentDirectory), m_pProcessSecurityAttributes(pProcessSecurityAttributes), m_pThreadSecurityAttributes(pThreadSecurityAttributes)
	{
		m_StartupInfo.cb = sizeof(m_StartupInfo);
	}

	~CProcess()
	{
		Release();
	}

	void Release()
	{
		if(m_ProcessInformation.hProcess != INVALID_HANDLE_VALUE)
		{
			Close();
			::WaitForSingleObject(m_ProcessInformation.hProcess, INFINITE); //to do work. 5분 기다리다 안끝내면 terminate시킬까?

			SAFE_CLOSE_HANDLE(m_ProcessInformation.hProcess);
			SAFE_CLOSE_HANDLE(m_ProcessInformation.hThread);
		}
	}

	BOOL Create()
	{
		if(FALSE == ::CreateProcess(GET_LPSTR_OR_NULL(m_strApplicationName.c_str())
			, const_cast<char*>(m_strCommandLine.c_str())
			, m_pProcessSecurityAttributes.Get()
			, m_pThreadSecurityAttributes.Get()
			, isInheritHandle()
			, 0 /* dwCreationFlag values*/
			, NULL
			, GET_LPSTR_OR_NULL(m_strCurrentDirectory.c_str())
			, &m_StartupInfo
			, &m_ProcessInformation))
		{
			LOG_INFO(_T("%s [%d]"), __TFUNCTION__, GetLastError());
			return FALSE;
		}
		DWORD dwRet = ::WaitForInputIdle(m_ProcessInformation.hProcess, INFINITE);
		if(0 != dwRet)
		{
			LOG_INFO(_T("%s [%u] [%d]"), __TFUNCTION__, dwRet, GetLastError());
			Release();
			return FALSE;
		}
		return TRUE;
	}

	BOOL Close()
	{
		HWND hWnd = GetHwndFromPid(m_ProcessInformation.dwProcessId);
		if(hWnd != NULL)
		{
			PostMessage(hWnd, WM_CLOSE, 0, 0);
			return TRUE;
		}
		return FALSE;
	}

	inline BOOL isInheritHandle()
	{
		return m_pProcessSecurityAttributes.Get() && m_pThreadSecurityAttributes.Get() ? TRUE : FALSE;
	}

	BOOL Observe()
	{
		DWORD dwExitCode;
		if(m_ProcessInformation.hProcess)
		{
			::GetExitCodeProcess(m_ProcessInformation.hProcess, &dwExitCode);
			if( STILL_ACTIVE == dwExitCode)
				return TRUE;
		}
		return FALSE;
	}

private:
	tstring m_strCommandLine;

	tstring m_strApplicationName;
	
	tstring m_strCurrentDirectory;

	CAutoPtr<SECURITY_ATTRIBUTES> m_pProcessSecurityAttributes;
	
	CAutoPtr<SECURITY_ATTRIBUTES> m_pThreadSecurityAttributes;
	
	STRUCT_EX<STARTUPINFO> m_StartupInfo;
	
	STRUCT_EX<PROCESS_INFORMATION> m_ProcessInformation;
};

} //namespace Redmoon