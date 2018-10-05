#pragma once

#include "Common.h"

namespace Redmoon
{

const int MAX_TICK = (60 * 1000) * 10; //10분

/**
@brief Tick단위로 시간 계산하는 클래스.
*/
class CTick
{
	///현재 기준이 되는 틱
	int m_nTickCount;

	///유효한 틱 범위
	int m_nMaxTick;

public:
	/**
	@brief 생성자. 유효 범위 틱을 포함한, 변수들을 초기화한다.
	@param nMaxTick
	*/
	CTick(int nMaxTick = MAX_TICK) : m_nMaxTick(nMaxTick), m_nTickCount(GetTickLimit49Day())
	{
	}


	/**
	@brief 소멸자. 아무일도 하지 않는다.
	*/
	~CTick()
	{
	}
		
	/**
	@brief 틱 검사 함수
	@param dwCurTick 현재 틱
	@return 유효 범위 내에 있는지 여부를 반환
	*/
	bool CheckTick(DWORD dwCurTick)
	{
		if(abs(dwCurTick - m_nTickCount) >= m_nMaxTick)	//m_nMaxTick만큼의 시간동안 ResetTick()이 안불리면 return false한다.
		{
			return false;
		}
		return true;
	}

	/**
	@brief 기준 틱을 현재 시간으로 변환한다.
	*/
	void ResetTick()
	{
		InterlockedExchange((LONG*)&m_nTickCount, (LONG)GetTickLimit49Day());
	}
};

} //namespace Redmoon