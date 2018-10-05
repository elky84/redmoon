#pragma once

#include "Buffer.h"
#include <queue>
#include "AutoPtr.h"
#include "CriticalSection.h"

namespace Redmoon
{

///기본 WSASend 대기 큐 크기
const int DEFAULT_SEND_BUF_SIZE = 10;

///기본 WSASend에서 사용할 STL QUEUE 크기
const int DEFAULT_WAITING_STL_QUEUE_SIZE = 100;

class CIOContext;
class CSession;

struct WSABUFEx : public WSABUF
{
public:
	WSABUFEx()
	{
		buf = NULL;
		len = 0;
	}
};

/**
@brief IOCP에서 IO처리를 하는 데에 사용되는 기능을 담은 베이스 클래스.
*/
class CIocpEvent 
{
public:
	/**
	@brief 소멸자.
	*/
	virtual ~CIocpEvent()
	{
		
	}

	/**
	@brief 초기화 메소드
	*/
	virtual void Init()
	{
		if(m_pcBuffer)
			m_pcBuffer->Init();
	}

	/**
	@brief 해제 메소드
	*/
	virtual void Release()
	{
		SAFE_DELETE(m_pcBuffer);
	}
	
	/**
	@brief 사용가능한 가장 앞 위치의 포인터를 반환하는 메소드
	@return 사용가능한 가장 앞 위치 포인터
	*/
	inline char* GetHeadBuffer()
	{
		if(m_pcBuffer)
		{
			return m_pcBuffer->GetHeadBuffer();
		}
		return NULL;
	}

	/**
	@brief 어떤 종류의 IO를 담당할지를 반환하는 메소드
	@return 어떤 종류의 IO를 담당하는지
	*/
	inline IO_TYPE GetIoType(){return m_eIOType;}

	/**
	@brief 버퍼의 크기를 알아오는 메소드
	@return 버퍼의 크기
	*/
	inline unsigned int GetSize(){return m_pcBuffer->GetSize();}

	/**
	@brief 버퍼에서 사용중인 크기를 알아오는 메소드
	@return 버퍼에서 사용중인 크기
	*/
	inline unsigned int GetUsingSize(){return m_pcBuffer->GetUsingSize();}

	/**
	@brief IO에 사용하는 WSABUF의 포인터를 반환하는 메소드
	@return IO에 사용하는 WSABUF의 포인터
	*/
	inline LPWSABUF GetWsaBufPtr(){return &m_WsaBuf;}

	/**
	@brief IO시작하기 전에 불러야 되는 메소드
	*/
	virtual void Begin(){}

	/**
	@brief IO가 끝났을때 불려지는 메소드
	*/
	virtual void End(){}

	/**
	@brief 데이터 추가 메소드
	@param nLen pData의 크기
	@param pData 기록할 데이터
	@param nAddLen pAddData의 크기
	@param pAddData 기록할 추가 데이터
	@return 데이터 추가 결과. (재정의 하지 않으면 무조건 실패)
	@desc pData가 버퍼의 앞쪽, pAddData가 pData의 뒤쪽에 기록된다.
	*/
	virtual bool AddData(const int len, const void* data, const int nAddLen = 0, const void* pAddData = 0) {return false;}

	/**
	@brief 데이터를 얼만큼 읽었는지 전달하는 메소드.
	@param nLen 읽은 크기
	@return 성공 여부. (재정의 하지 않으면 무조건 실패)
	*/
	virtual bool Read(int nLen){return false;}

	/**
	@brief 데이터를 기록한 후 부르는 메소드
	@param nLen 기록한 크기
	@return 성공 여부. (재정의 하지 않으면 무조건 실패)
	*/
	virtual bool Write(int nLen){return false;}

	/**
	@brief CBuffer 반환 메소드
	@return CBuffer
	*/
	inline CBuffer* GetBuffer(){return m_pcBuffer;}

protected:
	/**
	@brief 생성자.
	@param eIOType 어떤 종류의 IO를 담당할 객체인지
	@param pcBuffer IO에 사용될 버퍼 포인터
	@param nWsaBufSize 사용될 WSABUF 크기
	@desc 상속으로만 사용가능하게 하기 위해서 protected로 설정
	*/
	CIocpEvent(IO_TYPE eIOType, CBuffer* pcBuffer) : m_eIOType(eIOType), m_pcBuffer(pcBuffer)
	{
	}


protected:
	///어떤 종류의 IO인지를 알기 위함
	IO_TYPE m_eIOType;
	
	///버퍼 포인터
	CBuffer* m_pcBuffer;

	///IOCP에서 사용되는 버퍼 형식 포인터
	WSABUFEx m_WsaBuf;
};

/**
@brief IOCP의 Send에 이용되는 전담 클래스
*/
class CIocpEventSend : public CIocpEvent
{
public:
	/**
	@brief 생성자. 버퍼를 입력받는다.
	@param pcBuffer 사용할 버퍼
	*/
	CIocpEventSend(CBuffer* pcBuffer = new CBuffer(MAX_CLIENT_PACKET_BUFFER_SIZE)) : 
	  CIocpEvent(IOTYPE_SEND, pcBuffer)
	{
		Init();
	}

