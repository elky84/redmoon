#include "P2PRelayPeer.h"
#include "P2PRelayCommon.h"
#include "P2PEventSelect.h"

namespace Redmoon
{

/**
@brief 생성자. 패킷 처리 함수 포인터를 등록한다.
*/
CP2PRelayPeer::CP2PRelayPeer()
{
	Register(RPCOMMAND_RECV_EXTERNAL_SOCKADDRIN, &CP2PRelayPeer::_RecvExterSockAddrIn);
	Register(RPCOMMAND_RECV_PEERINFORMATION, &CP2PRelayPeer::_RecvPeerInformation);
	Register(RPCOMMAND_DISCONNECT_PEER, &CP2PRelayPeer::_DisconnectPeer);
	Register(RPCOMMAND_COMMAND, &CP2PRelayPeer::_Command);
	Register(RPCOMMAND_PING, &CP2PRelayPeer::_Ping);
}

/**
@brief 접속 완료 이벤트
*/
void CP2PRelayPeer::OnConnect()
{
	LOG_INFO(_T("%s"), __TFUNCTION__);

	//릴레이 서버로 한번 쏴본다. 포트가 뚫리도록~!!
	RUDPIsConnected sUdpData;
	CUDPSocket::Send(GetP2PInterface()->GetSocket(), GetSockAddr(GetIP().c_str(), P2P_ECHO_PORT), sizeof(sUdpData), &sUdpData);

	//내부 주소를 릴레이 서버로 올린다.
	PRInternalSockaddrIn sData(GetP2PInterface()->GetInternalSockAddrIn());
	Send(&sData);
}

/**
@brief 접속 해제 이벤트
*/
void CP2PRelayPeer::OnDisconnect()
{
}

/**
@brief 릴레이 서버로 데이터를 전송한다.
@param header 전달할 패킷 헤더
@nAddLen 부가 데이터 길이
@pAddData 부가 데이터 포인터
@return 전송 결과
*/
bool CP2PRelayPeer::Send(PRHeader* header, int nAddLen, void* pAddData)
{
	return CPeer::Send(header->GetLen(), header, nAddLen, pAddData);
}

/**
@brief 완성된 패킷 처리 메소드
@param nLen 패킷 길이
@param pData 패킷 데이터 포인터
*/
void CP2PRelayPeer::OnReceive(int nLen, void* pData)
{
	RPHeader* header = ((RPHeader*)pData);
	if(!OnProcess(header, header->GetCommand()))
	{
		LOG_ERROR(_T("%s [%d, %d]"), __TFUNCTION__, header->GetCommand(), header->GetLen());
	}
}

/**
@brief 수신 데이터를 분석해서 패킷으로 구성한다.
@param nLen 수신된 데이터 크기
@param pData 수신된 데이터 포인터
@return 패킷의 길이. 패킷으로 구성하지 못할 유효하지 않은 데이터면 0을 리턴한다.
*/
int CP2PRelayPeer::Verify(const unsigned int nLen, const void* pData)
{
	if(nLen < sizeof(RPHeader))
	{
		LOG_DEBUG(_T("%s [%d < %d]"), __TFUNCTION__, nLen, sizeof(RPHeader));
		return 0;
	}

	RPHeader* header = (RPHeader*)pData;
	if(nLen < header->GetLen()) //헤더에서의 길이가, 패킷 길이보다 작으면?
	{
		LOG_DEBUG(_T("%s [%d] [%d < %d]"), __TFUNCTION__, header->GetCommand(), nLen, header->GetLen());
		return 0;
	}
	else if(header->GetLen() <= 0)
	{
		LOG_ERROR(_T("%s [%d] [%d <= 0]"), __TFUNCTION__, header->GetCommand(), header->GetLen());
		Disconnect();
		return 0;
	}

	return header->GetLen();
}

/**
@brief 자신의 외부 주소 패킷 처리
@param header 패킷 헤더
*/
void CP2PRelayPeer::_RecvExterSockAddrIn(RPHeader* header)
{
	RPExternalSockaddrIn* externalsockaddrin = (RPExternalSockaddrIn*)header;
	GetP2PInterface()->SetExternalSockAddrIn(&externalsockaddrin->m_SockaddrIn);
	GetP2PInterface()->SetIdx(externalsockaddrin->m_nIdx);
	GetP2PInterface()->InitComplete();

	LOG_INFO(_T("%s [%d]"), __TFUNCTION__, externalsockaddrin->m_nIdx);
}

/**
@brief 연결 대상 정보 패킷 처리
@param header 패킷 헤더
*/
void CP2PRelayPeer::_RecvPeerInformation(RPHeader* header)
{
	RPPeerInformation* peerinformation = (RPPeerInformation*)header;
	GetP2PInterface()->RecvPeerInfomation(&peerinformation->m_PeerInformation);
}

/**
@brief 접속 해제 알림 패킷 처리
@param header 패킷 헤더
*/
void CP2PRelayPeer::_DisconnectPeer(RPHeader* header)
{
	RPDisconnectPeer*disconnectpeer = (RPDisconnectPeer* )header;
	GetP2PInterface()->RemovePeer(disconnectpeer->m_nIdx);
}

/**
@brief 사용자 정의 패킷 처리
@param header 패킷 헤더
*/
void CP2PRelayPeer::_Command(RPHeader* header)
{
	RPCommand* command = (RPCommand*)header;
	RUDPHeader* RUdpHeader = (RUDPHeader*)((char* )command + sizeof(RPCommand));

	if(RUdpHeader->GetCommand() == RUDPCOMMAND_ISCONNECTED) //릴레이로 IsConnected가 오면 걍 버린다.
		return;

	GetP2PInterface()->Receive(RUdpHeader);
}

/**
@brief 핑 패킷 처리
@param header 패킷 헤더
*/
void CP2PRelayPeer::_Ping(RPHeader* header)
{
	
}

} //namespace Redmoon
