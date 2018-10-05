#pragma once

namespace Redmoon
{

/**
@brief 비트 합 메소드
@param dest 기본 값
@param src 대상 값
@return 결과 값
*/
template <typename T>
inline int BitSum(const T &dest, const T &src)
{
	return dest | src;
}

/**
@brief 비트 논리합 메소드
@param dest 기본 값
@param src 대상 값
@return 결과 값
*/
template <typename T>
inline int BitXor(const T &dest, const T &src)
{
	return dest ^ src;
}

/**
@brief 두 변수간에 비트 연산을 한다.
@param dest 기본 값
@param src 대상 값
@return 두 변수가 같은 값인지 여부 (비트로)
*/
template <typename T>
inline bool BitCompare(const T &dest, const T &src)
{
	if((dest & src) == src)
	{
		return true;
	}

	return false;
}

/**
@brief 특정 위치의 비트를 검사한다. (1인지 여부)
@param value 대상 변수
@param n 이 값에 해당하는 위치의 비트를 대상으로 한다.
@return n번째 비트가 켜져있는지 여부
*/
template <typename T>
inline bool BitState(const T &refValue, const unsigned short &n)
{
	if(refValue & n)
	{
		return true;
	}
	return false;
}

/**
@brief 특정 위치의 비트를 켠다. (1로 만듬)
@param value 대상 변수
@param n 이 값에 해당하는 위치의 비트를 대상으로 한다.
*/
template <typename T>
inline void BitOn(T &refValue, const unsigned short &n)
{
	refValue |= (1 << (n - 1));
}

/**
@brief 특정 위치의 비트를 끈다. (0으로 만듬)
@param value 대상 변수
@param n 이 값에 해당하는 위치의 비트를 대상으로 한다.
*/
template <typename T>
inline void BitOff(T &refValue, const unsigned short &n)
{
	refValue &= ~(1 << (n - 1));
}

/**
@brief 특정 위치의 비트를 반전 시킨다
@param value 대상 변수
@param n 이 값에 해당하는 위치의 비트를 대상으로 한다.
*/
template <typename T>
inline void BitNot(T & refValue, const unsigned short &n)
{
	refValue ^= (1 << (n - 1));
}

} //namespace Redmoon