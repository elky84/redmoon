#include "Common.h"
#include "LogManager.h"
#include "LogWriter.h"
#include "CriticalSection.h"
#include "FileUtil.h"

namespace Redmoon
{


/**
@brief 생성자. 로그를 사용할때의 초기 기능을 설정한다.
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
@brief 소멸자. 할당한 것들 해제.
*/
CLogManager::~CLogManager()
{

}

/**
@brief 로그 기록 모드를 콘솔 모드로 설정한다.
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
@brief 로그 기록 모드를 아웃풋 디버그 스트링 모드로 설정한다.
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
@brief 로그 상태 추가 메소드.
@param dwLogFlag 로그 플래그
@param refLogState 로그 참조자
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
@brief 로그 상태 변경 메소드.
@param dwLogFlag 로그 플래그
@param bOn OnOff 상태
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
@brief 등록된 디버그 함수 호출
@param szStr 전달할 문자열
*/
void CLogManager::CallDebugFunc(const TCHAR* szStr)
{
	for(STL_MAP_LOG_CALL_BACK::iterator it = m_stl_map_LogCallBack.begin(); it != m_stl_map_LogCallBack.end(); ++it)
	{
		(it->second)(szStr);
	}
}

/**
@brief 로그 기록 메소드.
@param dwLogFlag 로그 플래그
@param szStr 로그 문자열
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
@brief 로그가 켜져있는지 여부
@param dwLogFlag 로그 플래그
@return 켜져있는지 여부. 등록되어있지 않다면 false.
*/
bool CLogManager::isOn(DWORD dwLogFlag)
{
	SLogState* pcLogState = _Get(dwLogFlag);
	if(pcLogState)
		return pcLogState->m_bOn;

	return false;
}

/**
@brief 로그 상태 얻어오는 메소드
@param dwLogFlag 로그 플래그
@return 로그 상태 구조체 변수 포인터. 등록된 상태 없으면 NULL.
*/
SLogState* CLogManager::_Get(DWORD dwLogFlag)
{
	STL_MAP_LOG_STATE::iterator it = m_stl_map_LogFlag.find(dwLogFlag);
	if(it == m_stl_map_LogFlag.end())
		return NULL;

	return &it->second;
}

/**
@brief 로그 기록 메소드
@param dwLogFlag 로그 플래그
@param szStr 로그 문자열
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