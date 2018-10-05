#pragma once

#include "P2PCommon.h"

namespace Redmoon
{

#pragma pack(1)
enum RP_COMMAND
{
	RPCOMMAND_RECV_EXTERNAL_SOCKADDRIN = 0, //외부 주소 수신
	RPCOMMAND_RECV_PEERINFORMATION = 1, //피어 정보 수신
	RPCOMMAND_DISCONNECT_PEER = 2, //피어와의 연결 종료 알림

	RPCOMMAND_COMMAND = 3, //사용자 정의 패킷

	RPCOMMAND_PING = 1000, //핑
};

enum PR_COMMAND
{
	PRCOMMAND_RECV_INTERNAL_SOCKADDRIN = 0, //내부 주소 수신

	PRCOMMAND_ENTER = 1, //그룹 합류
	PRCOMMAND_EXIT = 2, //그룹 퇴장

	PRCOMMAND_COMMAND = 3, //사용자 정의 패킷

	PRCOMMAND_PING = 1000, //핑
};

/**
@brief Relay to Peer Header.
*/
struct RPHeader : public SHeader
{
private:
	///패킷 커맨드
	RP_COMMAND m_eCommand;
public:
	/**
	@brief 생성자. 
	@param nLen  패킷 길이
	@param eCommand 패킷 커맨드
	*/
	RPHeader(PACKET_SIZE nLen, RP_COMMAND eCommand) : SHeader(nLen), m_eCommand(eCommand)
	{
	}

	/**
	@brief 패킷 커맨드 알아오는 메소드
	@return 패킷 커맨드
	*/
	RP_COMMAND GetCommand(){return m_eCommand;}
};

/**
@brief Peer to Relay Header.
*/
struct PRHeader : public SHeader
{
private:
	///패킷 커맨드
	PR_COMMAND m_eCommand;

public:
	/**
	@brief 생성자. 
	@param nLen  패킷 길이
	@param eCommand 패킷 커맨드
	*/
	PRHeader(PACKET_SIZE nLen, PR_COMMAND eCommand) : SHeader(nLen), m_eCommand(eCommand)
	{
	}

	/**
	@brief 패킷 커맨드 알아오는 메소드
	@return 패킷 커맨드
	*/
	PR_COMMAND GetCommand(){return m_eCommand;}
};


struct RPPing : public RPHeader
{
	RPPing() : RPHeader(sizeof(*this), RPCOMMAND_PING)
	{
	}
};

struct PRPing : public PRHeader
{
	PRPing() : PRHeader(sizeof(*this), PRCOMMAND_PING)
	{
	}
};

struct RPExternalSockaddrIn : public RPHeader
{
	int m_nIdx;
	sockaddr_in m_SockaddrIn;
	RPExternalSockaddrIn(int nIdx, sockaddr_in* sockaddrin) : RPHeader(sizeof(*this), RPCOMMAND_RECV_EXTERNAL_SOCKADDRIN), m_nIdx(nIdx), m_SockaddrIn(*sockaddrin)
	{

	}
};

struct RPPeerInformation : public RPHeader
{
	PeerInformation m_PeerInformation;
	RPPeerInformation(PeerInformation* peerinformation) : RPHeader(sizeof(*this), RPCOMMAND_RECV_PEERINFORMATION) , m_PeerInformation(*peerinformation)
	{

	}
};

struct RPDisconnectPeer : public RPHeader
{
	int m_nIdx;
	RPDisconnectPeer(int nIdx) : RPHeader(sizeof(*this), RPCOMMAND_DISCONNECT_PEER) , m_nIdx(nIdx)
	{

	}
};

struct PRInternalSockaddrIn : public PRHeader
{
	sockaddr_in m_SockaddrIn;
	PRInternalSockaddrIn(sockaddr_in* sockaddrin) : PRHeader(sizeof(*this), PRCOMMAND_RECV_INTERNAL_SOCKADDRIN), m_SockaddrIn(*sockaddrin)
	{

	}
};

struct PREnter : public PRHeader
{
	int m_nGroupNo;
	PREnter(int nGroupNo) : PRHeader(sizeof(*this), PRCOMMAND_ENTER), m_nGroupNo(nGroupNo)
	{

	}
};

struct PRExit : public PRHeader
{
	PRExit() : PRHeader(sizeof(*this), PRCOMMAND_EXIT)
	{

	}
};


struct RPCommand : public RPHeader
{
	RPCommand(PACKET_SIZE len) : RPHeader(sizeof(*this) + len, RPCOMMAND_COMMAND)
	{

	}
};


struct PRCommand : public PRHeader
{
	int m_nReceiverIdx;
	PRCommand(int nReceiverIdx, PACKET_SIZE len) : PRHeader(sizeof(*this) + len, PRCOMMAND_COMMAND), m_nReceiverIdx(nReceiverIdx)
	{

	}
};

#pragma pack()

} //namespace Redmoon