#pragma once

#include <vector>
#include <map>
#include "CriticalSection.h"

namespace Redmoon
{

struct RPHeader;
struct RUDPHeader;
class CP2PRelayPeerEx;

/**
@brief 릴레이 서버에서, 릴레이 기능을 지원하는 클래스
*/
class CP2PRelay
{
	///릴레이할 대상(CP2PRelayPeerEx*)의 그룹. (벡터)
	typedef std::vector<CP2PRelayPeerEx*> STL_VECTOR_P2PRelayPeerEx;

	///key: 그룹번호, value : 릴레이 대상 그룹. (벡터)
	typedef std::map<int, STL_VECTOR_P2PRelayPeerEx> STL_MAP_P2PPeerExGroup;

	///릴레이할 대상 맵
	STL_MAP_P2PPeerExGroup m_stl_map_P2PPeerExGroup;

	///크리티컬 섹션
	CCriticalSection m_CriticalSection;

public:
	///생성자
	CP2PRelay();

	///소멸자
	~CP2PRelay();

	///해당 그룹 멤버들에게 브로드 캐스트
	void BroadCast(STL_VECTOR_P2PRelayPeerEx* pstl_vector_P2PRelayPeerEx, RPHeader* header);

	///특정 피어에게만 데이터 전송
	void Send(CP2PRelayPeerEx* pcP2PRelayPeerEx, int nReceiverIdx, RUDPHeader* header);

	///그룹에 입장 처리
	void Enter(int nGroupNo, CP2PRelayPeerEx* pcP2PRelayPeerEx);

	///그룹에서 퇴장 처리
	void Exit(CP2PRelayPeerEx* pcP2PRelayPeerEx);

	///싱글턴으로 생성
	STATIC_SINGLETON_PTR(CP2PRelay);

	/**
	@brief 크리티컬 섹션 진입
	*/
	inline void Enter(){m_CriticalSection.Enter();}

	/**
	@brief 크리티컬 섹션 퇴장
	*/
	inline void Leave(){m_CriticalSection.Leave();}
};

inline CP2PRelay*  GetP2PRelay()
{
	return CP2PRelay::InstancePtr();
}

} //namespace Redmoon