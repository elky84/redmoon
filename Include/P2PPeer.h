#pragma once

#include "SmartPtr.h"
#include "Peer.h"
#include "AutoPtr.h"
#include <list>
#include <map>
#include "Buffer.h"
#include "P2PCommon.h"
#include "CriticalSection.h"

namespace Redmoon
{

///패킷 재전송 시도 횟수
const int RETRANSMISSION_TRY_LIMIT_COUNT = 5;

/**
@brief 패킷 재전송에 사용되는 버퍼 구조체
*/
struct SRetransmissionBuffer
{
	///재전송한 횟수
	unsigned char m_nTryCount;

	///보낸 시간
	DWORD m_dwSendTick;

	///보낸 버퍼
	CBuffer* m_pcBuffer;
	
	/**
	@brief 생성자. 패킷을 처음 보낼때의 틱과, 패킷의 버퍼를 전달 받아 소유한다.
	@param dwSendTick 보냈을 때 틱
	@param pcBuffer 보낸 패킷 (CBuffer형)
	*/
	SRetransmissionBuffer(DWORD dwSendTick, CBuffer* pcBuffer) : m_nTryCount(0), m_dwSendTick(dwSendTick), m_pcBuffer(pcBuffer)
	{
	}
	
	/**
	@brief 소멸자. 동적할당했던 버퍼를 해제한다.
	*/
	~SRetransmissionBuffer()
	{
		SAFE_DELETE(m_pcBuffer);
	}
};

struct SBufferWrapper
{
	///패킷 번호
	PACKET_NUMBER m_nPacketNumber;
	
	///패킷 데이터 버퍼
	CBuffer* m_pcBuffer;

	/**
	@brief 생성자. 패킷 번호와 버퍼를 담아 관리한다.
	@param nPacketNumber 저장한 패킷의 번호
	@param pcBuffer 저장중인 패킷 데이터 버퍼
	*/
	SBufferWrapper(PACKET_NUMBER nPacketNumber, CBuffer* pcBuffer) : m_nPacketNumber(nPacketNumber), m_pcBuffer(pcBuffer)
	{
	}

	/**
	@brief 소멸자. 동적할당했던 버퍼를 해제한다.
	*/
	~SBufferWrapper()
	{
		SAFE_DELETE(m_pcBuffer);
	}
};

/**
@brief 특정 패킷 번호를 찾는데에 사용하는 비교 클래스
*/
class FindPacketNumber
{
	///찾으려는 패킷 번호
	PACKET_NUMBER m_nPacketNumber;
public:
	/**
	@brief 생성자.
	@param nPacketNumber 찾으려는 패킷 번호
	*/
	FindPacketNumber(PACKET_NUMBER nPacketNumber) : m_nPacketNumber(nPacketNumber) 
	{
	}

	/**
	@brief ()오퍼레이터 재정의. 찾으려는 패킷 넘버와 같은 넘버를 찾으면 true를 반환해 알려준다.
	@param nPacketNumber 비교하려는 패킷 번호
	*/
	bool operator()(PACKET_NUMBER nPacketNumber) const
	{       
		if(m_nPacketNumber == nPacketNumber) 
		{
			return true;
		}
		return false;
	}

	/**
	@brief ()오퍼레이터 재정의. 찾으려는 패킷 넘버와 같은 넘버를 가진 버퍼 랩퍼를 찾으면 true를 반환해 알려준다.
	@param right 비교 대상 버퍼 랩퍼.
	*/
	bool operator()(CSmartPtr<SBufferWrapper> right) const
	{       
		if(m_nPacketNumber == right->m_nPacketNumber) 
		{
			return true;
		}
		return false;
	}
};
/**
@brief 해당 패킷 번호보다 큰 첫번째 패킷을 찾는 비교 클래스
*/
class FindMorePacketNumber
{
	///찾으려는 패킷 번호
	PACKET_NUMBER m_nPacketNumber;
public:
	/**
	@brief 생성자.
	@param nPacketNumber 찾으려는 패킷 번호
	*/
	FindMorePacketNumber(PACKET_NUMBER nPacketNumber) : m_nPacketNumber(nPacketNumber) {
	}

	/**
	@brief ()오퍼레이터 재정의. 찾으려는 패킷 넘버보다 큰 번호를 찾으면 true를 반환해 알려준다.
	@param nPacketNumber 비교하려는 패킷 번호
	*/
	bool operator()(PACKET_NUMBER nPacketNumber) const
	{       
		if(m_nPacketNumber < nPacketNumber) {
			return true;
		}
		return false;
	}
};

/**
@brief 해당 패킷 번호보다 같거나 큰 첫번째 패킷을 찾는 비교 클래스
*/
class FindMorethanPacketNumber
{
	///찾으려는 패킷 번호
	PACKET_NUMBER m_nPacketNumber;
public:
	/**
	@brief 생성자.
	@param nPacketNumber 찾으려는 패킷 번호
	*/
	FindMorethanPacketNumber(PACKET_NUMBER nPacketNumber) : m_nPacketNumber(nPacketNumber){
	}

