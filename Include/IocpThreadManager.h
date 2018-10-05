#pragma once

#include "SocketCommon.h"
#include "Singleton.h"
#include "SessionManager.h"
#include <map>

namespace Redmoon
{

class CIocpWorker;

///key:스레드 핸들, value:iocp워커
typedef std::multimap<HANDLE, CIocpWorker*> STL_MULTIMAP_THREAD;

/**
@brief IOCP 쓰레드 (워커)를 관리하는 매니저
*/
class CIocpThreadManager : public CSingleton<CIocpThreadManager>
{
public:
	///생성자
	CIocpThreadManager();

	///소멸자
	virtual ~CIocpThreadManager();

	///스레드에 등록
	void Register(HANDLE hIOCP, size_t nThreadCnt);
	
	///스레드에서 뺌
	void Unregister(HANDLE hIOCP);

	///자원 해제
	void Release();

private:
	///스레드를 관리할 멀티맵
	STL_MULTIMAP_THREAD m_stl_multimap_Thread;
	CCriticalSection m_CriticalSection;
};

} //namespace Redmoon
