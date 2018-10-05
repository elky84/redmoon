#pragma once

#include "Buffer.h"
#include "SocketInterface.h"
#include "AutoPtr.h"

namespace Redmoon
{

/**
@brief UDP 소켓을 사용하는 클래스들이 상속 받아야 할 클래스.
*/
class CUDPSocket : public CSocketInterface
{
public:
	///생성자
	CUDPSocket();

	///소멸자
	virtual ~CUDPSocket();

	///소켓 생성하는 메소드
	virtual bool Socket();

	///특정 포트로 소켓을 바인드 하는 메소드
	bool Bind(int nPort);

	///데이터 전송시 사용해야 할 메소드
	static bool Send(const SOCKET &refSocket, SOCKADDR_IN &sockaddrin, int nLen, void* pData);

	///패킷 받는데 사용해야 할 메소드
	CBuffer* Recv(sockaddr_in* sockaddrin);

private:
	///패킷 Recv에 사용하는 버퍼.
	CAutoPtr<CBuffer> m_pcRecvBuffer;
};

} //namespace Redmoon