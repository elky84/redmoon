#pragma once

#include "Peer.h"
#include "Singleton.h"
#include "FunctionPtr.h"

namespace Redmoon
{

struct RPHeader;
struct PRHeader;

/**
@brief 릴레이에서 사용되는 피어
*/
class CP2PRelayPeer : public CPeer, CSingleton<CP2PRelayPeer>, CFunctionPtr<CP2PRelayPeer, RPHeader>
{
public:
	///생성자
	CP2PRelayPeer();

	///접속 완료 이벤트
	virtual void OnConnect();

	///접속 해제 이벤트
	virtual void OnDisconnect();

	///데이터 전송 메소드
	bool Send(PRHeader* header, int nAddLen = 0, void* pAddData = NULL);

	///패킷 수신 이벤트
	virtual void OnReceive(int nLen, void* pData);

	///패킷 분석 메소드
	virtual int Verify(const unsigned int nLen, const void* pData);


private:
	///자신의 외부 주소 패킷
	void _RecvExterSockAddrIn(RPHeader* header);

	///연결 대상 정보
	void _RecvPeerInformation(RPHeader* header);

	///접속 해제 알림
	void _DisconnectPeer(RPHeader* header);

	///사용자 정의 패킷
	void _Command(RPHeader* header);

	///핑
	void _Ping(RPHeader* header);
};

} //namespace Redmoon
