#include "P2PEventSelect.h"
#include "Buffer.h"
#include "P2PPeer.h"
#include <MMSystem.h>

namespace Redmoon
{

/**
@brief 생성자
*/
CP2PEventSelect::CP2PEventSelect() : CP2PInterface(), CEventSelectInterface(m_Socket)
{
}

/**
@brief 소멸자
*/
CP2PEventSelect::~CP2PEventSelect()
{
	OnDisconnect();
}

/**
@brief 완성된 패킷을 처리하는 메소드
@param nLen 패킷 크기
@param pData 데이터 포인터
@return 성공 여부
*/
bool CP2PEventSelect::OnReceive(unsigned int len, void* data)
{
	return CP2PInterface::OnReceive(len, data);
}

/**
@brief 데이터 수신 완료시 불려지는 메소드
@return 처리 결과
*/
bool CP2PEventSelect::OnReceive()
{
	return CP2PInterface::OnReceive();
}

/**
@brief 데이터 송신 완료시 불려지는 메소드
@return 처리 결과
*/
bool CP2PEventSelect::OnSend()
{
	return true;
}

/**
@brief UDP 소켓 바인드
@param nPort 바인드 할 포트
@return 처리 결과
*/
bool CP2PEventSelect::Bind(int nPort)
{
	if(CUDPSocket::Bind(nPort))
	{
		OnConnect(); //성공했을때 등록해야, Bind가 성공한 Socket을 EventSelect에 사용한다.
		return true;
	}
	return false;
}

/**
@brief 완성된 패킷의 처리 메소드. 큐에 패킷을 집어넣는다.
@param header 패킷 포인터
@desc 집어넣은 패킷은 OnReceive를 통해 불려진다.
*/
void CP2PEventSelect::RecvPacket(RUDPHeader* header)
{
	CEventSelectInterface::Push(new CBuffer(header->GetLen(), header));
}

} //namespace Redmoon
