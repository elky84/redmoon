#pragma once

#include "P2PCommon.h"
#include "P2PInterface.h"
#include "AsyncSelectInterface.h"

namespace Redmoon
{

class CTick;
class CP2PPeer;

/**
@brief P2P 기능을 AsyncSelect로 지원하는 클래스
*/
class CP2PAsyncSelect : public CP2PInterface, public CAsyncSelectInterface
{
public:
	///생성자
	CP2PAsyncSelect();

	///소멸자
	~CP2PAsyncSelect();

	///윈도우 핸들과 연결
	virtual void Set(HWND hWnd, unsigned int wMsg = WM_SOCKET, long lEvent = FD_READ){__super::Set(hWnd, wMsg, lEvent);}

	///완성된 패킷의 처리 메소드
	virtual void RecvPacket(RUDPHeader* header);
};

} //namespace Redmoon
