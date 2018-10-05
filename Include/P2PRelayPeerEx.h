#pragma once

#include "PeerEx.h"
#include "P2PRelayCommon.h"
#include "FunctionPtr.h"
#include <vector>

namespace Redmoon
{

class CP2PRelayPeerEx : public CPeerEx, public CFunctionPtr<CP2PRelayPeerEx, PRHeader>
{
	typedef std::vector<CP2PRelayPeerEx*> STL_VECTOR_P2PRelayPeerEx;
	///P2P 피어 구조체 그룹(벡터) 포인터
	STL_VECTOR_P2PRelayPeerEx* m_pstl_vector_P2PRelayPeerEx;

	///내부 주소 구조체
	sockaddr_in m_InternalSockaddrIn;

	///외부 주소 값
	sockaddr_in m_ExternalSockaddrIn;

	///참여하려 시도할 그룹 번호
	int m_nReservationGroupNo;

	///내부 주소를 받았는지 플래그
	bool m_bRecvInternalSockaddrin;

public:
	///생성자
	CP2PRelayPeerEx(const int& nIdx);

	///소멸자
	~CP2PRelayPeerEx();

	///접속 완료 이벤트
	virtual void OnConnect();

	///접속 해제 이벤트
	virtual void OnDisconnect();

	///패킷 수신 이벤트
	virtual void OnReceive(int nLen, void* pData);

	///패킷 분석 메소드
	virtual int Verify(const unsigned int nLen, const void* pData);

	/**
	@brief 현재 참여중인 P2P 릴레이 대상 목록의 포인터를 설정한다.
	@param pstl_vector_P2PRelayPeerEx P2P 릴레이 대상 목록 벡터 포인터
	*/
	inline void SetVectorP2PRelayPeerEx(STL_VECTOR_P2PRelayPeerEx* pstl_vector_P2PRelayPeerEx){m_pstl_vector_P2PRelayPeerEx = pstl_vector_P2PRelayPeerEx;}

	/**
	@brief 현재 참여중인 P2P 릴레이 대상 목록의 포인터를 반환한다.
	@return 현재 참여중인 P2P 릴레이 대상 목록 벡터
	*/
	inline STL_VECTOR_P2PRelayPeerEx* GetVectorP2PRelayPeerEx(){return m_pstl_vector_P2PRelayPeerEx;}

	///내부 주소를 설정한다.
	void SetInternalSockaddrIn(sockaddr_in* sockaddrin);

	///내부 주소를 얻어온다
	sockaddr_in* GetInternalSockaddrIn(){return &m_InternalSockaddrIn;};

	///외부 주소를 얻어온다
	sockaddr_in* GetExternalSockaddrIn(){return &m_ExternalSockaddrIn;}

	///데이터 전송 메소드
	bool Send(RPHeader* header, int nAddLen = 0, void* pAddData = NULL);

private:
	///내부 주소 전달 패킷
	void _RecvInternalSockAddrIn(PRHeader* header);

	///그룹 참여 패킷
	void _Enter(PRHeader* header);

	///그룹 퇴장 패킷
	void _Exit(PRHeader* header);

	///사용자 정의 패킷
	void _Command(PRHeader* header);

	///핑 패킷
	void _Ping(PRHeader* header);

public:
	virtual void Act();
};

} //namespace Redmoon