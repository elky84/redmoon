#pragma once

#include "SocketCommon.h"

namespace Redmoon
{

///패킷 재전송 주기
const int DEFAULT_RETRANSMISSION_TICK = 1000;

///릴레이 서버에 사용할 기본 포트
const int P2P_ECHO_PORT = 50000;

///UDP 소켓 바인드시 재시도 횟수
const int UDP_BIND_TRY_COUNT = 255;

/**
@brief P2P 대상 피어 정보를 담은 구조체
*/
struct PeerInformation
{
	///릴레이 서버에서 부여한 인덱스
	int m_nIdx;

	///내부 주소
	sockaddr_in InternalAddr;

	///외부 주소
	sockaddr_in ExternalAddr;

	/**
	@brief 빈 생성자
	*/
	PeerInformation(){
		memset(this, 0, sizeof(*this));
	}

	/**
	@brief 생성자.
	@param nidx 인덱스
	@param pInternalSockAddrIn 내부 주소
	@param pExternalSockAddrIn 외부 주소
	*/
	PeerInformation(int nIdx, sockaddr_in* pInternalSockAddrIn, sockaddr_in* pExternalSockAddrIn) 
		: m_nIdx(nIdx), InternalAddr (*pInternalSockAddrIn), ExternalAddr(*pExternalSockAddrIn)
	{
	}
};

enum RUDP_COMMAND
{ 
	RUDPCOMMAND_ISCONNECTED = 0,				// UDP 연결 확인 요청
	RUDPCOMMAND_SUCCESSED = 1,				// UDP 연결 확인 완료 패킷

	RUDPCOMMAND_RECV_PACKET_ANSWER = 1000,		//패킷 받았다는 응답

	RUDPCOMMAND_PING = 2000,				// AliveCheck용 핑~!

	RUDPCOMMAND_COMMAND = 10000,				// 명령 (일반적인 내용들)
};

enum RUDP_LEVEL{
	RUDPLEVEL_UNRELIABLE = 0, //전혀 보장 안해줌
	RUDPLEVEL_RELIABLE_ORDER = 1, //순서만 보장 해줌
	RUDPLEVEL_RELIABLE_RECEIVE = 2, //도착 보장 해줌
	RUDPLEVEL_RELIABLE_DATA = 4, //데이터 무결성 보장
	RUDPLEVEL_RELIABLE_ALL = RUDPLEVEL_RELIABLE_ORDER + RUDPLEVEL_RELIABLE_RECEIVE + RUDPLEVEL_RELIABLE_DATA, //전부 보장됨
};

#pragma pack(1)

/**
@brief RUDP에 사용되는 패킷 구조체
*/
struct RUDPHeader
{
private:
	///  보낸 인덱스
	int m_nIdx;		

	//header포함한 데이터 길이!
	PACKET_SIZE m_nLength;	

	///패킷 커맨드
	RUDP_COMMAND m_eCommand;

	///패킷 번호
	PACKET_NUMBER m_nPacketNumber;

	///패킷 체크 섬
	short m_nCheckSum;

public:
	/**
	@brief 생성자. 
	@param eCommand 패킷 커맨드
	@param nLen 패킷 길이 (헤더 크기 포함)
	*/
	RUDPHeader(RUDP_COMMAND eCommand, PACKET_SIZE nLen) : m_nLength(nLen), m_nIdx(0), m_eCommand(eCommand), m_nPacketNumber(0), m_nCheckSum(0)
	{
		m_nCheckSum = GetSumValue();
	}

	/**
	@brief 인덱스 설정
	@param nIdx 지정할 인덱스
	*/
	void SetIdx(int nIdx)
	{
		m_nCheckSum -= m_nIdx;
		m_nIdx = nIdx;
		m_nCheckSum += nIdx;
	}

	/**
	@brief 인덱스 가져오는 메소드
	@return 인덱스
	*/
	inline int GetIdx()
	{
		return m_nIdx;
	}

	/**
	@brief 패킷 길이 가져오는 메소드
	@return 패킷 길이
	*/
	inline PACKET_SIZE GetLen()
	{
		return m_nLength;
	}

	/**
	@brief 깨지지 않고 유효한 패킷인지 알아오는 메소드
	@return 체크섬 규칙에 따른 완성된 패킷인지 검사 결과
	*/
	inline bool isValidPacket()
	{
		return GetCheckSum() == GetSumValue() ? true : false;
	}

	/**
	@brief 체크섬 값 얻어오는 메소드
	@return 체크섬 값
	*/
	inline short GetCheckSum()
	{
		return m_nCheckSum;
	}

	/**
	@brief 패킷 번호 가져오는 메소드
	@return 패킷 번호
	*/
	inline PACKET_NUMBER GetPacketNumber()
	{
		return m_nPacketNumber;
	}

	/**
	@brief 패킷 커맨드 알아오는 메소드
	@return 패킷 커맨드
	*/
	inline RUDP_COMMAND GetCommand()
	{
		return m_eCommand;
	}

	/**
	@brief 패킷 번호 지정 메소드
	@param nPacketNumber 지정할 패킷 번호
	*/
	inline void SetPacketNumber(PACKET_NUMBER nPacketNumber)
	{
		m_nCheckSum -= m_nPacketNumber;
		m_nPacketNumber = nPacketNumber;
		m_nCheckSum += nPacketNumber;
	}

	/**
	@brief 체크섬에 사용될 값을 얻어오는 메소드
	@return 체크섬에 사용될 값
	*/
	inline short GetSumValue()
	{
		return m_nLength + m_nIdx + m_eCommand + m_nPacketNumber;
	}
};

struct RUDPIsConnected : public RUDPHeader{
	RUDPIsConnected() : RUDPHeader(RUDPCOMMAND_ISCONNECTED, sizeof(*this)){
	}
};

struct RUDPSuccessed : public RUDPHeader{
	RUDPSuccessed() : RUDPHeader(RUDPCOMMAND_SUCCESSED, sizeof(*this)){
	}
};

struct RUDPPing : public RUDPHeader{
	RUDPPing() : RUDPHeader(RUDPCOMMAND_PING, sizeof(*this)){
	}
};

struct RUDPRecvPacketNumber : public RUDPHeader{
	PACKET_NUMBER nRecvPacketNumber;
	RUDPRecvPacketNumber(unsigned short recvpacketnumber) : RUDPHeader(RUDPCOMMAND_RECV_PACKET_ANSWER, sizeof(*this)){
		nRecvPacketNumber = recvpacketnumber;
	}
};

struct RUDPCommand : public RUDPHeader{
	RUDPCommand(short len) : RUDPHeader(RUDPCOMMAND_COMMAND, sizeof(*this) + len){
	}
};

#pragma pack()

} //namespace Redmoon