	/**
	@brief ()오퍼레이터 재정의. 찾으려는 패킷 넘버와 같거나 큰 넘버를 가진 첫번째 버퍼 랩퍼를 찾으면 true를 반환해서 알려준다.
	@param right 비교 대상 버퍼 랩퍼.
	*/
	bool operator()(CSmartPtr<SBufferWrapper> right) const
	{       
		if(m_nPacketNumber <= right->m_nPacketNumber) {
			return true;
		}
		return false;
	}
};

/**
@brief 패킷번호 값이 오버플로우가 됐는지 검사하고, 오버플로우 됐다면 그 위치를 찾기 위한 비교 클래스.
*/
class FindOverflowPoint
{
	///찾으려는 패킷 번호
	PACKET_NUMBER m_nPacketNumber;
public:
	/**
	@brief 생성자.
	@param nPacketNumber 찾으려는 패킷 번호
	*/	
	FindOverflowPoint(PACKET_NUMBER nPacketNumber = 0) : m_nPacketNumber(nPacketNumber)
	{

	}

	/**
	@brief ()오퍼레이터 재정의.  패킷번호 값이 오버플로우가 됐다면 true를 반환해 알려준다.
	@param right 비교 대상 버퍼 랩퍼.
	*/
	bool operator()(CSmartPtr<SBufferWrapper> right)
	{  
		if(right->m_nPacketNumber < m_nPacketNumber)
			return true;

		m_nPacketNumber = right->m_nPacketNumber;
		return false;
	}
};

///순서를 맞춰주기 위해 키핑해두는 버퍼 리스트의 typedef형
typedef std::list<CSmartPtr<SBufferWrapper> > STL_LIST_Buffer; 

///대기할 패킷 번호를 저장해두는 리스트의 typedef형
typedef std::list<unsigned short> STL_LIST_PacketNumber;

/**
@brief P2P 연결 대상 피어
*/
class CP2PPeer : public CPeer
{
	friend class CP2PInterface;
private:
	/// 식별용 인덱스
	int m_nIdx;

	///UDP 전송 중인지 여부
	bool m_bUdpEnable;

	///통신에 사용되는 주소 구조체
	SOCKADDR_IN m_SockAddrIn;

	///현재 보낸 패킷 번호
	PACKET_NUMBER m_nPacketNumber;

	///Send에 이용될 Socket 참조자
	const SOCKET &m_refSocket;

	///RUDP의 지원 레벨에 대한 플래그
	DWORD m_dwRUdpLevelFlag;

	///key : 패킷 번호, value : 재전송 버퍼의 스마트 포인터
	typedef std::map<short, CSmartPtr<SRetransmissionBuffer> > STL_MAP_RetransmissionBuffer;

	///재전송 버퍼를 저장하고 있는 맵
	STL_MAP_RetransmissionBuffer m_stl_map_RetransmissionBuffer;

	///재전송 주기
	DWORD m_dwRetransmissionTick;

	///처리하거나 저장중인 패킷 번호중 가장 큰 번호
	unsigned short m_nStoredPacketNumber;

	///아직 풀지 않고 보존중인 버퍼 목록
	STL_LIST_Buffer m_listKeepingBuffer; 

	///기다리는 패킷 번호 목록
	STL_LIST_PacketNumber m_listWaitingPacketNumber;
	
	///핑을 날린 가장 최근 틱
	DWORD m_dwPingTick;

	///크리티컬 섹션
	CCriticalSection m_CriticalSection;


private:

	///유효한 패킷인지 여부
	bool _isValidPacket(RUDPHeader* header);

	///패킷 번호 설정
	void _SetPacketNumber(RUDPHeader* header);

	///재전송하기 위해 추가
	bool _AddRetransmissionPacket(CBuffer* pcSendBuffer);

	///재전송 패킷 목록에서 제거
	bool _RemoveRetransmissionPacket(PACKET_NUMBER nPacketNumber);

	///패킷 푸는 메소드
	int _Proc(RUDPHeader* header);

	///UDP전송 메소드
	bool _UdpSend(CBuffer* buffer);

	///UDP 전송 메소드
	bool _UdpSend(RUDPHeader* header, int nAddLen = 0, void* pAddData = 0);

	///릴레이로 패킷을 전송한다.
	bool _RelaySend(CBuffer* pcBuffer);

	///릴레이로 패킷을 전송한다.
	virtual bool _RelaySend(RUDPHeader* header, int nAddLen = 0, void* pAddData = 0);

	///특정 패킷이 대기중인지를 반환하는 메소드
	STL_LIST_PacketNumber::iterator _FindWaitingPacketIterator(PACKET_NUMBER nPacketNumber);

