#include "MiniDump.h"

namespace Redmoon
{

/**
@brief 생성자. 기본 예외 필터를 Hander로 지정한다.
*/
CMiniDumper::CMiniDumper()
{
	::SetUnhandledExceptionFilter(Handler);
}

/**
@brief 미니 덤프 저장 기능을 처리 하는 메소드 
@param pExceptionInfo 예외 정보 포인터
@return 처리 결과 값
@desc 내부적으로 WriteMiniDump 메소드를 호출
*/
LONG CMiniDumper::Handler(_EXCEPTION_POINTERS *pExceptionInfo)
{
	return CMiniDumper::InstancePtr()->WriteMiniDump(pExceptionInfo);
}

/**
@brief 미니 덤프 저장 메소드
@param pExceptionInfo 예외 정보 포인터
@return 처리 결과 값
*/
LONG CMiniDumper::WriteMiniDump(_EXCEPTION_POINTERS *pExceptionInfo)
{
	LONG retval = EXCEPTION_CONTINUE_SEARCH;

	//정확한 dbghelp.dll을 찾아야 한다.
	//System32에 있는 것이 오래된 것(Win2K)일 수 있으므로, exe가 있는 
	//디렉토리를 먼저 뒤져야 한다. 즉, 배포본에는 따로 DBGHELP.DLL을 
	//포함시켜 디버깅할 수 있다는 얘기.

	HMODULE hDll = NULL;
	TCHAR szDbgHelpPath[_MAX_PATH];

	if(GetModuleFileName(NULL, m_szAppPath, _MAX_PATH))
	{
		TCHAR *pSlash = _tcsrchr(m_szAppPath, '\\');
		if(pSlash)
		{
			TCSCPY(m_szAppBaseName, _MAX_PATH, pSlash+1);
			*(pSlash+1) = 0;
		}

		TCSCPY(szDbgHelpPath, _MAX_PATH, m_szAppPath);
		TCSCPY(szDbgHelpPath, _MAX_PATH, _T("DBGHELP.DLL"));
		hDll = ::LoadLibrary(szDbgHelpPath);
	}

	if(hDll == NULL)
	{
		//못 찾았음. 기본 DLL 로드한다.
		hDll = ::LoadLibrary(_T("DBGHELP.DLL"));
	}

	LPCTSTR szResult = NULL;

	if(hDll)
	{
		MINIDUMPWRITEDUMP pMiniDumpWriteDump = 
			(MINIDUMPWRITEDUMP)::GetProcAddress(hDll, "MiniDumpWriteDump");

		if(pMiniDumpWriteDump)
		{
			TCHAR szScratch[_MAX_PATH];
			SetDumpFileName(GetFileTimeStamp());

			//덤프 파일 생성
			HANDLE hFile = ::CreateFile(m_szDumpPath, GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
			if(hFile != INVALID_HANDLE_VALUE)
			{
				_MINIDUMP_EXCEPTION_INFORMATION ExInfo;

				ExInfo.ThreadId = ::GetCurrentThreadId();
				ExInfo.ExceptionPointers = pExceptionInfo;
				ExInfo.ClientPointers = NULL;

				//덤프 기록
				BOOL bOK = pMiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpNormal, &ExInfo, GetUserStreamArray(), NULL);
				if(bOK)
				{
					szResult = NULL;
					retval = EXCEPTION_EXECUTE_HANDLER;
				}
				else
				{
					LPVOID pMsg;
					::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pMsg, 0, NULL);

					wsprintf(szScratch, _T("DumpFile '%s' Save Failed (Error %s)"), m_szDumpPath, pMsg);
					szResult = szScratch;
				}
				::CloseHandle(hFile);
			}
			else
			{
				LPVOID	pMsg;

				::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(), 
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&pMsg, 0, NULL);

				wsprintf(szScratch, _T("DumpFile '%s' Create Failed (Error %s)"), m_szDumpPath, pMsg);
				szResult = szScratch;
			}
		}
		else
		{
			szResult = _T("DBGHELP.DLL is Old version.");
		}
	}
	else
	{
		szResult = _T("DBGHELP.DLL is not find.");
	}
	if(szResult)
	{
		::MessageBox(NULL, szResult, NULL, MB_OK);
		TerminateProcess(GetCurrentProcess(), 0);
	}

	return retval;
}

/**
@brief 덤프 파일 이름을 지정하는 메소드
@param strDumpFileName 지정할 덤프 파일 명
*/
void CMiniDumper::SetDumpFileName(const tstring& strDumpFileName)
{
	TCSCPY(m_szDumpPath, _MAX_PATH, m_szAppPath);
	TCSCAT(m_szDumpPath, _countof(m_szDumpPath), strDumpFileName.c_str());
	TCSCAT(m_szDumpPath, _countof(m_szDumpPath), _T(".dmp"));
}

} //namespace Redmoon