	/**
	@brief 소멸자. Release 메소드를 호출한다.
	*/
	virtual ~CIocpEventSend()
	{
		Release();
	}

	/**
	@brief 초기화 메소드
	*/
	virtual void Init()
	{
		CScopeCriticalSection lock(m_CriticalSection);
		__super::Init();
	}

	/**
	@brief 해제 메소드
	*/
	virtual void Release()
	{
		__super::Release();
	}

	/**
	@brief IO 끝나고 압축하는 메소드
	*/
	virtual void End()
	{
		CScopeCriticalSection lock(m_CriticalSection);
		m_pcBuffer->Compress();
		BufferToWSABuf();
	}

	/**
	@brief IO 시작시 버퍼로 세팅하는 메소드
	*/
	virtual void Begin()
	{
		CScopeCriticalSection lock(m_CriticalSection);
		BufferToWSABuf();
	}
	
	/**
	@brief 데이터를 버퍼에서 읽는 작업이 끝났다는 메소드.
	@param nLen 읽은 데이터 크기
	@return 성공 여부
	*/
	virtual bool Read(int nLen)
	{
		CScopeCriticalSection lock(m_CriticalSection);
		return m_pcBuffer->JumpHead(nLen);
	}

	/**
	@brief 버퍼에서 WSABuf로 세팅해주는 메소드
	*/
	void BufferToWSABuf()
	{
		CScopeCriticalSection lock(m_CriticalSection);
		m_WsaBuf.buf = m_pcBuffer->GetHeadBuffer();
		m_WsaBuf.len = (MAX_ONE_PACKET_SIZE <= m_pcBuffer->GetUsingSize()) ? MAX_ONE_PACKET_SIZE : m_pcBuffer->GetUsingSize(); //MAX_ONE_PACKET_SIZE보다 크면, MAX_ONE_PACKET_SIZE만큼만 SEND를 건다.
	}

	/**
	@brief 데이터 버퍼에 추가하는 메소드
	@param nLen pData의 크기
	@param pData 기록할 데이터
	@param nAddLen pAddData의 크기
	@param pAddData 기록할 추가 데이터
	@return 데이터 추가 결과
	@desc pData가 버퍼의 앞쪽, pAddData가 pData의 뒤쪽에 기록된다.
	*/
	virtual bool AddData(const int nLen, const void* data, const int nAddLen = 0, const void* pAddData = 0)
	{
		CScopeCriticalSection lock(m_CriticalSection);
		return m_pcBuffer->AddData(nLen, data, nAddLen, pAddData);
	}

private:
	///동기화 객체.
	CCriticalSection m_CriticalSection;
};

/**
@brief IOCP의 Recv에 이용되는 전담 클래스
*/
class CIocpEventRecv : public CIocpEvent
{
public:
	/**
	@brief 생성자. 버퍼를 입력받는다.
	@param pcBuffer 사용할 버퍼
	*/
	CIocpEventRecv(CBuffer* pcBuffer) : CIocpEvent(IOTYPE_RECV, pcBuffer)
	{
		Init();
	}

	/**
	@brief 소멸자. Release 메소드를 호출한다.
	*/
	virtual ~CIocpEventRecv()
	{
		Release();
	}

	/**
	@brief 초기화 메소드
	*/
	virtual void Init()
	{
		m_pcBuffer->Init();
		BufferToWSABuf();
	}

	/**
	@brief 버퍼에서 WSABuf로 세팅해주는 메소드
	*/
	void BufferToWSABuf()
	{
		m_WsaBuf.buf = m_pcBuffer->GetTailBuffer();
		m_WsaBuf.len = m_pcBuffer->GetRemainSize();
	}

	/**
	@brief IO 끝나고 압축하는 메소드
	*/
	virtual void End()
	{
		m_pcBuffer->Compress();
		BufferToWSABuf();
	}

	/**
	@brief 데이터를 버퍼에서 읽는 작업이 끝났다는 메소드.
	@param nLen 읽은 데이터 크기
	@return 성공 여부
	*/
	virtual bool Read(int nLen)
	{
		return m_pcBuffer->JumpHead(nLen);
	}

	/**
	@brief 데이터를 버퍼에서 쓰는 작업이 끝났다는 메소드.
	@param nLen 쓴 데이터 크기
	@return 성공 여부
	*/
	virtual bool Write(int nLen)
	{
		return m_pcBuffer->JumpTail(nLen);
	}
};

/**
@brief IOCP의 Connect/Accept에 이용되는 전담 클래스
*/
class CIocpEventConnect : public CIocpEvent
{
public:
	CIocpEventConnect() : CIocpEvent(IOTYPE_CONNECT, new CBuffer(ACCEPT_BUFFER_SIZE))
	{
		Init();
	}

