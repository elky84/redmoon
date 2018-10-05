#include "Common.h"
#include "LogManager.h"
#include "LogWriter.h"
#include "CriticalSection.h"
#include "FileUtil.h"

namespace Redmoon
{


/**
@brief ������. �α׸� ����Ҷ��� �ʱ� ����� �����Ѵ�.
*/
CLogManager::CLogManager() : m_pcLogWriter(new CLogWriter)
{
	SetLogName(GetMoudleNameExclusiveExt());
	ConsoleMode(true);
	CLogManager::InstancePtr()->Add(LOGLEVEL_DEBUG, SLogState(_T("DEBUG"), false));
	CLogManager::InstancePtr()->Add(LOGLEVEL_INFO, SLogState(_T("INFO")));
	CLogManager::InstancePtr()->Add(LOGLEVEL_ERROR, SLogState(_T("ERROR")));
	CLogManager::InstancePtr()->Add(LOGLEVEL_ALARM, SLogState(_T("ALARM")));
	CLogManager::InstancePtr()->Add(LOGLEVEL_STATICSTICS, SLogState(_T("STATISTICS")));
}

/**
@brief �Ҹ���. �Ҵ��� �͵� ����.
*/
CLogManager::~CLogManager()
{

}

/**
@brief �α� ��� ��带 �ܼ� ���� �����Ѵ�.
*/
void CLogManager::ConsoleMode(bool bOn /* = true */)
{
	if(bOn)
	{
		::AllocConsole();
		m_stl_map_LogCallBack.insert(STL_MAP_LOG_CALL_BACK::value_type(LOGMODE_CONSOLE, &ConsoleOutput));
	}
	else
	{
		::FreeConsole();
		m_stl_map_LogCallBack.erase(LOGMODE_CONSOLE);
	}
}

/**
@brief �α� ��� ��带 �ƿ�ǲ ����� ��Ʈ�� ���� �����Ѵ�.
*/
void CLogManager::DebugStringMode(bool bOn /* = true */)
{
	if(bOn)
	{
		m_stl_map_LogCallBack.insert(STL_MAP_LOG_CALL_BACK::value_type(LOGMODE_DEBUGSTRING, &DebugStringOutput));
	}
	else
	{
		m_stl_map_LogCallBack.erase(LOGMODE_DEBUGSTRING);
	}
}

/**
@brief �α� ���� �߰� �޼ҵ�.
@param dwLogFlag �α� �÷���
@param refLogState �α� ������
*/
void CLogManager::Add(DWORD dwLogFlag, const SLogState& refLogState)
{
	STL_MAP_LOG_STATE::iterator it = m_stl_map_LogFlag.find(dwLogFlag);
	if(it != m_stl_map_LogFlag.end())
		it->second = refLogState;
	else
		m_stl_map_LogFlag.insert(STL_MAP_LOG_STATE::value_type(dwLogFlag, refLogState));
}

/**
@brief �α� ���� ���� �޼ҵ�.
@param dwLogFlag �α� �÷���
@param bOn OnOff ����
*/
bool CLogManager::Set(DWORD dwLogFlag, bool bOn)
{		
	SLogState* pcLogState = _Get(dwLogFlag);
	if(pcLogState)
	{
		pcLogState->m_bOn = bOn;
		return true;
	}
	return false;
}

/**
@brief ��ϵ� ����� �Լ� ȣ��
@param szStr ������ ���ڿ�
*/
void CLogManager::CallDebugFunc(const TCHAR* szStr)
{
	for(STL_MAP_LOG_CALL_BACK::iterator it = m_stl_map_LogCallBack.begin(); it != m_stl_map_LogCallBack.end(); ++it)
	{
		(it->second)(szStr);
	}
}

/**
@brief �α� ��� �޼ҵ�.
@param dwLogFlag �α� �÷���
@param szStr �α� ���ڿ�
*/
bool CLogManager::Log(DWORD dwLogFlag, const TCHAR* szStr)
{
	bool bRet = isOn(dwLogFlag);
	if(bRet)
		CallDebugFunc(szStr);

	for(DWORD i = dwLogFlag; i != LOGLEVEL_NONE; i--)
		_Write(i, szStr);

	return bRet;
}

/**
@brief �αװ� �����ִ��� ����
@param dwLogFlag �α� �÷���
@return �����ִ��� ����. ��ϵǾ����� �ʴٸ� false.
*/
bool CLogManager::isOn(DWORD dwLogFlag)
{
	SLogState* pcLogState = _Get(dwLogFlag);
	if(pcLogState)
		return pcLogState->m_bOn;

	return false;
}

/**
@brief �α� ���� ������ �޼ҵ�
@param dwLogFlag �α� �÷���
@return �α� ���� ����ü ���� ������. ��ϵ� ���� ������ NULL.
*/
SLogState* CLogManager::_Get(DWORD dwLogFlag)
{
	STL_MAP_LOG_STATE::iterator it = m_stl_map_LogFlag.find(dwLogFlag);
	if(it == m_stl_map_LogFlag.end())
		return NULL;

	return &it->second;
}

/**
@brief �α� ��� �޼ҵ�
@param dwLogFlag �α� �÷���
@param szStr �α� ���ڿ�
*/
void CLogManager::_Write(DWORD dwLogFlag, const TCHAR* szStr)
{
	SLogState* pcLogState = _Get(dwLogFlag);
	if( NULL == pcLogState || !pcLogState->m_bOn )
		return;

	SYSTEMTIME st;
	GetLocalTime(&st);
	CLogWriter::InstancePtr()->Push(SLog(m_strLogName.c_str(), pcLogState->m_strName.c_str(), szStr, st));
}

} //namespace Redmoon