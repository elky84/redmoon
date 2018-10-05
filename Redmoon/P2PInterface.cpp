#include "P2PInterface.h"
#include "Buffer.h"
#include <MMSystem.h>

namespace Redmoon
{

/**
@brief 생성자
*/
CP2PInterface::CP2PInterface() : m_nIdx(-1), CUDPSocket()
{

}

/**
@brief 소멸자. ClearPeer()를 호출한다.
*/
CP2PInterface::~CP2PInterface()
{
	ClearPeer();
}

/**
@brief 연결중인 모든 피어를 삭제한다.
*/
void CP2PInterface::ClearPeer()
{
	for( STL_MAP_P2PPeer::iterator it = m_stl_map_Peer.begin(); it != m_stl_map_Peer.end(); ++it )
	{
		SAFE_DELETE(it->second);
	}

	m_stl_map_Peer.clear();
}

/**
@brief UDP로 Receive 신호가 왔을때 불려지는 메소드
@return 처리 결과
*/
bool CP2PInterface::OnReceive()
{
	SOCKADDR_IN sockaddrin;
	CBuffer* buffer = Recv(&sockaddrin);
	if(buffer == NULL) return false;

	RUDPHeader* header = (RUDPHeader*)buffer->GetHeadBuffer();
	if(header->GetLen() < buffer->GetUsingSize()) {return false;} //헤더가 작으면 장난치는걸로 판단~! 데이터가 쪼금 왔을 수도?

	if(header->GetCommand() == RUDPCOMMAND_ISCONNECTED) //접속 요청 패킷이 피어 정보보다 먼저 올 가능성이 있다.
	{
		CP2PPeer* peer = GetPeer(header->GetIdx()); ///피어를 찾아보고
		if(peer)
		{
			peer->SetSockAddrIn(&sockaddrin); ///있으면 주소 갱신
		}
		else
		{
			NewPeer(header->GetIdx(), &sockaddrin); ///없으면 피어 생성
		}
	}
	return Receive(header);
}

/**
@brief UDP로 패킷이 왔을때 불려지는 메소드.  패킷의 유효성을 검사하고 유효하면 큐에 넣는다.
@param header 전달 받은 패킷 헤더
@return 유효하고 큐에 넣는데에 성공했는지 여부
*/
bool CP2PInterface::Receive(RUDPHeader* header)
{
	CP2PPeer* peer = GetPeer(header->GetIdx());
	if(peer == NULL)
	{
		LOG_ERROR(_T("%s Peer Not Find [%d] [%d]"), __TFUNCTION__, header->GetIdx(), header->GetCommand());
		return false;
	}

	return peer->Verify(header->GetLen(), header) ? true : false;
}

/**
@brief 연결 대상 정보를 받는 메소드
@param pPeerInformation 전달 받은 피어 정보
@desc 피어 정보를 바탕으로, 새 피어를 생성한다.
*/
void CP2PInterface::RecvPeerInfomation(PeerInformation* pPeerInformation)
{
	if(GetPeer(pPeerInformation->m_nIdx))
		return;

	SOCKADDR_IN* sockaddr_in = NULL;
	if(m_ExternalSockAddrIn.sin_addr.S_un.S_addr == pPeerInformation->ExternalAddr.sin_addr.S_un.S_addr)
	{
		sockaddr_in = &pPeerInformation->InternalAddr;
	}
	else
	{
		sockaddr_in = &pPeerInformation->ExternalAddr;
	}

	NewPeer(pPeerInformation->m_nIdx, sockaddr_in);
}

/**
@brief 피어를 새로 생성하는 메소드
@param nIdx 생성할 피어의 인덱스
@param sockaddrin 생성할 피어의 주소 구조체 포인터
@return 생성된 피어 포인터. 이미 등록된 피어의 정보라면, NULL을 리턴한다.
*/
CP2PPeer* CP2PInterface::NewPeer(int nIdx, SOCKADDR_IN* sockaddrin)
{
	STL_MAP_P2PPeer::iterator it = m_stl_map_Peer.find(nIdx);
	if(it != m_stl_map_Peer.end())
		return NULL;

	CP2PPeer* peer = AddPeer(nIdx, sockaddrin);
	m_stl_map_Peer.insert(STL_MAP_P2PPeer::value_type(nIdx, peer));

	LOG_INFO(_T("%s [%d] [%s:%d]"), __TFUNCTION__, peer->GetIdx(), inet_ntoa(sockaddrin->sin_addr), ntohs(sockaddrin->sin_port));
	peer->Act();
	return peer;
}

/**
@brief 피어를 찾아서 반환하는 메소드
@param nIdx 찾으려는 피어 인덱스
@return 찾은 피어 포인터. 못찾으면 NULL을 리턴한다.
*/
CP2PPeer* CP2PInterface::GetPeer(int nIdx)
{
	STL_MAP_P2PPeer::iterator it = m_stl_map_Peer.find(nIdx);
	if(it == m_stl_map_Peer.end())
		return NULL;

	return it->second;
}

/**
@brief 피어를 찾아서 삭제하는 메소드
@param nIdx 삭제하려는 피어 인덱스
*/
void CP2PInterface::RemovePeer(int nIdx)
{
	STL_MAP_P2PPeer::iterator it = m_stl_map_Peer.find(nIdx);
	if(it == m_stl_map_Peer.end())
	{
		LOG_ERROR(_T("%s NotFind Peer [%d]"), __TFUNCTION__, nIdx);
		return;
	}

	SAFE_DELETE(it->second);
	m_stl_map_Peer.erase(it);

	LOG_INFO(_T("%s [%d]"), __TFUNCTION__, nIdx);
}

/**
@brief 주기적으로 불려지는 메소드. 소유한 피어를 돌면서 Act를 호출해준다.
*/
void CP2PInterface::Act()
{
	for(STL_MAP_P2PPeer::iterator it = m_stl_map_Peer.begin(); it != m_stl_map_Peer.end(); ++it)
	{
		CP2PPeer* peer = it->second;
		peer->Act();
	}
}

/**
@brief 연결중인 대상들에게 브로드캐스트 하는 메소드
@param header 패킷 헤더를 포함한 기본 데이터
@param nAddLen 추가 데이터 크기
@param pAddData 추가 데이터 포인터
*/
void CP2PInterface::Broadcast(RUDPHeader* header, int nAddLen, void* pAddData)
{
	for(STL_MAP_P2PPeer::iterator it = m_stl_map_Peer.begin(); it != m_stl_map_Peer.end();++it)
	{
		CP2PPeer* peer = it->second;
		peer->Send(header, nAddLen, pAddData);
	}
}

/**
@brief 특정 피어에게만 패킷을 전달하는 메소드
@param nIdx 대상 피어 인덱스
@param header 패킷 헤더를 포함한 기본 데이터
@param nAddLen 추가 데이터 크기
@param pAddData 추가 데이터 포인터
*/
void CP2PInterface::Send(int nIdx, RUDPHeader* header, int nAddLen, void* pAddData)
{
	CP2PPeer* peer = GetPeer(nIdx);
	if(peer)
	{
		peer->Send(header, nAddLen, pAddData);
	}
}

/**
@brief 완성된 패킷으로, P2P 패킷이 도착했을때 불려지는 메소드
@param nLen 도착한 패킷 크기
@param pData 패킷 데이터 포인터
@return 패킷 처리 완료 여부
*/
bool CP2PInterface::OnReceive(unsigned int nLen, void* pData)
{
	RUDPHeader* header = (RUDPHeader*)pData;
	CP2PPeer* peer = GetPeer(header->GetIdx());
	if(peer == NULL)
	{
		LOG_ERROR(_T("%s Peer Not Find [%d] [%d]"), __TFUNCTION__, header->GetIdx(), header->GetCommand());
		return false;
	}

	peer->OnReceive(header->GetLen(), header);
	return true;
}

/**
@brief P2P에 이용할 UDP 소켓을 바인드 한다.
@return 성공 여부
*/
bool CP2PInterface::Bind(int nPort)
{
	return CUDPSocket::Bind(nPort);
}

} //namespace Redmoon
