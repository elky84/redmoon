#include "P2PAsyncSelect.h"
#include "Buffer.h"
#include "P2PPeer.h"
#include <MMSystem.h>


namespace Redmoon
{

/**
@brief 생성자
*/
CP2PAsyncSelect::CP2PAsyncSelect() :CP2PInterface(), CAsyncSelectInterface(m_Socket)
{
	 
}

/**
@brief 소멸자
*/
CP2PAsyncSelect::~CP2PAsyncSelect()
{
}

/**
@brief AsyncSelect에서는 완성된 데이터이기에 바로 푼다.
@param header 전달받은 패킷 헤더
*/
void CP2PAsyncSelect::RecvPacket(RUDPHeader* header)
{
	CP2PInterface::OnReceive(header->GetLen(), header);
}

}