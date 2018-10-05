#pragma once

#include "Common.h"

#include "Singleton.h"
#include <crtdbg.h>
#include "dbghelp.h"

namespace Redmoon
{

/**
@brief 미니덤프 생성 기능을 하는 클래스.
*/
class CMiniDumper
{
public:
	///생성자
	CMiniDumper();

	///static 핸들러 예외 발생시 불러라
	static LONG WINAPI Handler(struct _EXCEPTION_POINTERS *pExceptionInfo);

	///싱글턴으로 생성
	STATIC_SINGLETON_PTR(CMiniDumper);

protected:
	///미니 덤프 쓰기 메소드
	LONG WriteMiniDump(_EXCEPTION_POINTERS *pExceptionInfo);

	///덤프 파일 이름 지정 메소드
	void SetDumpFileName(const tstring& strDumpFileName);

	/**
	@brief 미니덤프 유저 스트림 정보 배열을 리턴하는 메소드
	@return 미니 덤프 유저 스트림 정보 배열
	*/
	virtual MINIDUMP_USER_STREAM_INFORMATION* GetUserStreamArray(){return NULL;}

private:
	///예외 정보 포인터
	_EXCEPTION_POINTERS* m_pExceptionInfo;

	///덤프 파일 저장될 경로
	TCHAR m_szDumpPath[_MAX_PATH];

	///프로그램 실행될 경로
	TCHAR m_szAppPath[_MAX_PATH];

	///프로그램 기본 이름
	TCHAR m_szAppBaseName[_MAX_PATH];
};

typedef BOOL (WINAPI *MINIDUMPWRITEDUMP)(HANDLE hProcess, DWORD dwPid, HANDLE hFile, MINIDUMP_TYPE DumpType, 
			CONST PMINIDUMP_EXCEPTION_INFORMATION ExceptionParam, CONST PMINIDUMP_USER_STREAM_INFORMATION UserStreamParam, CONST PMINIDUMP_CALLBACK_INFORMATION CallbackParam);

} //namespace Redmoon