#pragma once

#include "MemoryManager.h"
#include <queue>
#include "CriticalSection.h"


namespace Redmoon
{

/**
@brief 상속을 통해 메모리풀 기능을 제공하는 클래스.
@par 주의 사항 
상속을 통한 사용만 가능하도록 생성자와 소멸자가 protected이다.
*/
template <typename T>
class CMemoryPool
{
public:
	/**
	@brief operator new를 중복 정의함으로써, 상속 받은 클래스의 new 기능을 MemoryManager를 통해 동작하도록 한다.
	@param nSize 생성되는 개체의 크기
	@return 할당된 메모리
	*/
	static void *operator new(size_t nSize)
	{
		return CMemoryManager::InstancePtr()->Get(nSize);
	}

	/**
	@brief operator delete를 중복 정의 함으로써 MemoryManager 사용.
	@param pDelObj 소멸되는 개체
	@param nSize 소멸되는 개체의 크기
	*/
	static void operator delete(void* pDelobj, size_t nSize)
	{
		if( !pDelobj ) return;

		CMemoryManager::InstancePtr()->Return(nSize, pDelobj);
	}

protected:
	/**
	@brief 생성자를 protected로 선언함으로써 상속을 통한 사용만 가능하도록 한다
	*/
	CMemoryPool()
	{

	}
};

template <typename T>
class CObjectPool
{
private:
	/// 할당되어있지만 사용되고 있지 않은 유휴 메모리의 멀티맵
	static STL_QUEUE_MEMORY m_stl_queue_Memory;
	static CCriticalSection m_cs;

public:
	/**
	@brief operator new를 중복 정의함으로써, 상속 받은 클래스의 new 기능을 MemoryManager를 통해 동작하도록 한다.
	@param nSize 생성되는 개체의 크기
	@return 할당된 메모리
	*/
	static void *operator new(size_t nSize)
	{
		return _Get(nSize);
	}

	/**
	@brief operator delete를 중복 정의 함으로써 MemoryManager 사용.
	@param pDelObj 소멸되는 개체
	@param nSize 소멸되는 개체의 크기
	*/
	static void operator delete(void* pDelobj, size_t nSize)
	{
		if( !pDelobj ) return;

		_Return(pDelobj);
	}

	/**
	@brief 추후에 여러개의 메모리 공간을 미리 할당 해두는 함수
	@param nSize 할당할 메모리 크기
	@param nCount 할당할 메모리 갯수
	*/
	static void Alloc(size_t nCount)
	{
		for(size_t i = 0; i < nCount; i++)
		{
			_Return(::operator new(sizeof(T)));
		}
	}

	/**
	@brief 모든 할당된 메모리 삭제하는 메소드. 
	@desc 사용중인 것은 관리하지 않고 있기에, 안지운다.
	*/
	static void Clear()
	{
		m_cs.Enter();
		while(!m_stl_queue_Memory.empty())
		{
			void* pData = m_stl_queue_Memory.front();
			m_stl_queue_Memory.pop();
			delete pData;
		}
		m_cs.Leave();
	}

protected:
	/**
	@brief 생성자를 protected로 선언함으로써 상속을 통한 사용만 가능하도록 한다
	*/
	CObjectPool()
	{

	}

	/**
	@brief 소멸자가 virtual이 아닌 이유는 소멸자에게 하는 일이 없기 때문이다.
	*/
	~CObjectPool()
	{		
		
	}

private:
	/**
	@brief 메모리 매니저에게 메모리 할당 요청한다.
	@param nSize 생성 요청한 개체의 크기
	@return 할당된 메모리
	*/
	static void* _Get(size_t nSize)
	{
		m_cs.Enter();
		if( m_stl_queue_Memory.empty() )
		{
			m_cs.Leave();
			return ::operator new(nSize);
		}

		void *memory = m_stl_queue_Memory.front();
		m_stl_queue_Memory.pop();
		m_cs.Leave();
		return memory;
	}

	/**
	@brief 메모리 매니저에게 반환 요청한다.
	@param pMemory 반환할 메모리
	*/
	static void _Return(void *pMemory)
	{
		m_cs.Enter();
		m_stl_queue_Memory.push(pMemory);
		m_cs.Leave();
	}
};

template <typename T> STL_QUEUE_MEMORY CObjectPool<T>::m_stl_queue_Memory;
template <typename T> CCriticalSection CObjectPool<T>::m_cs;

} //namespace Redmoon