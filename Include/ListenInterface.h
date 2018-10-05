#pragma once

#include "TCPSocket.h"

namespace Redmoon
{

/**
@brief Listen의 범용적인 기능을 제공하는 인터페이스
*/
class CListenInterface : public CTCPSocket
{
protected:
	///생성자
	CListenInterface(); 
	
	///소멸자
	virtual ~CListenInterface();

	///소켓에 해당 포트를 바인드
	bool Bind(int nPort);

	///Bind된 포트로 Listen한다. 
	bool Listen(int nWaitingQueue);
};

} //namespace Redmoon