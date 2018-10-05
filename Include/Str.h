#pragma once

#include "Macro.h"

#include <tchar.h>
#include <string>
#include <iomanip>
#include <sstream>
#include <atlbase.h>

///stl string의 TCHAR형 매크로
typedef std::basic_string<TCHAR> tstring;

///stl stringstream의 TCHAR형 매크로
typedef std::basic_stringstream<TCHAR> tstringstream;

///TCHAR를 CHAR로 변환하는 매크로
#ifdef _UNICODE 
	#define TCHAR_TO_CHAR(x) CW2A(x) 
#else 
	#define TCHAR_TO_CHAR(x) (CHAR*)(x) 
#endif

///CHAR를 TCHAR로 변환하는 매크로
#ifdef _UNICODE 
	#define CHAR_TO_TCHAR(x) CA2W(x) 
#else 
	#define CHAR_TO_TCHAR(x) (CHAR*)(x) 
#endif 

///TCHAR를 WCHAR로 변환하는 매크로
#ifdef _UNICODE 
	#define TCHAR_TO_WCHAR(x) (TCHAR*)(x) 
#else 
	#define TCHAR_TO_WCHAR(x) CA2W(x) 
#endif

///WCHAR를 TCHAR로 변환하는 매크로
#ifdef _UNICODE 
	#define WCHAR_TO_TCHAR(x) (TCHAR*)(x) 
#else 
	#define WCHAR_TO_TCHAR(x) CW2A(x) 
#endif 

/**
@brief char를 wchar로 바꿔주는 메소드
@param pwszDest 변환할 문자열 버퍼 (wchar)
@param pszSrc 원본 문자열 (char)
*/
inline void CharToWChar(wchar_t* pwszDest, const char* pszSrc)
{
	size_t nLen = strlen(pszSrc)+1;
	size_t nConvertedChars = 0;
	MBSTOWCS(&nConvertedChars, pwszDest, nLen, pszSrc);
}

/**
@brief wchar를 char로 바꿔주는 메소드
@param pszDest 변환할 문자열 버퍼 (char)
@param pwszSrc 원본 문자열 (wchar)
*/
inline void WCharToChar(char* pszDest, const wchar_t* pwszSrc)
{
	size_t nLen = wcslen(pwszSrc) + 1;
	size_t nConvertedChars = 0;
	WCSTOMBS(&nConvertedChars, pszDest, nLen, pwszSrc);
}

/**
@brief 특정 문자열을 공백으로 대체해주는 메소드
@param szRaw 변환할 대상 문자열
@param nChar 들어가서는 안되는 문자열
@param nReplaceChar 대체할 문자열
*/
inline void ConvertString(IN OUT TCHAR * szRaw, IN TCHAR nChar = _T('\''), IN TCHAR nReplaceChar = _T(' '))
{
	for(size_t i = 0; i < _tcslen(szRaw); i++)
	{
		if(szRaw[i] == nChar)
		{
			szRaw[i] = nReplaceChar;
		}
	}
}

/**
@brief 문자열에 timestamp를 찍어서 반환
@return timestamp가 찍힌 문자열
@desc 반환 규격 연-월-일 시:분:초\t
*/
inline tstring GetFileTimeStamp()
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	TCHAR szTimeStamp[_MAX_FNAME];
	SPRINTF(szTimeStamp, _T("%04d%02d%02d_%02d%02d%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	return szTimeStamp;
}

/**
@brief 문자열에 timestamp를 찍어서 반환
@return timestamp가 찍힌 문자열
@desc 반환 규격 연-월-일 시:분:초\t
*/
inline tstring GetTimeStamp()
{
	SYSTEMTIME st;
	GetLocalTime(&st);

	TCHAR szTimeStamp[_MAX_FNAME];
	SPRINTF(szTimeStamp, _T("%04d-%02d-%02d %02d:%02d:%02d"), st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
	return szTimeStamp;
}

/**
@brief 문자열에 timestamp을 합쳐서 반환
@param szStr 문자열
@return timestamp가 찍힌 문자열
@desc 반환 규격 연-월-일 시:분:초\t문자열\n
*/
inline tstring GetTimeStamp(const TCHAR* szStr)
{
	return GetTimeStamp() + _T("\t") + szStr + _T("\n");
}

/**
@brief 가변 인수 문자열 유틸 클래스
*/
class CVariantArgumentString
{
	///문자열 배열의 포인터
	TCHAR* m_szString;

public:
	/**
	@brief 생성자. 널로 초기화만 함.
	*/
	CVariantArgumentString() : m_szString(NULL)
	{
		
	}

	/**
	@brief 소멸자. 동적할당한 문자열을 해제한다.
	*/
	~CVariantArgumentString()
	{
		SAFE_DELETE_ARRAY(m_szString);
	}

	/**
	@brief 가변 문자열로 넘어온 문자열을 합치는 오퍼레이터 재정의.
	@param szStr 가변 문자열로 넘어올 베이스 문자열
	*/
	void __cdecl operator() (TCHAR* szStr, ...)
	{
		va_list vl;
		va_start(vl, szStr);
		int nLen = _vsctprintf(szStr, vl) + sizeof(TCHAR);
		m_szString = new TCHAR[nLen];
		memset(m_szString, 0, sizeof(TCHAR) * nLen);
		VSTPRINTF(m_szString, nLen, szStr, vl);

		Write();
		va_end(vl);
	}

	/**
	@brief 합친 문자열 얻어오기
	@return 합친 문자열
	*/
	inline TCHAR* GetString(){return m_szString;}

protected:
	/**
	@brief 기록시에 사용할 메소드. 가상 함수로 써서 재정의를 유도함.
	*/
	virtual void Write(){}
};