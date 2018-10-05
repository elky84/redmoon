#pragma once

#include "SocketInterface.h"

namespace Redmoon
{

/**
@brief TCP 소켓을 사용하는 클래스들이 상속 받아야 할 클래스.
*/
class CTCPSocket : public CSocketInterface
{
public:
	///생성자
	CTCPSocket();

	///소멸자
	virtual ~CTCPSocket();

	///소켓 생성 메소드
	virtual bool Socket();
};

} //namespace Redmoon
