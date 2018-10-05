#pragma once

#include "P2PCommon.h"
#include "P2PInterface.h"
#include "EventSelectInterface.h"

namespace Redmoon
{

class CTick;
class CP2PPeer;

/**
@brief P2P기능을 EventSelect로 제공 받는 클래스
*/
class CP2PEventSelect : public CP2PInterface, public CEventSelectInterface
{
public:
	///생성자
	CP2PEventSelect();

	///소멸자
	virtual ~CP2PEventSelect();

	///완성된 패킷을 처리하는 메소드
	virtual bool OnReceive(unsigned int nLen, void* pData);

	///데이터 수신 완료시 불려지는 메소드
	virtual bool OnReceive();

	///데이터 송신 완료시 불려지는 메소드
	virtual bool OnSend();

	///Udp 소켓 바인드
	virtual bool Bind(int nPort);
	
	///완성된 패킷의 처리 메소드
	virtual void RecvPacket(RUDPHeader* header);
};

} //namespace Redmoon
