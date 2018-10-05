#include "Common.h"
#include "Buffer.h"

namespace Redmoon
{

/**
@brief 생성자.
@param nBufferSize 이 크기만큼 버퍼를 할당한다.
*/
CBuffer::CBuffer(BUFFER_SIZE nBufferSize) : m_pcBuffer(new char[nBufferSize]), m_nBufferSize(nBufferSize), m_nTail(0), m_nHead(0)
{
}

/**
@brief 복사 생성자
@param pcBuffer 해당 버퍼와 같은 크기로 버퍼를 만들고, 데이터도 복사한다.
*/
CBuffer::CBuffer(CBuffer* pcBuffer) : m_pcBuffer(new char[pcBuffer->GetUsingSize()]), m_nBufferSize(pcBuffer->GetUsingSize()), m_nTail(0), m_nHead(0)
{
	CBuffer::AddData(pcBuffer->GetUsingSize(), pcBuffer->GetHeadBuffer());
}

/**
@brief 복사 생성자
@param nLen pData의 크기
@param pData 기록할 데이터
@param nAddLen pAddData의 크기
@param pAddData 기록할 추가 데이터
*/
CBuffer::CBuffer(const BUFFER_SIZE nLen, const void* pData, const BUFFER_SIZE nAddLen /* = 0 */, const void* pAddData /* = 0 */) 
	: m_pcBuffer(new char[nLen + nAddLen]), m_nBufferSize(nLen + nAddLen), m_nTail(0), m_nHead(0)
{
	CBuffer::AddData(nLen, pData, nAddLen, pAddData);
}


/**
@brief 소멸자. 버퍼를 동적해제 한다.
*/
CBuffer::~CBuffer()
{
	SAFE_DELETE_ARRAY(m_pcBuffer);
}

/**
@brief 초기화 메소드. Head와 Tail값을 초기화한다. (버퍼 자체에 들어있는 데이터의 초기화는 안함)
*/
void CBuffer::Init()
{
	_InitHead(); 
	_InitTail();
}

/**
@brief Head값만큼 사용했다 판단하고, 사용후 처리를 한다.
@param nPosition 이 크기만큼 Head포인터를 이동한다.
@return nPosition만큼 Head위치 이동하는데 성공하는지 여부.
*/
bool CBuffer::JumpHead(BUFFER_POSITION nPosition)
{
	if(GetTail() < GetHead() + nPosition)
	{
		LOG_ERROR(_T("%s [%p] %d %d %d"), __TFUNCTION__, this, GetHead(), GetTail(), nPosition);
		return false;
	}
	_AddHead(nPosition);
	return true;
}

/**
@brief Tail값만큼 사용했다 판단하고, 사용후 처리를 한다.
@param nPosition 이 크기만큼 Tail포인터를 이동한다.
@return nPosition만큼 Tail위치 이동하는데 성공하는지 여부.
*/
bool CBuffer::JumpTail(BUFFER_POSITION nPosition)
{
	if(GetSize() < GetTail() + nPosition)
	{
		LOG_ERROR(_T("%s [%p] %d %d %d"), __TFUNCTION__, this, GetHead(), GetTail(), nPosition);
		return false;
	}

	_AddTail(nPosition);
	return true;
}

/**
@brief 데이터 압축 메소드.
*/
void CBuffer::Compress()
{
	if(GetHead() == GetTail()) //다 읽었다면
	{
		Init();
	}
	else // 아직 남았다면
	{
		BUFFER_SIZE nUsingDataSize = GetUsingSize();
		CopyMemory(GetBuffer(), GetHeadBuffer(), nUsingDataSize);
		_InitTail();
		_AddTail(nUsingDataSize);
		_InitHead();
	}
}

/**
@brief 버퍼에 데이터를 기록한다.
@param nLen pData의 크기
@param pData 기록할 데이터
@param nAddLen pAddData의 크기
@param pAddData 기록할 추가 데이터
@desc pData가 버퍼의 앞쪽, pAddData가 pData의 뒤쪽에 기록된다.
*/	
bool CBuffer::AddData(const BUFFER_SIZE nLen, const void* pData, const BUFFER_SIZE nAddLen /* = 0 */, const void* pAddData /* = 0 */)
{
	BUFFER_SIZE nAddDataLen = nLen + nAddLen;
	if(GetSize() < GetTail() + nAddDataLen)
	{
		LOG_ERROR(_T("%s [%p] %d %d %d"), __TFUNCTION__, this, GetSize(), GetTail(), nAddDataLen);
		return false;
	}

	if(pData)
	{
		CopyMemory(GetBuffer(GetTail()), pData, nLen);
	}

	if(pAddData)
	{
		CopyMemory(GetBuffer(GetTail() + nLen), pAddData, nAddLen);
	}
	_AddTail(nAddDataLen);
	return true;
}

} //namespace Redmoon