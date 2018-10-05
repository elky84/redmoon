#pragma once

#include <map>
#include <queue>
#include <set>
#include "Singleton.h"
#include "CriticalSection.h"

namespace Redmoon
{

/// Value: 유휴 메모리 포인터
typedef std::queue<void*> STL_QUEUE_MEMORY; 

/// Key: 유휴 메모리의 크기, Value: 유휴 메모리 큐
typedef std::map<size_t, STL_QUEUE_MEMORY> STL_MAP_QUEUEMEMORY; 

/**
@brief 메모리들의 목록을 관리하며, 메모리의 크기로 map이 정렬하고, 
Get함수가 요청한 크기의 버퍼를 반환함으로써 
힙으로 부터 메모리가 재할당되는 상황을 줄이는 역할을 하는 메모리풀 클래스다.
*/
class CMemoryManager
{
	STATIC_SINGLETON_PTR(CMemoryManager);

private:
	/// 할당되어있지만 사용되고 있지 않은 유휴 메모리의 맵
	STL_MAP_QUEUEMEMORY m_stl_map_QueueMemory;

	///동기화 객체
	CCriticalSection m_CriticalSection;

public:
	/**
	@brief 추후에 여러개의 메모리 공간을 미리 할당 해두는 함수
	@param nSize 할당할 메모리 크기
	@param nCount 할당할 메모리 갯수
	*/
	inline void Alloc(size_t nSize, size_t nCount)
	{
		m_CriticalSection.Enter();
		STL_QUEUE_MEMORY& stl_queue_memory = _GetQueue(nSize);
		for(size_t i = 0; i < nCount; i++)
		{
			stl_queue_memory.push(::operator new(nSize));
		}
		m_CriticalSection.Leave();
	}

	/**
	@brief 메모리를 할당해서 반환하는 메소드
	*/
	inline void* Get(size_t nSize)
	{
		STL_QUEUE_MEMORY& stl_queue_memory = _GetQueue(nSize);
		void* pData = NULL;
		m_CriticalSection.Enter();
		if(stl_queue_memory.empty())
		{
			m_CriticalSection.Leave();
			return ::operator new(nSize);
		}
		else
		{
			pData = stl_queue_memory.front();
			stl_queue_memory.pop();
			m_CriticalSection.Leave();
			return pData;
		}
	}

	/**
	@brief 사용이 끝난 메모리를 관리자에게 반환하는 메소드
	*/
	inline void Return(size_t nSize, void* pData)
	{
		STL_QUEUE_MEMORY& stl_queue_memory = _GetQueue(nSize);
		m_CriticalSection.Enter();
		stl_queue_memory.push(pData);
		m_CriticalSection.Leave();
	}

	/**
	@brief 모든 할당된 메모리 삭제하는 메소드. 
	@desc 사용중인 것은 관리하지 않고 있기에, 안지운다.
	*/
	inline void Clear()
	{
		for(STL_MAP_QUEUEMEMORY::iterator it = m_stl_map_QueueMemory.begin(); it != m_stl_map_QueueMemory.end(); ++it)
		{
			STL_QUEUE_MEMORY& stl_queue_memory = it->second;
			while(!stl_queue_memory.empty())
			{
				void* pData = stl_queue_memory.front();
				stl_queue_memory.pop();
				delete pData;
			}
		}
	}

private:
	/**
	@brief 직접적인 생성이 불가능하도록 private 설정.
	*/
	CMemoryManager()
	{

	}

	/**
	@brief static 객체이기에, 프로그램 종료 과정에서만 불려진다.
	*/
	~CMemoryManager()
	{
		Clear();
	}

	/**
	@brief 사이즈에 맞는 큐 주기
	@param nSize 반환할 메모리 크기
	@return 사이즈와 일치하는 큐
	*/
	inline STL_QUEUE_MEMORY& _GetQueue(size_t nSize)
	{
		STL_MAP_QUEUEMEMORY::iterator it = m_stl_map_QueueMemory.find(nSize);
		if( it == m_stl_map_QueueMemory.end() )
		{
			STL_QUEUE_MEMORY stl_queue_Memory;
			m_stl_map_QueueMemory.insert(STL_MAP_QUEUEMEMORY::value_type(nSize, stl_queue_Memory));
			return _GetQueue(nSize);
		}
		else
		{
			return it->second;
		}
	}
};

} //namespace Redmoon