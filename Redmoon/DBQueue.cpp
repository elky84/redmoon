#include "DBQueue.h"

namespace Redmoon
{

/**
@brief 생성자. 각종 멤버를 초기화 한다.
*/
CDBQueue::CDBQueue() : m_stl_queue_Producer(new STL_QUEUE_DATA), m_stl_queue_Consumer(new STL_QUEUE_DATA), m_pcEvent(new CEvent), m_bActive(true)
{
}

/**
@brief 소멸자. 동적 할당한 메소드를 제거해준다.
*/
CDBQueue::~CDBQueue()
{
	SAFE_DELETE(m_stl_queue_Consumer);
	SAFE_DELETE(m_stl_queue_Producer);
}

/**
@brief Consumer와 Producer를 교환해준다.
*/
void CDBQueue::Swap()
{
	STL_QUEUE_DATA *pTemp = m_stl_queue_Consumer;

	_Enter();
	m_stl_queue_Consumer = m_stl_queue_Producer;
	m_stl_queue_Producer = pTemp;
	_Leave();
}

/**
@brief 큐에 데이터를 넣는 메소드
@param nSize 큐에 넣는 크기
@param data 데이터
*/
void CDBQueue::Push(int nSize, void *data)
{
	char *pData = new char[nSize];
	memcpy(pData, data, nSize);

	_Enter();
	m_stl_queue_Producer->push(pData);
	m_pcEvent->Set();
	_Leave();
}

/**
@brief 큐에서 데이터를 하나 빼서 리턴한다.
@return 큐에서 뺀 데이터
*/
char* CDBQueue::Pop()
{
	if(m_stl_queue_Consumer->empty())
	{
		Swap();
		if(m_stl_queue_Consumer->size())
		{
			return Pop();
		}
		return NULL;
	}
	char *pData = m_stl_queue_Consumer->front();
	m_stl_queue_Consumer->pop();
	return pData;
}

} //namespace Redmoon