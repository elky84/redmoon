#include "EventSelectInterface.h"
#include "EventWorker.h"

namespace Redmoon
{

/**
@brief 생성자. 
@param refSocket 이벤트 검사에 사용되는 소켓 핸들 참조자
*/
CEventSelectInterface::CEventSelectInterface(const SOCKET& refSocket) : m_refSocket(refSocket), m_hEvent(WSACreateEvent())
{

}

/**
@brief 소멸자. 이벤트 워커에서 빼고, 큐에 담긴 패킷들 동적할당 해제한다.
*/
CEventSelectInterface::~CEventSelectInterface()
{
	if(GetSmartSingleton()->Unregister(this))
	{
		while(!m_stl_queue_Buffer.empty())
		{
			CBuffer* pcBuffer = m_stl_queue_Buffer.front();
			m_stl_queue_Buffer.pop();
			delete pcBuffer;
		}
		SAFE_CLOSE_HANDLE(m_hEvent);
	}
}

/**
@brief 이벤트가 생겼을때 불려지는 메소드
@desc 사용된 소켓과 WSAEVENT 핸들을 읽어서, Receive, Write, Close 이벤트 판단해서 해당 함수를 호출해준다.
*/
void CEventSelectInterface::Proc()
{
	WSANETWORKEVENTS wsaEvent;
	WSAEnumNetworkEvents(m_refSocket, m_hEvent, &wsaEvent);
	Enter();
	if(wsaEvent.lNetworkEvents & FD_READ)
	{
		OnReceive();
	}
	else if(wsaEvent.lNetworkEvents & FD_WRITE)
	{
		OnSend();
	}
	else if(wsaEvent.lNetworkEvents & FD_CLOSE)
	{
		OnDisconnect();
	}
	Leave();
}

/**
@brief 이벤트 워커에 등록하는 메소드
@param 성공 여부
*/
bool CEventSelectInterface::Register()
{
	if(GetSmartSingleton()->Register(this))
	{
		int nRet = WSAEventSelect(m_refSocket, m_hEvent, FD_WRITE | FD_READ | FD_CLOSE);
		if(nRet == SOCKET_ERROR)
			return false;

		return true;
	}
	return false;
}

/**
@brief 이벤트 워커에서 빼는 메소드
@param 성공 여부
*/
bool CEventSelectInterface::Unregister()
{
	if(GetSmartSingleton()->Unregister(this))
	{
		Pop();
		SAFE_CLOSE_HANDLE(m_hEvent);
		return true;
	}
	return false;
}

/**
@brief 접속 성공시 불려짐. 이벤트 워커에 등록
*/
void CEventSelectInterface::OnConnect()
{
	Register();
}

/**
@brief 접속 해제시 불려짐. 이벤트 워커에서 뺌.
*/
void CEventSelectInterface::OnDisconnect()
{
	Unregister();
}

/**
@brief 큐에 패킷 넣는 메소드
@param pcBuffer 큐에 쌓을 패킷
*/
void CEventSelectInterface::Push(CBuffer* pcBuffer)
{
	Enter();
	m_stl_queue_Buffer.push(pcBuffer);
	Leave();
}

/**
@brief 큐에 패킷 빼는 메소드
@desc 큐에 쌓인 모든 패킷을 뺌.
*/
void CEventSelectInterface::Pop()
{
	Enter();
	std::queue<CBuffer*> stl_queue_Buffer = m_stl_queue_Buffer;
	while(!m_stl_queue_Buffer.empty())
		m_stl_queue_Buffer.pop();
	Leave();

	while(!stl_queue_Buffer.empty())
	{
		CBuffer* pcBuffer = stl_queue_Buffer.front();
		stl_queue_Buffer.pop();
		OnReceive(pcBuffer->GetUsingSize(), pcBuffer->GetHeadBuffer());
		delete pcBuffer;
	}
}

} //namespace Redmoon