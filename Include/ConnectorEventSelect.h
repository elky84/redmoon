#pragma once

#include "EventSelectInterface.h"
#include "AutoPtr.h"
#include "IOInterface.h"
#include "ConnectorInterface.h"
#include "TCPSocket.h"
#include "Peer.h"

namespace Redmoon
{

class CTick;
class CBuffer;

/**
@brief EventSelect로 커넥터 기능을 구현한 클래스.
*/
class CConnectorEventSelect : public CEventSelectInterface, public CTCPSocket, public CConnectorInterface, public CIoInterface
{
protected:
	///접속 해제 이벤트
	virtual void OnDisconnect();	

	///접속 완료 이벤트
	virtual void OnConnect();

	///Send 완료 이벤트
	virtual bool OnSend();

	///Recv 완료 이벤트
	virtual bool OnReceive();

	///소켓 재사용 요청시
	virtual void Reuse();

	///완성된 패킷 처리 이벤트
	virtual bool OnReceive(unsigned int nLen, void* pData);
public:
	///생성자
	CConnectorEventSelect(CPeer* peer = new CPeer);

	///소멸자
	virtual ~CConnectorEventSelect();

	///데이터 전송 메소드
	virtual bool Send(const unsigned int nLen, const void* pData, const unsigned int nAddLen = 0, const void* pAddData = 0);

	///접속 해제 요청 메소드
	virtual void Disconnect();

	///접속 중인지 여부
	virtual BOOL isConnected(){return m_bConnect;}

	///AliveCheck메소드. 일정 시간동안 패킷이 도착하지 않았다면 서버와 연결을 끊는다.
	void AliveCheck();

	///접속 시도 메소드
	bool Connect(const char* ip, int nPort);

private:
	CBuffer* m_pcRecvBuffer;

	CBuffer* m_pcSendBuffer;

	BOOL m_bConnect;

	CAutoPtr<CTick> m_pcTick;
};

}