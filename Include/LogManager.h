#pragma once

#include <map>
#include "Singleton.h"
#include "ConsoleUtil.h"

namespace Redmoon
{

class CLogWriter;
class CCriticalSection;

#ifdef _UNICODE   
#define WIDEN(x)			L ## x   

#define WIDEN2(x)			WIDEN(x)   
#define __TFILE__			WIDEN2(__FILE__)   
#define __TFUNCTION__		WIDEN2(__FUNCTION__)   
#else   
#define __TFILE__			__FILE__   
#define __TFUNCTION__		__FUNCTION__   
#endif   

#define	LOGLEVEL_NONE			-1
#define	LOGLEVEL_DEBUG			0
#define	LOGLEVEL_INFO			1
#define	LOGLEVEL_ALARM			2
#define	LOGLEVEL_ERROR			3
#define	LOGLEVEL_STATICSTICS	4

enum LOG_MODE
{
	LOGMODE_NONE = 0,
	LOGMODE_CONSOLE,
	LOGMODE_DEBUGSTRING,
	MAX_LOGMODE,
};

/**
@brief Console창으로 로그를 출력하는 메소드
@param szStr 출력할 로그 문자열
*/
inline void DebugStringOutput(const TCHAR* szStr)
{
	tstring string = GetTimeStamp(szStr);
	::OutputDebugString(string.c_str());
}

/**
@brief Console창으로 로그를 출력하는 메소드
@param szStr 출력할 로그 문자열
*/
inline void ConsoleOutput(const TCHAR* szStr)
{
	HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
	if( INVALID_HANDLE_VALUE != hOut )
	{
		tstring string = GetTimeStamp(szStr);
		::WriteConsole(hOut, string.c_str(), (DWORD)string.size(), NULL, NULL);
	}
	else
	{
		DebugStringOutput(szStr);
	}
}

/**
@brief Log의 활성화 상태와 이름을 담고 있는 구조체
*/
struct SLogState
{
	///로그가 켜져있는지 여부
	bool m_bOn;

	///로그의 이름
	tstring m_strName;

	/**
	@brief 생성자. 로그의 상태와 이름을 입력받아 저장한다.
	@param strName 로그의 이름
	@param bOn 켜져 있는지 여부
	*/
	SLogState(const tstring& strName, bool bOn = true) : m_bOn(bOn), m_strName(strName)
	{
	}
};

/**
@brief 로그 기능을 관리하는 관리자.
*/
class CLogManager
{
public:
	/// 소멸자. 할당한 것들 해제.
	~CLogManager();

	/// 로그 기록 모드를 콘솔 모드로 설정한다.
	void ConsoleMode(bool bOn = true);

	/// 로그 기록 모드를 아웃풋 디버그 스트링 모드로 설정한다.
	void DebugStringMode(bool bOn = true);

	/// 로그 상태 추가 메소드.
	void Add(DWORD dwLogFlag, const SLogState& refLogState);

	/// 로그 상태 변경 메소드.
	bool Set(DWORD dwLogFlag, bool bOn);

	/// 등록된 디버그 함수 호출
	void CallDebugFunc(const TCHAR* szStr);

	/// 로그 이름 설정
	void SetLogName(tstring strLogName)
	{
		m_strLogName = strLogName;
	}

	/// 로그 기록 메소드.
	bool Log(DWORD dwLogFlag, const TCHAR* szStr);

	/// 로그가 켜져있는지 여부
	bool isOn(DWORD dwLogFlag);

	///싱글턴으로 생성
	STATIC_SINGLETON_PTR(CLogManager);

private:
	/// 로그 상태 얻어오는 메소드
	SLogState* _Get(DWORD dwLogFlag);

	/// 로그 기록 메소드
	void _Write(DWORD dwLogFlag, const TCHAR* szStr);

	/// 생성자. 로그를 사용할때의 초기 기능을 설정한다.
	CLogManager();

private:
	///key : integer형 LogFlag. value : SLogState 구조체 변수
	typedef std::map<DWORD, SLogState> STL_MAP_LOG_STATE;

	///로그 상태를 관리하는 맵
	STL_MAP_LOG_STATE m_stl_map_LogFlag;

	///로그 이름
	tstring m_strLogName;

	///로그 콜백 함수의 typedef형
	typedef void (*LOG_CALL_BACK)(const TCHAR*) ; 

	///key: 로그 모드. value : LOG_CALL_BACK 함수 포인터
	typedef std::map<LOG_MODE, LOG_CALL_BACK> STL_MAP_LOG_CALL_BACK; 

