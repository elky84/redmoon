#pragma once

#include "SocketCommon.h"
#include <map>
#include <queue>
#include "CriticalSection.h"
#include "Singleton.h"

namespace Redmoon
{

class CPeer;
class CSession;

class CSessionManager : public CSingleton<CSessionManager>
{
	///key: SocketIdx, value: CSession 포인터를 담은 맵의 typedef형
	typedef std::map<int, CSession*> STL_MAP_SESSION;

	///key: SocketIdx, value: CSession 포인터를 담은 맵.
	STL_MAP_SESSION m_stl_map_UsingSession;

	///현재 사용중이지 않은 세션모음.
	std::queue<CSession* > m_stl_queue_NotUseSession;

	///Using+NotUse 합한 갯수. (가능한 최대 갯수)
	int m_nTotalCountSession;	

	///크리티컬 섹션
	CCriticalSection m_CriticalSection;

public:
	///생성자
	CSessionManager();

	///소멸자
	~CSessionManager();

	///사용하고 싶으니 하나 꺼내달라는 메소드
	CSession* Pop();

	///사용 끝났으니 넣어두겠다는 메소드
	void Push(CSession* pcSession);

	///특정 CSession를 찾는 메소드
	CSession* Get(int nIdx);

	///새로 CSession를 할당하는 메소드
	void New(int nCount);

	/**
	@brief 크리티컬 섹션 진입
	*/
	inline void Enter(){m_CriticalSection.Enter();}

	/**
	@brief 크리티컬 섹션 퇴장
	*/
	inline void Leave(){m_CriticalSection.Leave();}
};

} //namespace Redmoon
