#pragma once

#include "Singleton.h"
#include "P2PPeer.h"
#include "P2PCommon.h"
#include "AutoPtr.h"
#include "IOInterface.h"
#include <map>
#include "UDPSocket.h"
#include "CriticalSection.h"

namespace Redmoon
{

/**
@brief P2P에서 사용되는 범용적인 기능을 제공하는 메소드
@desc 내부적으로 동기화 하고 있다.
*/
class CP2PInterface : public CUDPSocket, public CSingleton<CP2PInterface>
{
protected:
	///key:인덱스, value: P2P Peer
	typedef std::map<int, CP2PPeer*> STL_MAP_P2PPeer;
	
	///다른 컴퓨터와의 통신에 쓰이는 피어 맵
	STL_MAP_P2PPeer m_stl_map_Peer;

	///자신의 P2P 인덱스
	int m_nIdx;

	///자신의 외부 주소 구조체
	SOCKADDR_IN m_ExternalSockAddrIn;

	///크리티컬 섹션
	CCriticalSection m_CriticalSection;


public:
	///생성자
	CP2PInterface();

	///소멸자
	virtual ~CP2PInterface();
	
	///피어 맵에서 피어를 제거하는 메소드
	virtual void RemovePeer(int nIdx);

	///데이터 송신시 이용할 메소드
	virtual void Send(int nIdx, RUDPHeader* header, int nAddLen, void* pAddData);


	///인덱스에 해당하는 피어를 얻어오는 메소드
	CP2PPeer* GetPeer(int nIdx);

	/**
	@brief 자신의 인덱스를 설정한다
	@param nIdx 설정할 인덱스
	*/
	inline void SetIdx(int nIdx){m_nIdx = nIdx;}

	/**
	@brief 자신의 인덱스를 얻어온다
	@return 자신의 인덱스
	*/
	inline int GetIdx(){return m_nIdx;}

	/**
	@brief 자신의 내부 주소 구조체를 얻어온다
	@return 자신의 내부 주소 구조체 포인터
	*/
	inline sockaddr_in* GetInternalSockAddrIn(){return &m_SockAddrIn;}

	/**
	@brief 자신의 외부 주소 구조체를 설정한다.
	@param externalsockaddrin 자신의 외부 주소 구조체 포인터
	*/
	inline void SetExternalSockAddrIn(SOCKADDR_IN* externalsockaddrin){m_ExternalSockAddrIn =* externalsockaddrin;}

	/**
	@brief 자신의 포트를 얻어온다
	@return 자신의 포트
	*/
	inline unsigned short GetPort(){return ntohs(m_SockAddrIn.sin_port);}

	///연결 대상의 정보를 받았을때 불려지는 메소드
	void RecvPeerInfomation(PeerInformation* pPeerInformation);
	
	///도착한 패킷을 처리하는 메소드
	bool Receive(RUDPHeader* header);
	
	///자신과 연결되어있는 모든 개체에게 브로드 캐스트 한다
	void Broadcast(RUDPHeader* header, int nAddLen, void* pAddData);

	///릴레이 서버와 연결 완료되면 불려지는 메소드
	virtual void InitComplete() = 0;
	
	///데이터 수신시 불려지는 메소드
	virtual bool OnReceive(unsigned int len, void* data);
	
	///주기적으로 불려지는 메소드
	virtual void Act();
	
	///특정 포트로 바인드 한다.
	virtual bool Bind(int nPort);

	///완성된 패킷의 처리 메소드
	virtual void RecvPacket(RUDPHeader* header) = 0;

	/**
	@brief 크리티컬 섹션 진입
	*/
	inline void Enter(){m_CriticalSection.Enter();}

	/**
	@brief 크리티컬 섹션 퇴장
	*/
	inline void Leave(){m_CriticalSection.Leave();}

protected:
	///데이터 수신시 불려지는 메소드
	virtual bool OnReceive();
	
	///피어를 추가할 때 사용하는 메소드. 순수 가상 함수므로 반드시 재정의 해야함
	virtual CP2PPeer* AddPeer(int nIdx, SOCKADDR_IN* sockaddrin) = 0;

	///새로 피어를 생성할 때 이용할 메소드
	CP2PPeer* NewPeer(int nIdx, SOCKADDR_IN* sockaddrin);

	///모든 피어를 제거하는 메소드
	void ClearPeer();
};

inline CP2PInterface* GetP2PInterface()
{
	return CP2PInterface::InstancePtr();
} 

} //namespace Redmoon