	///로그 콜백을 담고 있는 맵
	STL_MAP_LOG_CALL_BACK m_stl_map_LogCallBack;

	CAutoPtr<CLogWriter> m_pcLogWriter;
};

/**
@brief 로그 기능 베이스 클래스
*/
class CLogInterface : public CVariantArgumentString
{
public:
	CLogInterface(DWORD dwLogLevel) : m_dwLogLevel(dwLogLevel)
	{
	}

	/**
	@brief 가변 스트링을 이용해서, 로그가 켜져있을때만 로그를 쓰게 처리하는 메소드
	*/
	virtual void Write()
	{
		if(CLogManager::InstancePtr()->isOn(m_dwLogLevel))
		{
			SetColor();
			Log();
		}
	}

	///로그의 색상을 지정하는 순수 가상 함수
	virtual void SetColor() = 0;

	/**
	@brief 실제로 로그 매니저의 기능을 써서 로그를 쓰는 메소드
	*/
	void Log()
	{
		CLogManager::InstancePtr()->Log(m_dwLogLevel, GetString());
	}
	
private:
	DWORD m_dwLogLevel;
};

/**
@brief Debug 레벨로 로그 출력할 때 사용하는 클래스
*/
class CLogDebug : public CLogInterface
{
public:
	/**
	@brief 생성자. 로그 레벨을 지정한다.
	*/
	CLogDebug() : CLogInterface(LOGLEVEL_DEBUG)
	{

	}

protected:

	/**
	@brief Debug Level로 로그 출력하는 메소드. 회색 글씨로 출력.
	*/
	virtual void SetColor()
	{
		std::cout << con::fg_gray;
	}
};

/**
@brief Info 레벨로 로그 출력할 때 사용하는 클래스
*/
class CLogInfo : public CLogInterface
{
public:
	/**
	@brief 생성자. 로그 레벨을 지정한다.
	*/
	CLogInfo() : CLogInterface(LOGLEVEL_INFO)
	{

	}
protected:

	/**
	@brief Info Level로 로그 출력하는 메소드. 흰색 글씨로 출력.
	*/
	virtual void SetColor()
	{
		std::cout << con::fg_white;
	}
};

/**
@brief Error 레벨로 로그 출력할 때 사용하는 클래스
*/
class CLogError : public CLogInterface
{
public:
	/**
	@brief 생성자. 로그 레벨을 지정한다.
	*/
	CLogError() : CLogInterface(LOGLEVEL_ERROR)
	{

	}

protected:

	/**
	@brief Error Level로 로그 출력하는 메소드. 빨간 글씨로 출력.
	*/
	virtual void SetColor()
	{
		std::cout << con::fg_red;
	}	
};

/**
@brief Alarm 레벨로 로그 출력할 때 사용하는 클래스
*/
class CLogAlarm : public CLogInterface
{
public:
	/**
	@brief 생성자. 로그 레벨을 지정한다.
	*/
	CLogAlarm() : CLogInterface(LOGLEVEL_ALARM)
	{

	}

protected:
	/**
	@brief Alarm Level로 로그 출력하는 메소드. 빨간 글씨로 출력.
	*/
	virtual void SetColor()
	{
		std::cout << con::fg_cyan;
	}	
};

/**
@brief Staticstics 레벨로 로그 출력할 때 사용하는 클래스
*/
class CLogStaticstics : public CLogInterface
{
public:
	/**
	@brief 생성자. 로그 레벨을 지정한다.
	*/
	CLogStaticstics() : CLogInterface(LOGLEVEL_STATICSTICS)
	{

	}

protected:
	/**
	@brief Error Level로 로그 출력하는 메소드. 빨간 글씨로 출력.
	*/
	virtual void SetColor()
	{
		std::cout << con::fg_magenta;
	}	
};

} //namespace Redmoon

///CLogDebug 형식으로 () 오퍼레이터 호출 하기 위한 매크로
#define LOG_DEBUG Redmoon::CLogDebug()

///CLogInfo 형식으로 () 오퍼레이터 호출 하기 위한 매크로
#define LOG_INFO Redmoon::CLogInfo()

///CLogError 형식으로 () 오퍼레이터 호출 하기 위한 매크로
#define LOG_ERROR Redmoon::CLogError()

///CLogAlarm 형식으로 () 오퍼레이터 호출 하기 위한 매크로
#define LOG_ALARM Redmoon::CLogAlarm()

///CLogStaticstics 형식으로 () 오퍼레이터 호출 하기 위한 매크로
#define LOG_STATICSTICS Redmoon::CLogStaticstics()