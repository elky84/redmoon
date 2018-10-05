#include "Common.h"
#include <algorithm>
#include "P2PRelay.h"
#include "P2PRelayPeerEx.h"

namespace Redmoon
{

/**
@brief 생성자. 아무일도 안함.
*/
CP2PRelay::CP2PRelay()
{

}

/**
@brief 소멸자. 아무일도 안함.
*/
CP2PRelay::~CP2PRelay()
{

}

/**
@brief 그룹에 입장하는 기능
@param nGroupNo 입장하려는 그룹 번호
@param pcP2PRelayPeerEx 합류하려는 대상 피어
@desc 이미 참여중인 그룹이 있으면 그 곳에서 빼주는 처리도 한다.
*/
void CP2PRelay::Enter(int nGroupNo, CP2PRelayPeerEx* pcP2PRelayPeerEx)
{
	CScopeCriticalSection cs(m_CriticalSection);
	//이게 불리기전에 InternalSockaddrIn을 받아놓은 상태여야 한다.
	if(pcP2PRelayPeerEx->GetInternalSockaddrIn() == NULL)
	{
		LOG_ERROR(_T("%s pcP2PRelayPeerEx->GetInternalSockaddrIn() == NULL [%d] [%d]"), __TFUNCTION__, nGroupNo, pcP2PRelayPeerEx->GetIdx());
		return;
	}

	if(pcP2PRelayPeerEx->GetVectorP2PRelayPeerEx())
	{
		Exit(pcP2PRelayPeerEx);
	}

	STL_MAP_P2PPeerExGroup::iterator it = m_stl_map_P2PPeerExGroup.find(nGroupNo);
	if(it == m_stl_map_P2PPeerExGroup.end())
	{
		STL_VECTOR_P2PRelayPeerEx vP2PPeerEx;
		m_stl_map_P2PPeerExGroup.insert(STL_MAP_P2PPeerExGroup::value_type(nGroupNo, vP2PPeerEx));
		Enter(nGroupNo, pcP2PRelayPeerEx);
	}
	else
	{
		PeerInformation peerinformation(pcP2PRelayPeerEx->GetIdx(), pcP2PRelayPeerEx->GetInternalSockaddrIn(), pcP2PRelayPeerEx->GetExternalSockaddrIn());
		RPPeerInformation sPeerData(&peerinformation);

		STL_VECTOR_P2PRelayPeerEx* pstl_vector_P2PRelayPeerEx = &it->second;
		BroadCast(pstl_vector_P2PRelayPeerEx, &sPeerData);

		for(STL_VECTOR_P2PRelayPeerEx::iterator it = pstl_vector_P2PRelayPeerEx->begin(); it != pstl_vector_P2PRelayPeerEx->end(); ++it)
		{
			CP2PRelayPeerEx* pcTargetPeer = (*it);
			PeerInformation targetpeerinformation(pcTargetPeer->GetIdx(), pcTargetPeer->GetInternalSockaddrIn(), pcTargetPeer->GetExternalSockaddrIn());
			RPPeerInformation sTargetData(&targetpeerinformation);

			pcP2PRelayPeerEx->Send(&sTargetData);
		}

		//자신을 나중에 넣음으로써, 자기 정보를 자신이 받지 않도록 한다.
		pstl_vector_P2PRelayPeerEx->push_back(pcP2PRelayPeerEx);
		pcP2PRelayPeerEx->SetVectorP2PRelayPeerEx(pstl_vector_P2PRelayPeerEx);

		LOG_INFO(_T("%s [%d] [%d]"), __TFUNCTION__, nGroupNo, pcP2PRelayPeerEx->GetIdx());
	}
}

/**
@brief 그룹에 제외하는 기능
@param pcP2PRelayPeerEx 제외하려는 대상 피어
*/
void CP2PRelay::Exit(CP2PRelayPeerEx* pcP2PRelayPeerEx)
{
	CScopeCriticalSection cs(m_CriticalSection);
	STL_VECTOR_P2PRelayPeerEx* pstl_vector_P2PRelayPeerEx = pcP2PRelayPeerEx->GetVectorP2PRelayPeerEx();
	if(pstl_vector_P2PRelayPeerEx)
	{
		pstl_vector_P2PRelayPeerEx->erase(remove(pstl_vector_P2PRelayPeerEx->begin(), pstl_vector_P2PRelayPeerEx->end(), pcP2PRelayPeerEx));
		pcP2PRelayPeerEx->SetVectorP2PRelayPeerEx(NULL);

		RPDisconnectPeer sData(pcP2PRelayPeerEx->GetIdx());
		BroadCast(pstl_vector_P2PRelayPeerEx, &sData);
		LOG_INFO(_T("%s [%d]"), __TFUNCTION__, pcP2PRelayPeerEx->GetIdx());
	}
	else
	{
		LOG_ERROR(_T("%s Error [%d]"), __TFUNCTION__, pcP2PRelayPeerEx->GetIdx());
	}
}

/**
@brief 특정 그룹에 브로드 캐스트 하는 메소드
@param pstl_vector_P2PRelayPeerEx 브로드 캐스트 하려는 대상 그룹
@param header 보내려는 패킷의 헤더
*/
void CP2PRelay::BroadCast(STL_VECTOR_P2PRelayPeerEx* pstl_vector_P2PRelayPeerEx, RPHeader* header)
{
	CScopeCriticalSection cs(m_CriticalSection);
	for(STL_VECTOR_P2PRelayPeerEx::iterator it = pstl_vector_P2PRelayPeerEx->begin(); it != pstl_vector_P2PRelayPeerEx->end(); ++it)
	{
		CP2PRelayPeerEx* pcP2PRelayPeerEx = (*it);
		pcP2PRelayPeerEx->Send(header);
	}
}

/**
@brief 특정 피어에게만 데이터 전송 하는 메소드
@param pcP2PRelayPeerEx 데이터를 보내는 피어
@param nReceiverIdx 받을 대상 인덱스
@param header 보내려는 패킷의 헤더
*/
void CP2PRelay::Send(CP2PRelayPeerEx* pcP2PRelayPeerEx, int nReceiverIdx, RUDPHeader* header)
{
	CScopeCriticalSection cs(m_CriticalSection);
	STL_VECTOR_P2PRelayPeerEx* pstl_vector_P2PRelayPeerEx = pcP2PRelayPeerEx->GetVectorP2PRelayPeerEx();
	if(pstl_vector_P2PRelayPeerEx == NULL)
		return;

	for(STL_VECTOR_P2PRelayPeerEx::iterator it = pstl_vector_P2PRelayPeerEx->begin(); it != pstl_vector_P2PRelayPeerEx->end(); ++it)
	{
		CP2PRelayPeerEx* pcCheckP2PRelayPeerEx = (*it);
		if(pcCheckP2PRelayPeerEx->GetIdx() == nReceiverIdx) //받을 사람 찾으면!
		{
			RPCommand sData(header->GetLen());
			pcCheckP2PRelayPeerEx->Send(&sData, header->GetLen(), header);
			return;
		}
	}
}

} //namespace Redmoon