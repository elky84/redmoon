#pragma once

#include "SocketCommon.h"

#include "listeninterface.h"
#include "Session.h"
#include "AutoPtr.h"
#include "Acceptor.h"
#include <vector>
#include <list>
#include "IocpInterface.h"
#include "AutoPtr.h"

#include "PeerManager.h"

#include "PeerEx.h"

#include "CriticalSection.h"

#define DECLARE_ALLOCATE_FUNCTION(CLASS, FunctionName) \
	inline Redmoon::CPeerEx* FunctionName(int nIdx)\
	{\
		return new CLASS(nIdx);\
	}

#define ALLOCATE_FUNCTION_PTR(FunctionName) Redmoon::CPeerEx* (*FunctionName)(int nIdx)

namespace Redmoon
{

/**
@brief Iocp로 Listen을 하는 클래스.
*/
class CListener : public CIocpInterface, public CListenInterface
{
	///Session의 맵
	typedef std::map<int, CSession*> STL_MAP_SESSION;

private:
	///접속 완료시 불려지는 메소드.
	virtual void OnConnected(CSession* pcSession);

	///접속 끊겼을시 불려지는 메소드
	virtual void OnDisconnect(CSession* pcSession);

	///Io실패시 불려지는 메소드
	virtual void OnIoFailed(CSession* pcSession);

	///소켓 재사용 가능시 불려지는 메소드
	virtual void OnReuse(CSession* pcSession);

	///소켓 재사용 원할시 부르는 메소드
	virtual void Reuse(CSession* pcSession);

	///접속/억셉트 실패시
	virtual void ConnectFailed(CSession* pcSession);

	/**
	@brief 현재 사용중인 Iocp 핸들을 반환
	@return 현재 사용중인 Iocp 핸들
	*/
	virtual HANDLE GetIOCP(){return m_hIOCP;}

	///AliveCheck를 해서, 일정 시간동안 패킷이 도착하지 않은 Session의 목록을 listDisconnectedSession에 담는 메소드
	void _AliveCheck(std::list<CSession*>&listDisconnectedSession);

	///PeerManager의 포인터를 반환한다.
	CPeerManager* GetPeerManager(){return m_pcPeerManager.Get();}

protected:
	///CPeerEx를 생성하여 반환하는 메소드.
	virtual CPeerEx* _Allocate(const int& nIdx);

public:
	///생성자. Allocate 함수의 포인터를 넘겨 받는다. 기본은 CPeerEx를 Allocate한다.
	CListener(ALLOCATE_FUNCTION_PTR(pFunc) = NULL);

	///소멸자.
	virtual ~CListener();

	///임의의 CPeerEx를 반환한다.
	CPeer* GetRandPeer();

	/**
	@brief CPeerEx를 찾아서 반환하는 메소드. template로 넘어온 타입으로 캐스팅해서 반환한다. dynamic_cast이므로 클래스 구조상 가능한 캐스트가 아니면 실패.
	@param nIdx 찾는 Peer의 인덱스
	@param nAccKey 찾는 Peer의 누적 키 값.
	@return template로 넘어온 T형으로 반환. cast실패나 못찾으면 NULL 반환.
	@desc 반환된 CPeerEx는 사용 후, 반드시 DecRefCount 해주어야 한다.
	*/
	template <typename T> T* GetPeerEx(int nIdx, int nAccKey)
	{
		return dynamic_cast<T*> (GetPeerExManager()->Get(nIdx, nAccKey));
	}

	///쓰레드 할당.
	static void AllocThread(int nThreadCnt);

	///쓰레드 해제. 모든 스레드를 해제한다.
	static void FreeThread();

	///Listener를 시작한다.
	bool Start(int nPort, int nWaitingQueueSize = DEFAULT_WAITING_QUEUE);

	///접속 끊기 시도하는 메소드
	virtual void Disconnect(CSession* pcSession);

	///접속 끊기 시도하는 메소드. nIdx로 CSession를 찾아서 접속을 끊는다.
	void Disconnect(int nIdx);

	///AliveCheck메소드. 일정 시간동안 패킷이 도착하지 않은 CSession들의 접속을 끊는다.
	virtual void AliveCheck();

	///데이터 전송 메소드.
	virtual bool Send(CSession* pcSession, int nLen, void* pData, int nAddLen = 0, void* pAddData = 0);
	
	///데이터 전송 메소드
	bool Send(int nIdx, int nLen, void* pData, int nAddLen = 0, void* pAddData = 0);

	///이 Listener에 포함된 모든 개체들에게 브로드 캐스트 하는 메소드.
	void BroadCast(int nLen, void* pData, int nAddLen = 0, void* pAddData = 0);

	///일정 시간 간격으로 반복해야 되는 작업을 호출. (소유한 Peer들의 Act를 호출)
	void Act();

private:
	/**
	@brief 크리티컬 섹션 진입
	*/
	inline void _Enter(){m_CriticalSection.Enter();}

	/**
	@brief 크리티컬 섹션 퇴장
	*/
	inline void _Leave(){m_CriticalSection.Leave();}

	///Accept 메소드
	inline bool _Accept(CSession* pcSession);

private:
	///CPeerEx를 반환하는 규격의 함수 포인터
	ALLOCATE_FUNCTION_PTR(m_pFunc);

	///사용중인 Session를 관리하는 맵
	STL_MAP_SESSION m_stl_map_UsingSession;

	///할당받은 Session를 관리하는 맵
	STL_MAP_SESSION m_stl_map_Session;

	///PeerEx를 관리하는 매니저를 소유한다. (AutoPtr)
	CAutoPtr<CPeerManager> m_pcPeerManager;

	///Accept기능을 가진 CAcceptor를 소유한다 (AutoPtr)	
	CAutoPtr<CAcceptor> m_pcAcceptor;

	///Listner끼리는 같은 IocpHandle을 공유한다.
	static HANDLE m_hIOCP;

	///현 클래스의 참조 횟수.
	static int m_nRefCount;

	///크리티컬 섹션
	CCriticalSection m_CriticalSection;

	///Accept 걸려있는 숫자. 10%보다 작으면? 새로~
	LONG m_nAcceptSessionCount;

	///전체적으로 Listen할 숫자.
	LONG m_nMaxSessionCount;
};

} //namespace Redmoon