#pragma once

#include "Common.h"

namespace Redmoon
{

/**
@brief ���� ������ Ŭ�����Դϴ�.
*/
class CRandom
{
private:
	/**
	@brief �� ������. �����δ� ȣ����� �ʽ��ϴ�.
	*/
	CRandom::CRandom()
	{
	}

private:
	///�õ� ��
	static UINT32 m_unSeed;

public:
	/**
	@brief �õ尪 ���� �Լ�
	*/
	static void CRandom::ResetRandomSeed(UINT32 unSeed)
	{
		m_unSeed = unSeed;
	}

	/**
	@brief �Ǽ� ���� ���� �Լ�
	@return 0~1�� ������ ������ �Ǽ� ��ȯ
	*/
	static float CRandom::RandFloat() 
	{
		return static_cast<float>(Random()) / 0x7fff;
	}

	/**
	@brief ���� ���� �Լ�
	@return ������ ����
	*/
	static INT16 CRandom::Random()
	{
		return static_cast<INT16>(((m_unSeed = m_unSeed * 214013L + 2531011L) >> 16) & 0x7fff);
	}

	/**
	@brief ���� ���� �Լ�
	@param nTo 0~nTo -1 �� ������ ������ ������ ��ȯ�Ѵ�
	@return ������ ����
	*/
	static INT16 CRandom::Random(INT16 nTo)
	{
		return Random() % nTo;
	}

	/**
	@brief ���� ���� �Լ�
	@param nMin ���� ���� ������ �ּҰ�
	@param nMax ���� ���� ������ �ִ밪
	@return ������ ����
	*/
	static INT16 CRandom::Random(INT16 nMin, INT16 nMax)
	{
		return Random() % (nMax - nMin + 1) + nMin;
	}

	/**
	@brief ������ ���� ���� �Լ�
	@return ������ ����
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
	@brief �ִ밪 ���� ������ ���� ���� �Լ�
	@param nMax ���� ���� ������ �ִ밪
	@return ������ ����
	*/
	static INT32 CRandom::ComplexRandom(INT32 nMax)
	{
		return ComplexRandom() % nMax;
	}

	/**
	@brief ���� ���� ������ ���� ���� �Լ�
	@param nMin ���� ���� ������ �ּҰ�
	@param nMax ���� ���� ������ �ִ밪
	@return ������ ����
	*/
	static INT32 CRandom::ComplexRandom(INT32 nMin, INT32 nMax)
	{
		return ComplexRandom() % (nMax - nMin + 1) + nMin;

	}
};

__declspec(selectany) UINT32 CRandom::m_unSeed = static_cast<UINT32>(time(NULL));

} //namespace Redmoon