#include "IocpThreadManager.h"
#include "IocpInterface.h"
#include "StlUtil.h"
#include "IocpWorker.h"

namespace Redmoon
{

/**
@brief 생성자. 패킷 워커를 하나 만든다.
*/
CIocpThreadManager::CIocpThreadManager()
{
}

/**
@brief 생성자. 소멸 동작. 패킷 푸는 스레드를 모두 날려버린다. Iocp안쓰면 패킷 스레드가 필요 없어지기 때문에~
*/
CIocpThreadManager::~CIocpThreadManager()
{
	Release();
}

/**
@brief IOCP핸들을 워커에 등록하고 멀티맵에도 등록한다.
@param hIOCP IOCP핸들
@param nThreadCnt 생성할 스레드 갯수
*/
void CIocpThreadManager::Register(HANDLE hIOCP, size_t nThreadCnt)
{
	CScopeCriticalSection cs(m_CriticalSection);
	for(size_t i = 0; i < nThreadCnt; i++)
	{
		m_stl_multimap_Thread.insert(STL_MULTIMAP_THREAD::value_type(hIOCP, new CIocpWorker(hIOCP)));
	}
}

/**
@brief IOCP핸들에 따라 동작하는 스레드를 멈추게 한다.
@param hIOCP IOCP핸들
*/
void CIocpThreadManager::Unregister(HANDLE hIOCP)
{
	CScopeCriticalSection cs(m_CriticalSection);
	Execute<HANDLE, CIocpWorker>(m_stl_multimap_Thread, hIOCP, &CIocpWorker::Stop);
	Delete<HANDLE, CIocpWorker>(m_stl_multimap_Thread, hIOCP);
}

/**
@brief 자원 해제 동작
@desc 사용중인 모든 스레드를 멈추고 스레드를 제거한다.
*/
void CIocpThreadManager::Release()
{
	CScopeCriticalSection cs(m_CriticalSection);
	STL_MULTIMAP_THREAD::iterator it = m_stl_multimap_Thread.begin(); 
	while(it != m_stl_multimap_Thread.end())
	{
		SAFE_DELETE_THREAD(it->second);
		it = m_stl_multimap_Thread.erase(it);
	}
}

} //namespace Redmoon
