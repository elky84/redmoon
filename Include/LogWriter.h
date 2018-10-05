#pragma once

#include <queue>

#include "Str.h"
#include "AutoPtr.h"

#include "Thread.h"
#include "Event.h"
#include "Singleton.h"

#include "CriticalSection.h"

namespace Redmoon
{

/**
@brief 로그를 파일로 남길때 사용할 정보 구조체
*/
struct SLogFileName
{
public:
	/**
	@brief 생성자.
	@param strKeyName 로그의 메인 네임
	@param strSubName 로그의 서브 네임
	*/
	SLogFileName(const tstring& strKeyName, const tstring& strSubName) : m_strKeyName(strKeyName), m_strSubName(strSubName)
	{
	}

	const tstring& GetKeyName() const {return m_strKeyName;}

	const tstring& GetSubName() const {return m_strSubName;}

private:
	///로그의 메인 네임
	tstring m_strKeyName;

	///로그의 서브 네임
	tstring m_strSubName;
};

/**
@brief 로그 구조체
*/
struct SLog : public SLogFileName
{
	SLog(const tstring& strKeyName, const tstring& strSubName, const tstring& strLog, SYSTEMTIME& systemtime) : SLogFileName(strKeyName, strSubName), m_strLog(strLog), m_SystemTime(systemtime)
	{
	}

	SLog(const SLog& refLog) : SLogFileName(refLog.GetKeyName(), refLog.GetSubName()), m_strLog(refLog.GetLog()), m_SystemTime(refLog.GetSystemTime())
	{
	}

	const SYSTEMTIME& GetSystemTime() const {return m_SystemTime;}
	const tstring& GetLog()const {return m_strLog;}

private:
	tstring m_strLog;

	SYSTEMTIME m_SystemTime;
};

typedef std::queue<SLog*> STL_QUEUE_LOG;

class CLogWriter : public CThread, public CSingleton<CLogWriter>
{
public:
	/**
	@brief 생성자. 싱글턴으로만 이용되게 하기위해 private 설정. 로그가 저장될 폴더 설정 읽어오는 역할도 한다.
	*/
	CLogWriter();

	/**
	@brief 소멸자. 아무것도 안함.
	*/
	virtual ~CLogWriter();

	/// 주기적인 동작
	virtual void Act();

	/// 각종 리소스 해제 동작
	virtual void Release();

	/**
	@brief 로그 경로 얻어오기
	@return 로그 경로
	*/
	const TCHAR* GetLogPath(){return m_strDirectoryPath.c_str();}

	/// 도착한 패킷 큐에 쌓기
	bool Push(const SLog& refLog);

private:
	/// 로그를 파일에 쓰는 메소드
	void _Write(SLog* psLog);


	/// 생산자와 소비자를 스왑한다.
	void _Swap();
	
	/// 소비자 큐에서 하나 꺼낸다.
	SLog* _Pop();

	/**
	@brief 큐가 모두 비었는지 검사
	@return 큐가 모두 비었는지 여부
	*/
	bool _Empty(){return m_stl_queue_Consumer->empty() && m_stl_queue_Producer->empty();}

	/// 디렉토리 설정
	void _SetDirectory(const tstring& strDirectory);

private:

	///로그가 기록될 디렉토리 경로
	tstring	m_strDirectoryPath;

	///소비자 큐
	CAutoPtr<STL_QUEUE_LOG> m_stl_queue_Consumer;

	///생산자 큐
	CAutoPtr<STL_QUEUE_LOG> m_stl_queue_Producer;

	///소비자 락
	CCriticalSection m_ConsumerLock;

	///생산자 락
	CCriticalSection m_ProducerLock;

	CEvent m_cLogEvent;
};

} //namespace Redmoon