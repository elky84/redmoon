#pragma once

#include "Common.h"

namespace Redmoon
{

/**
@brief 참조 값 관리를 쓰레드 세이프하게 만든 클래스
*/
class CSafeRefCount
{
	///참조 값
	LONG m_nRefCount;

protected:
	/**
	@brief 생성자. 참조 값 초기화.
	*/
	CSafeRefCount() : m_nRefCount(0)
	{
	}

	/**
	@brief 소멸자. 아무 일도 하지 않는다.
	*/
	virtual ~CSafeRefCount()
	{
	}

public:
	/**
	@brief 참조 값을 증가 시킨다.
	*/
	void IncRefCount()
	{
		InterlockedIncrement(&m_nRefCount);
	}

	/**
	@brief 참조 값을 감소 시키고, 참조 값이 0이되면 객체 소멸.
	@desc delete this에 따른 리스크 (여러가지 주의 사항) 존재
	*/
	void DecRefCount()
	{
		if(0 == InterlockedDecrement(&m_nRefCount))
			delete this;
	}
};

/**
@brief CSafeCount 기능을 사용하는 객체의 사용권한 반환을 편리하게 하기 위한 랩퍼
@desc 실제 클래스 타입을 template으로 넘겨라
*/
template <typename T>
class CSafeRefCountWrapper
{
	///실제 사용할 클래스 타입의 포인터
	T* m_pcObject;
public:
	/**
	@brief 생성자.
	@param pcObject 실제 사용할 클래스의 포인터
	@desc NULL이 넘어올 수도 있음에 주의
	*/
	CSafeRefCountWrapper(T *pcobject) : m_pcObject(pcobject)
	{
	}

	/**
	@brief 소멸자. 사용중인 클래스가 있으면 값을 감소 시킨다.
	*/
	~CSafeRefCountWrapper()
	{
		if(m_pcObject)
			m_pcObject->DecRefCount();
	}

	/**
	@brief 포인터 멤버 접근자 (->) 오퍼레이터 재 정의. 기본 클래스와 사용법을 일치 시키기 위해 재정의.
	@return 사용중인 객체 포인터
	*/
	T* operator ->()
	{
		return m_pcObject;
	}

	/**
	@brief 포인터 유효성 검사용 메소드
	@return 사용중인 객체 포인터
	@desc NULL검사에서 사용하라.
	*/
	T* Get()
	{
		return m_pcObject;
	}
};

} //namespace Redmoon