	///특정 패킷 번호의 패킷이 대기중인지 여부
	bool _isWaitingPacketNumber(PACKET_NUMBER nPacketNumber);

	///저장중인 데이터를 처리하고 리스트도 비우는 메소드.
	void _ProcKeepingData();

	///해당 패킷 번호가 대기중인 패킷 번호 리스트에서 저장될 위치를 잡아주는 메소드
	STL_LIST_Buffer::iterator _GetInsertPosition(PACKET_NUMBER nPacketNumber);

	///특정 패킷보다 큰 번호의 반복자를 리턴하는 메소드
	STL_LIST_PacketNumber::iterator _GetAdjoinPacketNumber(PACKET_NUMBER nPacketNumber);

	///대기중인 패킷이 없을때 전달받은 패킷의 처리 메소드
	void _NonWaitingPacketProc(PACKET_NUMBER nPacketNumber, int nLen, void* pData);

	///패킷 번호를 맞춰서 풀기 위해 목록에 저장하는 메소드
	void _AddKeepingBuffer(PACKET_NUMBER nPacketNumber, int nLen, void* pData);

	///대기할 패킷 번호 목록에 추가
	void _AddWaitingList(PACKET_NUMBER nPacketNumber);

	///패킷을 잘 받았다고 전송한다. 더 보내지 말란건 안전하게 릴레이로 전달한다.
	void _SendRecvPacketNumber(PACKET_NUMBER nPacketNumber);

	/**
	@brief 패킷 번호를 증가 시키는 메소드
	*/
	inline void _IncreasePacketNumber(){m_nPacketNumber++;}
	
	/**
	@brief 현재 패킷 번호를 알아오는 메소드
	@return 현재 패킷 번호
	*/	
	inline unsigned short _GetPacketNumber(){return m_nPacketNumber;}

protected:
	///초기화
	virtual void Initialize();

	///자원 해제
	virtual void Release();

	///사용자 정의 패킷 푸는 메소드
	virtual void Proc(RUDPCommand* command) = 0;

public:
	///생성자.
	CP2PPeer(const SOCKET &refSocket, int nIdx, SOCKADDR_IN* sockaddrin, DWORD dwRetransmissionTick = DEFAULT_RETRANSMISSION_TICK, DWORD dwRUdpFlag = RUDPLEVEL_RELIABLE_ALL);

	///소멸자
	virtual ~CP2PPeer();

	/**
	@brief 피어의 인덱스를 반환한다.
	@return 피어의 인덱스
	*/
	inline int GetIdx(){return m_nIdx;}
	
	/**
	@brief 피어의 주소를 반환한다.
	@return 연결중인 대상 주소 구조체 포인터
	*/
	inline SOCKADDR_IN* GetSockAddrIn(){return &m_SockAddrIn;}
	
	/**
	@brief 피어의 주소를 설정한다.
	@param sockaddrin 연결 대상 주소 구조체 포인터
	*/
	inline void SetSockAddrIn(SOCKADDR_IN* sockaddrin);

	///완성된 패킷의 처리 메소드
	virtual void OnReceive(int nLen, void* data);

	///패킷의 유효성 검사 메소드
	virtual int Verify(const unsigned int nLen, const void* pData);

	///패킷 전달 메소드
	bool Send(RUDPHeader* header, int nAddLen = 0, void* pAddData = 0);

	///주기적으로 불러주어야 함. 상속 받은 후에도 상위 클래스의 Act를 꼭 불러줘야함!
	virtual void Act(); 

	/**
	@brief 크리티컬 섹션 진입
	*/
	inline void Enter(){m_CriticalSection.Enter();}

	/**
	@brief 크리티컬 섹션 퇴장
	*/
	inline void Leave(){m_CriticalSection.Leave();}

private:
	///Udp로 보내고, 재전송 하지 않는다.
	bool _UdpSendNotRetransmission(RUDPHeader* header, int nAddLen = 0, void* pAddData = 0);

	///Relay로 보내고 패킷 번호 지정을 안한다
	bool _RelaySendNotSetPacketNumber(RUDPHeader* header, int nAddLen = 0, void* pAddData = 0);

	///패킷 재전송.
	bool _Retransmission(int nPacketNumber);

	///재전송 중이던 패킷을 모두 릴레이로 보낸다.
	void _RetransmissionPacketToRelay();

private:
	///UDP전송이 가능한지 물어본 패킷을 받았다
	void _IsConnected(RUDPHeader* header);

	///UDP수신 성공 패킷을 받았다.
	void _Successed(RUDPHeader* header);

	///패킷 번호를 받았을때불려지는 메소드
	void _RecvPacketNumber(RUDPHeader* header);

	///주기적으로 날리는 핑을 받았다.
	void _Ping(RUDPHeader* header);

	///사용자 정의 패킷을 받았다.
	void _Command(RUDPHeader* header);
};

} //namespace Redmoon