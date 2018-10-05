#pragma once

#include "Peer.h"
#include <map>
#include "Session.h"
#include "CriticalSection.h"

namespace Redmoon
{

//Key:CPeer.m_nIdx, value: CPeer*인 맵
typedef std::map<int, CPeer*> STL_MAP_Peer;

const int INVALID_PEER_IDX = 0xFFFFFFFF;

class CPeerManager
{
public:
	///생성자
	CPeerManager();

	///소멸자
	~CPeerManager();

	///피어를 등록한다.
	void Set(int nIdx, CPeer* pcPeer);

	///피어를 등록해제한다.
	void Release(int nIdx);

	///Active한 피어중에 nIdx를 가진 피어를 찾는 메소드
	CPeer* Get(int nIdx);

	///임의의 피어를 반환하는 메소드
	CPeer* GetRandPeer();

	///현재 사용중인 CPeer들의 Act를 불러주는 메소드
	void Act();

	///같은 그룹 (같은 IocpInterface의 그룹 내)의 Peer들에게 브로드 캐스트 한다.
	void BroadCast(int nLen, void* data, int nAddLen = 0, void* adddata = 0);

	/**
	@brief 크리티컬 섹션 진입
	*/
	inline void Enter(){m_CriticalSection.Enter();}

	/**
	@brief 크리티컬 섹션 퇴장
	*/
	inline void Leave(){m_CriticalSection.Leave();}

private:
	///모든 데이터를 없앤다
	void _Clear();

protected:
	//모든 CPeer맵
	STL_MAP_Peer m_stl_map_Peer;

	///크리티컬 섹션
	CCriticalSection m_CriticalSection;
};

} //namespace Redmoon