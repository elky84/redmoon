#pragma once

#include "Common.h"

namespace Redmoon
{

/**
@brief 난수 생성기 클래스입니다.
*/
class CRandom
{
private:
	/**
	@brief 빈 생성자. 실제로는 호출되지 않습니다.
	*/
	CRandom::CRandom()
	{
	}

private:
	///시드 값
	static UINT32 m_unSeed;

public:
	/**
	@brief 시드값 설정 함수
	*/
	static void CRandom::ResetRandomSeed(UINT32 unSeed)
	{
		m_unSeed = unSeed;
	}

	/**
	@brief 실수 난수 생성 함수
	@return 0~1의 범위를 가지는 실수 반환
	*/
	static float CRandom::RandFloat() 
	{
		return static_cast<float>(Random()) / 0x7fff;
	}

	/**
	@brief 난수 생성 함수
	@return 생성된 난수
	*/
	static INT16 CRandom::Random()
	{
		return static_cast<INT16>(((m_unSeed = m_unSeed * 214013L + 2531011L) >> 16) & 0x7fff);
	}

	/**
	@brief 난수 생성 함수
	@param nTo 0~nTo -1 의 범위를 가지는 난수를 반환한다
	@return 생성된 난수
	*/
	static INT16 CRandom::Random(INT16 nTo)
	{
		return Random() % nTo;
	}

	/**
	@brief 난수 생성 함수
	@param nMin 얻어내고 싶은 난수의 최소값
	@param nMax 얻어내고 싶은 난수의 최대값
	@return 생성된 난수
	*/
	static INT16 CRandom::Random(INT16 nMin, INT16 nMax)
	{
		return Random() % (nMax - nMin + 1) + nMin;
	}

	/**
	@brief 복잡한 난수 생성 함수
	@return 생성된 난수
	*/
	static INT32 CRandom::ComplexRandom()
	{
		INT32 result;
		m_unSeed *= 1103515245;
		m_unSeed += 12345;
		result = (m_unSeed / 65536) % 2048;

		m_unSeed *= 1103515245;
		m_unSeed += 12345;
		result <<= 10;
		result ^= (m_unSeed / 65536) % 1024;

		m_unSeed *= 1103515245;
		m_unSeed += 12345;
		result <<= 10;
		result ^= (m_unSeed / 65536) % 1024;

		m_unSeed = m_unSeed;
		return result;
	}

	/**
	@brief 최대값 지정 복잡한 난수 생성 함수
	@param nMax 얻어내고 싶은 난수의 최대값
	@return 생성된 난수
	*/
	static INT32 CRandom::ComplexRandom(INT32 nMax)
	{
		return ComplexRandom() % nMax;
	}

	/**
	@brief 범위 지정 복잡한 난수 생성 함수
	@param nMin 얻어내고 싶은 난수의 최소값
	@param nMax 얻어내고 싶은 난수의 최대값
	@return 생성된 난수
	*/
	static INT32 CRandom::ComplexRandom(INT32 nMin, INT32 nMax)
	{
		return ComplexRandom() % (nMax - nMin + 1) + nMin;

	}
};

__declspec(selectany) UINT32 CRandom::m_unSeed = static_cast<UINT32>(time(NULL));

} //namespace Redmoon