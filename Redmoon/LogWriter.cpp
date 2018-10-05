#include "LogWriter.h"

#include "LogManager.h"

#include <windows.h>
#include <algorithm>
#include "DirectoryUtil.h"

namespace Redmoon
{

/**
@brief ������. �̱������θ� �̿�ǰ� �ϱ����� private ����. �αװ� ����� ���� ���� �о���� ���ҵ� �Ѵ�.
*/
CLogWriter::CLogWriter() : m_stl_queue_Consumer(new STL_QUEUE_LOG), m_stl_queue_Producer(new STL_QUEUE_LOG)
{
	Start();
	std::locale::global(std::locale("kor")); //�����ڵ� �ѱ� ����� ���� ��!
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
@brief �Ҹ���. �ƹ��͵� ����.
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
@brief ���� ���ҽ� ���� ����
*/
void CLogWriter::Release()
{
	Stop();
	m_cLogEvent.Set();
	__super::Release();
}

/**
@brief �ֱ����� ����
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
@brief �α׸� ���Ͽ� ���� �޼ҵ�
@param strKeyName �α��� ���� ����
@param strSubName �α��� ���� ����
@param strLog ����� �α�
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
@brief �����ڿ� �Һ��ڸ� �����Ѵ�.
@desc �Һ��ڰ� ���� �Һ�(empty����) �� ���Ŀ� �ҷ����Ѵ�. �׷��� ������ ��߳��� �ʴ´�.
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
@brief �Һ��� ť���� �ϳ� ������.
@return ���� ��Ŷ
*/
SLog* CLogWriter::_Pop()
{
	m_ConsumerLock.Enter();
	if(m_stl_queue_Consumer->empty())
	{
		_Swap();
		if(m_stl_queue_Consumer->size()) //�ϳ��� ������
		{
			m_ConsumerLock.Leave();
			return _Pop();
		}
		m_ConsumerLock.Leave();
		LOG_ERROR(_T("%s NULL"), __TFUNCTION__); //����ٸ� ������ �ϳ��� �־�� �ǰԲ� ¥���� �ִ�.
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
@brief ������ ��Ŷ ť�� �ױ�
@return ��Ŷ �ֱ� ���! ������ ������.
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
@brief ���丮 ����
*/
void CLogWriter::_SetDirectory(const tstring& strDirectory)
{
	CDirectoryUtil::CreateDirectory(strDirectory);
	m_strDirectoryPath = strDirectory;		
}

} //namespace Redmoon