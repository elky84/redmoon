#include "LogWriter.h"

#include "LogManager.h"

#include <windows.h>
#include <algorithm>
#include "DirectoryUtil.h"

namespace Redmoon
{

/**
@brief 생성자. 싱글턴으로만 이용되게 하기위해 private 설정. 로그가 저장될 폴더 설정 읽어오는 역할도 한다.
*/
CLogWriter::CLogWriter() : m_stl_queue_Consumer(new STL_QUEUE_LOG), m_stl_queue_Producer(new STL_QUEUE_LOG)
{
	Start();
	std::locale::global(std::locale("kor")); //유니코드 한글 출력을 위한 것!
	TCHAR szStr[512];
	FILE *fp;
	TFOPEN(fp,_T("cfg\\logpath.cfg"), _T("rt"));
	if(fp) 
	{
		if (_ftscanf(fp, _T("LogPath  = %s\n"), szStr) == 1)
		{
			if(szStr[_tcslen(szStr)] !=_T('\\'))
				TCSCAT(szStr, _countof(szStr), _T("\\"));

			_SetDirectory(szStr);
		}
		else
		{
			_SetDirectory(_T("c:\\Log\\"));
		}
		fclose(fp);
	}
	else
	{
		_SetDirectory(_T("c:\\Log\\"));
	}
}

/**
@brief 소멸자. 아무것도 안함.
*/
CLogWriter::~CLogWriter()
{
	Release();
	while(!_Empty())
	{
		delete _Pop();
	}
}

/**
@brief 각종 리소스 해제 동작
*/
void CLogWriter::Release()
{
	Stop();
	m_cLogEvent.Set();
	__super::Release();
}

/**
@brief 주기적인 동작
*/
void CLogWriter::Act()
{
	m_cLogEvent.Wait();
	while(!_Empty())
	{
		SLog* psLog = _Pop();
		_Write(psLog);
		delete psLog;
	}
}

/**
@brief 로그를 파일에 쓰는 메소드
@param strKeyName 로그의 메인 네임
@param strSubName 로그의 서브 네임
@param strLog 기록할 로그
*/
void CLogWriter::_Write(SLog* psLog)
{
	TCHAR szfilename[MAX_PATH];
	const SYSTEMTIME& time = psLog->GetSystemTime();

	STPRINTF(szfilename, _T("%s\\%s_%d_%s_%04d-%02d-%02d.txt"), m_strDirectoryPath.c_str() , psLog->GetKeyName().c_str(), ::getpid(), psLog->GetSubName().c_str(), time.wYear, time.wMonth, time.wDay); 

	FILE *fp;
	TFOPEN(fp, szfilename, _T("at"));
	if (fp)
	{
		FTPRINTF(fp,_T("%02d:%02d:%02d\t%s\n"), time.wHour, time.wMinute, time.wSecond, psLog->GetLog().c_str());
		fclose(fp);
	}
}

/**
@brief 생산자와 소비자를 스왑한다.
@desc 소비자가 모든걸 소비(empty상태) 한 이후에 불러야한다. 그래야 순서가 어긋나지 않는다.
*/
void CLogWriter::_Swap()
{
	m_ProducerLock.Enter();
	m_ConsumerLock.Enter();
	STL_QUEUE_LOG *pTemp = m_stl_queue_Consumer.Release();
	m_stl_queue_Consumer.Reset(m_stl_queue_Producer.Release());
	m_ConsumerLock.Leave();

	m_stl_queue_Producer.Reset(pTemp);
	m_ProducerLock.Leave();
}

/**
@brief 소비자 큐에서 하나 꺼낸다.
@return 꺼낸 패킷
*/
SLog* CLogWriter::_Pop()
{
	m_ConsumerLock.Enter();
	if(m_stl_queue_Consumer->empty())
	{
		_Swap();
		if(m_stl_queue_Consumer->size()) //하나라도 있으면
		{
			m_ConsumerLock.Leave();
			return _Pop();
		}
		m_ConsumerLock.Leave();
		LOG_ERROR(_T("%s NULL"), __TFUNCTION__); //깨어났다면 무조건 하나는 있어야 되게끔 짜여져 있다.
		return NULL;
	}
	m_ConsumerLock.Leave();

	m_ConsumerLock.Enter();
	SLog *pSLog = m_stl_queue_Consumer->front();
	m_stl_queue_Consumer->pop();
	m_ConsumerLock.Leave();
	return pSLog;
}

/**
@brief 도착한 패킷 큐에 쌓기
@return 패킷 넣기 결과! 무조건 성공임.
*/
bool CLogWriter::Push(const SLog& refLog)
{
	m_ProducerLock.Enter();
	m_stl_queue_Producer->push(new SLog(refLog));
	m_ProducerLock.Leave();
	m_cLogEvent.Set();
	return true;
}

/**
@brief 디렉토리 설정
*/
void CLogWriter::_SetDirectory(const tstring& strDirectory)
{
	CDirectoryUtil::CreateDirectory(strDirectory);
	m_strDirectoryPath = strDirectory;		
}

} //namespace Redmoon