#include "PeerManager.h"

namespace Redmoon
{

/**
@brief 생성자. 아무일도 안함
*/
CPeerManager::CPeerManager()
{

}

/**
@brief 소멸자. 할당한 피어들 모두 해제
*/
CPeerManager::~CPeerManager()
{
	_Clear();
}

/**
@brief 피어를 맵에 등록하는 메소드
@param nIdx 피어 인덱스
@param pcPeer 등록할 피어
*/
void CPeerManager::Set(int nIdx, CPeer* pcPeer)
{
	CScopeCriticalSection cs(m_CriticalSection);
	m_stl_map_Peer.insert(STL_MAP_Peer::value_type(nIdx, pcPeer));
	LOG_DEBUG(_T("%s OK [%d]"), __TFUNCTION__, nIdx);  
}


/**
@brief 피어를 맵에서 빼는 메소드
@param nIdx 키 값
*/
void CPeerManager::Release(int nIdx)
{
	CScopeCriticalSection cs(m_CriticalSection);
	STL_MAP_Peer::iterator it = m_stl_map_Peer.find(nIdx);
	if(it == m_stl_map_Peer.end())
	{
		LOG_ERROR(_T("%s Fail [%d]"), __TFUNCTION__, nIdx);  
		return;
	}

	m_stl_map_Peer.erase(it);
	LOG_DEBUG(_T("%s OK [%d]"), __TFUNCTION__, nIdx);
}

/**
@brief 사용중인 모든 피어를 삭제
*/
void CPeerManager::_Clear()
{
	CScopeCriticalSection cs(m_CriticalSection);
	m_stl_map_Peer.clear();
}

/**
@brief Active한 피어중에 nIdx를 가진 피어를 찾는 메소드
@param nIdx 키 값
@return 찾은 피어. 못찾으면 NULL을 반환한다.
*/
CPeer* CPeerManager::Get(int nIdx)
{
	CScopeCriticalSection cs(m_CriticalSection);
	STL_MAP_Peer::iterator it = m_stl_map_Peer.find(nIdx);
	if(it != m_stl_map_Peer.end())
	{
		return it->second;
	}
	else
	{
		if(nIdx != INVALID_PEER_IDX) //둘다 -1일 경우 전달 메시지
		{
			LOG_INFO(_T("%s Fail. Not Find Peer [%d]"), __TFUNCTION__, nIdx);
		}
	}
	return NULL;
}

/**
@brief 사용중인 피어를 돌며 Act함수를 호출해주는 메소드
*/
void CPeerManager::Act()
{
	CScopeCriticalSection cs(m_CriticalSection);
	for(STL_MAP_Peer::iterator it = m_stl_map_Peer.begin(); it != m_stl_map_Peer.end(); ++it)
	{
		CPeer* pcPeer = it->second;
		pcPeer->Act();
	}
}

/**
@brief 사용중인 피어를 돌며 브로드 캐스트 한다.
@param nLen Receive된 데이터 길이
@param pData 데이터 포인터
@param nAddLen pAddData의 크기
@param pAddData 기록할 추가 데이터
*/
void CPeerManager::BroadCast(int nLen, void* pData, int nAddLen, void* pAddData)
{
	CScopeCriticalSection cs(m_CriticalSection);
	for(STL_MAP_Peer::iterator it = m_stl_map_Peer.begin(); it != m_stl_map_Peer.end(); ++it)
	{
		(it->second)->Send(nLen, pData, nAddLen, pAddData);
	}
}

/**
@brief 사용중인 피어중 아무 피어나 임의로 뽑아내는 메소드
@return 임의의 피어. 등록된게 하나도 없을 때만 NULL을 리턴.
*/
CPeer* CPeerManager::GetRandPeer()
{
	CScopeCriticalSection cs(m_CriticalSection);
	std::vector<CPeer*> vList;
	for(STL_MAP_Peer::iterator it = m_stl_map_Peer.begin(); it != m_stl_map_Peer.end(); ++it)
		vList.push_back(it->second);

	if(vList.empty())
		return NULL;

	return vList[rand() % vList.size()];
}

} //namespace Redmoon
