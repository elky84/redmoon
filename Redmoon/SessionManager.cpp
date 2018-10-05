#include "SessionManager.h"
#include "Session.h"
#include "Peer.h"

namespace Redmoon
{

/**
@brief 소켓 컨텍스트 관리 클래스 생성자. 변수의 초기화를 담당한다.
*/
CSessionManager::CSessionManager() : m_nTotalCountSession(0)
{
}

/**
@brief 소켓 컨텍스트 관리 클래스 소멸자. 할당한 메모리를 해제한다.
*/
CSessionManager::~CSessionManager()
{
	CScopeCriticalSection cs(m_CriticalSection);
	while(!m_stl_map_UsingSession.empty()) 
	{
		STL_MAP_SESSION::iterator it = m_stl_map_UsingSession.begin();
		if(it != m_stl_map_UsingSession.end())
		{
			CSession* pcSession = it->second;
			Push(pcSession);
		}
	}

	while(!m_stl_queue_NotUseSession.empty())
	{
		CSession* pcSession = m_stl_queue_NotUseSession.front();
		m_stl_queue_NotUseSession.pop();

		SAFE_DELETE(pcSession);
	}
}

/**
@brief 소켓 컨텍스트를 새로 할당한다. 
@param nCount 이 매개변수 값 만큼 새로 할당한다.
*/
void CSessionManager::New(int nCount)
{
	CScopeCriticalSection cs(m_CriticalSection);
	for(int i = m_nTotalCountSession; i < nCount + m_nTotalCountSession; i++)
	{
		m_stl_queue_NotUseSession.push(new CSession(i));
	}

	m_nTotalCountSession += nCount;
}

/**
@brief 소켓 컨텍스트를 하나 꺼내서 반환한다.
@return 사용 가능한 소켓 컨텍스트
@desc 없으면 하나 더 만들어서 리턴한다.
*/
CSession*  CSessionManager::Pop()
{
	CScopeCriticalSection cs(m_CriticalSection);
	CSession* pcSession = NULL;
	if(m_stl_queue_NotUseSession.empty()) 
	{
		LOG_INFO(_T("CSessionManager::Pop() m_stl_queue_NotUseSession.empty()"));
		New(1); //하나 더 만들고 걔 쓰라고 함
		return Pop();
	}
	else
	{
		pcSession = m_stl_queue_NotUseSession.front();
		m_stl_queue_NotUseSession.pop();
	}

	m_stl_map_UsingSession.insert(STL_MAP_SESSION::value_type(pcSession->GetIdx(), pcSession));
	LOG_DEBUG(_T("%s Ok [%d, %d]"), __TFUNCTION__, m_stl_map_UsingSession.size(), m_stl_queue_NotUseSession.size());
	return pcSession;
}

/**
@brief 사용한 소켓 컨텍스트를 다시 큐에 집어넣는다
@param pcSession 큐에 접어넣을 소켓 컨텍스트
*/
void CSessionManager::Push(CSession* pcSession)
{
	CScopeCriticalSection cs(m_CriticalSection);
	pcSession->ClearPeer();
	m_stl_map_UsingSession.erase(pcSession->GetIdx()); //Get 해서 있었던거니 키로 지워도 된다고 확신!
	m_stl_queue_NotUseSession.push(pcSession); //무조건 큐에 넣어 재활용하자.
	LOG_DEBUG(_T("%s [%d] [%d]"), __TFUNCTION__, m_stl_map_UsingSession.size(), m_stl_queue_NotUseSession.size());
}

/**
@brief 소켓 컨텍스트를 하나 찾아서 반환한다
@param nIdx 식별 값
@return 성공 여부
*/
CSession* CSessionManager::Get(int nIdx)
{
	CScopeCriticalSection cs(m_CriticalSection);
	STL_MAP_SESSION::iterator it = m_stl_map_UsingSession.find(nIdx);
	if(it != m_stl_map_UsingSession.end())
	{
		return it->second;
	}
	LOG_ERROR(_T("%s Not Find Session [%d]"), __TFUNCTION__, nIdx);
	return NULL;
}

} //namespace Redmoon
