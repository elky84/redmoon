#pragma once

#include "AsyncSelectInterface.h"
#include "AutoPtr.h"
#include "IoInterface.h"
#include "ConnectorInterface.h"
#include "TCPSocket.h"
#include "Peer.h"

namespace Redmoon
{

class CTick;
class CBuffer;

/**
@brief AsyncSelect로 Connector 기능을 제공하는 클래스
*/
class CConnectorAsyncSelect : public CAsyncSelectInterface, public CTCPSocket, public CConnectorInterface, public CIoInterface
{
public:
	///생성자
	CConnectorAsyncSelect(CPeer* pcPeer = new CPeer);

	///소멸자
	virtual ~CConnectorAsyncSelect();

	///데이터 전송 메소드
	virtual bool Send(unsigned int nLen, void* pData, unsigned int nAddLen = 0, void* pAddData = 0);

	///접속 해제 요청 메소드
	virtual void Disconnect();

	///접속 중인지 여부
	virtual BOOL isConnected(){return m_bConnect;}

	///AliveCheck메소드. 일정 시간동안 패킷이 도착하지 않았다면 서버와 연결을 끊는다.
	void AliveCheck();

	///접속 시도 메소드
	bool Connect(const char* szIP, int nPort);

protected:
	///접속 해제 이벤트
	virtual void OnDisconnect();	

	///접속 완료 이벤트
	virtual void OnConnect();

	///Send 완료 이벤트
	virtual bool OnSend();

	///Recv 완료 이벤트
	virtual bool OnReceive();

	///완성된 패킷 처리 이벤트
	virtual bool OnReceive(unsigned int nLen, void* pData);

	///소켓 재사용 요청시
	virtual void Reuse();

private:
	///받는 데에 사용하는 버퍼
	CBuffer* m_pcRecvBuffer;

	///보내는데에 사용하는 버퍼
	CBuffer* m_pcSendBuffer;

	///접속중인지 여부
	BOOL m_bConnect;

	///AliveCheck용 틱
	CAutoPtr<CTick> m_pcTick;

};

} //namespace Redmoon