	virtual ~CIocpEventConnect()
	{
		Release();
	}
};

/**
@brief IOCP의 Reuse에 이용되는 전담 클래스
*/
class CIocpEventReuse : public CIocpEvent
{
public:
	CIocpEventReuse() : CIocpEvent(IOTYPE_REUSE, NULL)
	{
		Init();
	}

	virtual ~CIocpEventReuse()
	{
		Release();
	}
};

/**
@brief IOCP에 등록되고, 리턴되는 클래스.
*/
class CIocpContext : public STRUCT_EX<OVERLAPPED>//이 클래스는 가상 함수 만들면 안된다. 왜냐고? OVERLAPPED의 위치가 가상 함수 테이블 크기만큼 밀려서 관리하기 어려워진다.
{
public:
	/**
	@brief 생성자.
	@param pcSession 나를 사용하는 CSession 포인터
	@param pcIocpEvent IO 전담 클래스 포인터
	*/
	CIocpContext(CSession* pcSession, CIocpEvent* pcIocpEvent) : m_pcSession(pcSession), m_pcIocpEvent(pcIocpEvent)
	{
	}

	/**
	@brief 소멸자. 동적할당한 데이터를 해제한다.
	*/
	~CIocpContext()
	{
		SAFE_DELETE(m_pcIocpEvent);
	}

	/**
	@brief 초기화 메소드
	*/
	inline void Init()
	{
		m_pcIocpEvent->Init();
	}

	/**
	@brief 소유하고 있는 IO 전담 클래스 포인터를 반환하는 메소드
	@return IO 전담 클래스 포인터
	*/
	inline CIocpEvent* GetSocketIOEvent(){return m_pcIocpEvent;}

	/**
	@brief 나를 사용하는 CSession 포인터를 반환하는 메소드
	@return 나를 사용하는 CSession 포인터
	*/
	inline CSession* GetSession(){return m_pcSession;}

	/**
	@brief 버퍼의 크기를 알아오는 메소드
	@return 버퍼의 크기
	*/
	inline unsigned int GetSize(){return m_pcIocpEvent->GetSize();}

	/**
	@brief 버퍼에서 사용중인 크기를 알아오는 메소드
	@return 버퍼에서 사용중인 크기
	*/
	inline unsigned int GetUsingSize(){return m_pcIocpEvent->GetUsingSize();}

	/**
	@brief 버퍼가 꽉찼는지 알아내는 메소드
	@return 버퍼가 꽉찼는지 여부
	*/
	inline bool isFull(){return GetUsingSize() == GetSize();}


	/**
	@brief 데이터를 얼만큼 읽었는지 전달하는 메소드.
	@param nLen 읽은 크기
	@return 성공 여부. 
	*/
	inline bool Read(int nLen){return m_pcIocpEvent->Read(nLen);}

	/**
	@brief 데이터를 기록한 후 부르는 메소드
	@param nLen 기록한 크기
	@return 성공 여부.
	*/
	inline bool Write(int nLen){return m_pcIocpEvent->Write(nLen);}

	/**
	@brief IO에 사용하는 WSABUF의 포인터를 반환하는 메소드
	@return IO에 사용하는 WSABUF의 포인터
	*/
	inline LPWSABUF GetWsaBufPtr(){return m_pcIocpEvent->GetWsaBufPtr();}

	/**
	@brief 데이터 추가 메소드
	@param nLen pData의 크기
	@param pData 기록할 데이터
	@param nAddLen pAddData의 크기
	@param pAddData 기록할 추가 데이터
	@return 데이터 추가 결과. (재정의 하지 않으면 무조건 실패)
	@desc pData가 버퍼의 앞쪽, pAddData가 pData의 뒤쪽에 기록된다.
	*/
	inline bool AddData(const int len, const void* data, const int nAddLen = 0, const void* pAddData = 0) {return m_pcIocpEvent->AddData(len, data, nAddLen, pAddData);}

	/**
	@brief IO가 시작하려 할때 불러야되는 메소드
	*/
	inline void Begin(){return m_pcIocpEvent->Begin();}


	/**
	@brief IO가 끝났을때 불려지는 메소드
	*/
	inline void End(){return m_pcIocpEvent->End();}

	/**
	@brief 사용가능한 가장 앞 위치의 포인터를 반환하는 메소드
	@return 사용가능한 가장 앞 위치 포인터
	*/
	inline char* GetHeadBuffer(){return m_pcIocpEvent->GetHeadBuffer();}

	/**
	@brief CBuffer 반환 메소드
	@return CBuffer
	*/
	inline CBuffer* GetBuffer(){return m_pcIocpEvent->GetBuffer();}

	/**
	@brief 어떤 종류의 IO를 담당할지를 반환하는 메소드
	@return 어떤 종류의 IO를 담당하는지
	*/
	inline IO_TYPE GetIoType(){return m_pcIocpEvent->GetIoType();}


private:
	///IO 전담 클래스 포인터
	CIocpEvent* m_pcIocpEvent;

	///나를 사용하는 CSession 포인터
	CSession* m_pcSession;
};

} //namespace Redmoon