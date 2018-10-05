#pragma once

#include "SocketCommon.h"
#include "ConnectorInterface.h"
#include <vector>
#include "IocpInterface.h"
#include "Session.h"
#include "ManualPtr.h"

#include <MSWSock.h>

namespace Redmoon
{

class CPeer;
class CPeerExManager;
class CSessionManager;

class CConnector : public CIocpInterface, public CConnectorInterface
{	
public:
	///쓰레드를 생성한다. 
	static void AllocThread(int nThreadCnt);

	///쓰레드에서 Connector를 뺀다.
	static void FreeThread();

	///생성자. pcPeer를 전달받아 소유한다.
	CConnector(CPeer* pcPeer = NULL, BOOL bActive = FALSE);

	///소멸자.
	virtual ~CConnector();

	///접속 대상의 ip와 port를 설정한다.
	virtual bool Set(const char* szIP, int nPort);

	///접속을 시도한다.
	virtual bool Connect();

	/**
	@brief 사용중인 CSocketConext를 반환한다.
	@return 사용중인 CSocketConext
	*/
	CSession* GetSession(){return m_pcSession.Get();}

	/**
	@brief 사용중인 CPeer를 반환한다.
	@return 사용중인 CPeer
	*/
	CPeer* GetPeer(){return m_pcPeer.Get();}
	
	///접속 끊기를 시도한다.
	void Disconnect();

	///일정 시간동안 패킷이 도착하지 않는 개체와의 접속을 끊는다.
	virtual void AliveCheck();

	/**
	@brief 접속 중인지 여부를 반환하는 메소드
	@return 접속 중인지 여부
	*/
	BOOL isConnected(){return m_pcSession->isConnected();}

	/**
	@brief 세션 인덱스 반환
	@return 세션 인덱스
	*/
	int GetIdx(){return m_pcSession->GetIdx();}


	///Connect를 시도한다.
	bool Start();

	///일정 시간 간격으로 반복해야 되는 작업을 호출. (소유한 Peer들의 Act를 호출)
	virtual void Act();

	///패킷을 보내는 메소드
	bool Send(const int nLen, const void* pData, const int nAddLen = 0, const void* pAddData = 0);

	///동작 그만~!!
	void Stop();

protected:
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

private:
	///초기화 작업
	bool _Set();

	///접속 완료시 불려지는 메소드.
	virtual void OnConnected(CSession* pcSession);

	///접속 끊기 시도하는 메소드
	virtual void Disconnect(CSession* pcSession);

	/**
	@brief 현재 사용중인 Iocp 핸들을 반환
	@return 현재 사용중인 Iocp 핸들
	*/
	virtual HANDLE GetIOCP(){return m_hIOCP;}

private:
	///현재 접속 작업 중인지 여부
	BOOL m_bConnecting;

	///IO를 담당하는 Peer를 소유한다. (CManualPtr)
	CManualPtr<CPeer> m_pcPeer;

	///워커에서 사용할 Iocp핸들
	static HANDLE m_hIOCP;

	///CConnector클래스의 참조값.
	static int m_nRefCount;

	///ConnectEx의 함수 포인터
	LPFN_CONNECTEX m_lpfnConnectEx;

	///동작 상태 플래그
	BOOL m_bActive;

	///Worker에 등록할 CSession의 포인터다. 소유한다 (AutoPtr)
	CAutoPtr<CSession> m_pcSession;
};

} //namespace Redmoon